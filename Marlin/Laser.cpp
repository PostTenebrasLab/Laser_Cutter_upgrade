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
#include "temperature.h"

#define LASER_MAX           1       // in case of several laser
#define LASER_INTENSITY     0
#define LASER_PPM           0
#define LASER_DURATION      0
#define LASER_DEFAULT_MODE  PULSED
#define MAX_PPM             10000  // max point per mm
#define DAC_RESOLUTION

//#define WRITE_PUMP(pin,value) WRITE(pin,value)

uint8_t LaserCount = 0;

volatile unsigned long lastTime;
volatile unsigned long frequence;
extern void updateTemperaturesFromRawValues();
extern float current_temperature[EXTRUDERS];
extern float current_temperature_bed;

void measureFlow(){

    /* weighted average of last measure and new one */
    frequence = 1000 * (frequence + 1) / (1000 + millis() - lastTime);
    lastTime = millis();

}


/* laser constructor */
Laser::Laser(int FiringPin, int PulsePin, laser_e mode)
{

    // TODO init timers on laser pins
    if(LaserCount < LASER_MAX) {

        id = LaserCount++;
        if (LASER_MAX == 1) {       // do something here if you want add a laser

#if LASER_FIRING_PIN > 0
            laser.FiringPin.nbr = LASER_FIRING_PIN;
            laser.FiringPin.isActive = true;
            pinMode(laser.FiringPin.nbr, OUTPUT);
#else
            return;
#endif

#if LASER_INTENSITY_PIN > 0
            laser.IntensityPin.nbr = LASER_INTENSITY_PIN;
            laser.IntensityPin.isActive = true;
            pinMode(laser.IntensityPin.nbr, OUTPUT);
#define INTENSITY_ENABLED
#endif

#if TEMP_0_PIN > 0
            laser.Therm0_Pin.nbr = TEMP_0_PIN;
            laser.Therm0_Pin.isActive = true;
            pinMode(laser.Therm0_Pin.nbr, INPUT);
#endif
#if TEMP_BED_PIN > 0
            laser.Therm1_Pin.nbr = TEMP_BED_PIN;
            laser.Therm1_Pin.isActive = true;
            pinMode(laser.Therm1_Pin.nbr, INPUT);
#endif

#if defined(LASER_FAN) && FAN_PIN > 0
            laser.AirPumpPin.nbr = FAN_PIN;
            laser.AirPumpPin.isActive = true;
            pinMode(laser.AirPumpPin.nbr, OUTPUT);
            airPumpOff();
#endif

#if defined(LASER_PUMP) && LASER_PUMP_PIN > 0
            laser.WaterPumpPin.nbr = LASER_PUMP_PIN;
            laser.WaterPumpPin.isActive = true;
            pinMode(laser.WaterPumpPin.nbr, OUTPUT);
#endif

#if defined(FLOW_METER_PIN) && FLOW_METER_PIN > 0
            attachInterrupt(FLOW_METER_PIN, measureFlow, RISING);
#endif


            /* Activate timer for pulse and intensity */
            HAL_timer_isr_prologue (LASER_TIMER_NUM);
            HAL_timer_set_count (STEP_TIMER_NUM, HAL_TIMER_RATE / 1000); // 1kHz
            HAL_timer_start (STEP_TIMER_NUM, 1000);
            ENABLE_LASER_INTERRUPT();
            sei();
/*#ifdef DEBUG_LASER
    SERIAL_ECHOLN("Laser attached");
#endif*/

        }
        setMode(mode);

    } else {
        this->id = -1;
/*        #ifdef DEBUG_LASER
            SERIAL_ECHOLN("Too many laser");
        #endif*/
    }

    reset();
}

/* Firing the laser on */
void Laser::fireOn(float intensity) {

    setIntensity(intensity);
    fireOn();
}

/* Firing the laser on */
void Laser::fireOn() {

    if(id > 0 && armed) {

        firing = true;
        switch(mode) {
            case CONTINUOUS:
                // TODO check if laser's moving
                digitalWrite(laser.FiringPin.nbr, HIGH);
                break;
            case PULSED:
                // TODO generate PWN on FiringPin
                break;
            case RASTER:
                // TODO do something
                break;
        }

/*        #ifdef DEBUG_LASER
            SERIAL_ECHOLN("laser firing !!!");
        #endif*/
    }

/*    #ifdef DEBUG_LASER
        if(!armed) SERIAL_ECHOLN("Laser is disarmed can't fire it on");
    #endif*/
}

/* Firing the laser off */
void Laser::fireOff() {

    digitalWrite(LASER_FIRING_PIN, LOW);

    if(firing) {
        firing = false;
/*        #ifdef DEBUG_LASER
            SERIAL_ECHOLN("laser turned off");
        #endif*/
    }
}

/* Switch the air pump On */
void Laser::airPumpOn(){

#if defined(LASER_FAN) && FAN_PIN > 0
    digitalWrite(FAN_PIN,HIGH);
#endif
}

/* Switch the air pump Off */
void Laser::airPumpOff(){

#if defined(LASER_FAN) && FAN_PIN > 0
    digitalWrite(FAN_PIN, LOW);
#endif
}

void Laser::checkTemperatures(){

    updateTemperaturesFromRawValues();

    if( abs(current_temperature_bed-current_temperature[0]) > 10) {
        armed = false;
//        SERIAL_ECHOLN("laser too hot, laser disabled");
    }


}

/* Reset laser to initial (safe) values */
void Laser::reset() {

    fireOff();            // force laser off
    intensity = 0.0;      // set intensity to zero
    ppm = 0;              // set pulses per millimeter to zero, for pulsed firing mode
    armed = false;

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

    /* Activate timer for pulse mode */
    if(Laser::mode != PULSED && mode == PULSED) {
//        HAL_timer_isr_prologue (LASER_TIMER_NUM);
//        HAL_timer_set_count (STEP_TIMER_NUM, HAL_TIMER_RATE / 1000); // 1kHz
//        HAL_timer_start (STEP_TIMER_NUM, 1000);
//        ENABLE_LASER_INTERRUPT();
//        sei();
    }

    Laser::mode = mode;
}

/* change laser intensity*/
void Laser::setIntensity(float intensity) {

    /* normalise value */
    (intensity > 1.0)? Laser::intensity = 1.0 : Laser::intensity = intensity;
    (intensity < 0.0)? Laser::intensity = 0.0 : Laser::intensity = intensity;

//  TODO generate signal on DAC output
//    analogWrite(laser.IntensityPin.nbr, labs((getIntensity() / DAC_RESOLUTION) * (F_CPU / LASER_PWM)));

}

void Laser::setDuration(float duration) {

    /* normalise value */
    (duration > 1.0)? Laser::duration = 1.0 : Laser::duration = duration;
    (duration < 0.0)? Laser::duration = 0.0 : Laser::duration = duration;

}

void Laser::setLifetime(unsigned long lifetime) {
    Laser::lifetime = lifetime;
}

unsigned int Laser::getLifetime() {
    return Laser::lifetime;
}

