/*
  laser.cpp - Laser control library for Arduino using 16 bit timers- Version 1
  Copyright (c) 2013 Timothy Schmidt.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Laser.h"
#include "Configuration.h"
#include "ConfigurationStore.h"
#include "pins.h"
#include <avr/interrupt.h>
#include <Arduino.h>
#include "Marlin.h"

#define LASER_MAX           1       // in case of several laser
#define LASER_INTENSITY     0
#define LASER_PPM           0
#define LASER_DURATION      0
#define LASER_DEFAULT_MODE  PULSED

uint8_t LaserCount = 0;

/* laser constructor */
Laser::Laser()//int FiringPin = LASER_FIRING_PIN, int PulsePin = LASER_INTENSITY_PIN, int mode = PULSED)
{

    // TODO init timers on laser pins
    if(LaserCount < LASER_MAX) {

        this->id = LaserCount++;
        if (LASER_MAX == 1) {       // do something here if you want add a laser
            this->laser.FiringPin.nbr = LASER_FIRING_PIN;
            this->laser.FiringPin.isActive = true;

            pinMode(laser.FiringPin.nbr, OUTPUT);

            #ifdef LASER_CONTROL
                this->laser.PulsePin.nbr = LASER_INTENSITY_PIN;
                this->laser.PulsePin.isActive = true;
                pinMode(laser.PulsePin.nbr, OUTPUT);
            #else
                laser.PulsePin.isActive = false;
            #endif

            this->laser.Therm0_Pin.nbr = TEMP_0_PIN;
            this->laser.Therm1_Pin.nbr = TEMP_1_PIN;
            this->laser.Therm2_Pin.nbr = TEMP_2_PIN;
            (TEMP_SENSOR_0 > 0)? this->laser.Therm0_Pin.isActive = true : this->laser.Therm0_Pin.isActive = false;
            (TEMP_SENSOR_1 > 0)? this->laser.Therm1_Pin.isActive = true : this->laser.Therm1_Pin.isActive = false;
            (TEMP_SENSOR_2 > 0)? this->laser.Therm2_Pin.isActive = true : this->laser.Therm2_Pin.isActive = false;
            if(this->laser.Therm0_Pin.isActive) pinMode(this->laser.Therm0_Pin.nbr, INPUT);
            if(this->laser.Therm1_Pin.isActive) pinMode(this->laser.Therm1_Pin.nbr, INPUT);
            if(this->laser.Therm2_Pin.isActive) pinMode(this->laser.Therm2_Pin.nbr, INPUT);

            #ifdef LASER_FAN
                if(FAN_PIN > 0){
                    this->laser.BeamPumpPin.nbr = FAN_PIN;
                    this->laser.BeamPumpPin.isActive = true;
                }
            #endif
            #ifdef LASER_PUMP
            if(PUMP_PIN > 0){
                this->laser.WaterPumpPin.nbr = PUMP_PIN;
                this->laser.WaterPumpPin.isActive = true;
            }
            #endif

            #ifdef DEBUG_LASER
            SERIAL_ECHOLN("Laser attached");
            #endif
        }
        setMode(mode);
    } else {
        this->id = -1;
        #ifdef DEBUG_LASER
            SERIAL_ECHOLN("Too many laser");
        #endif
    }

    reset();
}

/* Firing the laser on */
void Laser::fireOn(uint16_t intensity) {

    if(intensity > 4096) intensity = 4096;
    if(intensity < 0) intensity = 0;


    if(id > 0 && enabled) {

        this->firing = true;
        this->last_firing = micros();

   	    analogWrite(LASER_FIRING_PIN, labs((intensity / 4096)*(F_CPU / LASER_PWM)));
        #ifdef LASER_CONTROL
            digitalWrite(LASER_FIRING_PIN, HIGH);
        #endif

        #ifdef DEBUG_LASER
            SERIAL_ECHOLN("laser firing !!!");
        #endif
    }

    #ifdef DEBUG_LASER
        if(!enabled) SERIAL_ECHOLN("Laser is disabled can't fire it on");
    #endif
}

/* Firing the laser off */
void Laser::fireOff() {

    digitalWrite(LASER_FIRING_PIN, LOW);
    // TODO need to set pwm laser_intensity too ?

    this->time += millis() - (this->last_firing / 1000);

    if(this->firing) {
        this->firing = false;
        #ifdef DEBUG_LASER
            SERIAL_ECHOLN("laser turned off");
        #endif
    }
}

/* Reset laser to initial (safe) values */
void Laser::reset() {

    fireOff();            // force laser off
    intensity = 0;        // set intensity to zero
    ppm = 0;              // set pulses per millimeter to zero, for pulsed firing mode
    enabled = false;

    #ifdef LASER_RASTER
        raster_aspect_ratio = LASER_RASTER_ASPECT_RATIO;
        raster_mm_per_pulse = LASER_RASTER_MM_PER_PULSE;
        raster_direction = 1;
    #endif // LASER_RASTER
    #ifdef MUVE_Z_PEEL
        peel_distance = 2.0;
        peel_speed = 2.0;
        peel_pause = 0.0;
    #endif // MUVE_Z_PEEL
}

/* change laser mode */
void Laser::setMode(laser_e mode) {
    // TODO set PULSED || CONTINUOUS || RASTER need to change behaviors ?
    Laser::mode = mode;
}

/* change laser intensity*/
void Laser::setIntensity(uint16_t intensity) {
    if(intensity > 4096)
        Laser::intensity = 4096;
    else
        Laser::intensity = intensity;
}

void Laser::setMethode(methode_e methode) {
    Laser::methode = methode;
}

void Laser::setDuration(unsigned long duration) {
    Laser::duration = duration;
}

/* change PPM values (point per millimeter 0 <=> 4095) */
void Laser::setPpm(uint16_t ppm) {
    if(ppm > 4096)
        Laser::ppm = 4095;
    else
        Laser::ppm = ppm;
}

void Laser::setLifetime(unsigned long lifetime) {
    Laser::lifetime = lifetime;
}

void Laser::setTime(unsigned int time) {
    Laser::time = time;
}

unsigned int Laser::getTime() {
    return Laser::time;
}

unsigned int Laser::getLifetime() {
    return Laser::lifetime;
}

unsigned int Laser::getPpm() {
    return (unsigned int)Laser::ppm;
}
