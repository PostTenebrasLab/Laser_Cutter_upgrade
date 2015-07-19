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
Laser::Laser(int FiringPin = LASER_FIRING_PIN, int PulsePin = LASER_INTENSITY_PIN, int mode = PULSED)
{

    // TODO init timers on laser pins
    if(LaserCount < LASER_MAX) {

        this->id = LaserCount++;
        if (LASER_MAX == 1) {
            laser.FiringPin.nbr = FiringPin;
            laser.FiringPin.isActive = 0;

            laser.PulsePin.nbr = PulsePin;
            laser.PulsePin.isActive = 0;

            pinMode(laser.FiringPin.nbr, OUTPUT);
            pinMode(laser.PulsePin.nbr, OUTPUT);

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
void Laser::fireOn(::Laser::uint16_t intensity) {

    if(intensity > 4096) intensity = 4096;
    if(intensity < 0) intensity = 0;


    if(id > 0) {

        this->firing = true;
        this->last_firing = micros();

        #if LASER_CONTROL == 1
    	    analogWrite(LASER_FIRING_PIN, labs((intensity / 4096)*(F_CPU / LASER_PWM)));
        #endif
        #if LASER_CONTROL == 2
            analogWrite(LASER_INTENSITY_PIN, labs((intensity / 4096) * (F_CPU / LASER_PWM)));
            digitalWrite(LASER_FIRING_PIN, HIGH);
        #endif

        #ifdef DEBUG_LASER
            SERIAL_ECHOLN("laser firing !!!");
        #endif
    }
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


void Laser::setMode(const ::Laser::laser_e &l_mode) {
    Laser::l_mode = mode;
}


#endif // LASER_PERIPHERALS
