/*
  laser.h - Laser cutter control library for Arduino using 16 bit timers- Version 1
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

#ifndef LASER_H
#define LASER_H

#include <inttypes.h>
#include "Configuration.h"

extern volatile unsigned long frequence;

typedef enum {CONTINUOUS = 0, PULSED = 1, RASTER = 2} laser_e;

typedef struct {
  uint8_t nbr        :6 ;             // a pin number from 0 to 63
  bool isActive      :1 ;             // true if this channel is enabled, pin not pulsed if false
} LaserPin_t;

typedef struct {
  LaserPin_t FiringPin;     // Firing pin (control mode 1 and 2)
  LaserPin_t IntensityPin;      // Pulse pin (control mode 2 only)
  LaserPin_t Therm0_Pin;    // first thermistor pin
  LaserPin_t Therm1_Pin;    // second thermistor pin
  LaserPin_t Therm2_Pin;    // second thermistor pin
  LaserPin_t Therm3_Pin;    // second thermistor pin
  LaserPin_t WaterPumpPin;  // water cooling system pump's pin
  LaserPin_t AirPumpPin;   // beam air flow pump's pin (blow smoke)
} Laser_t;

class Laser
{
public:

//  Laser();
  Laser(int FiringPin = LASER_FIRING_PIN, int PulsePin = LASER_INTENSITY_PIN, laser_e mode = PULSED);
  void reset();            // reset values (OFF)
  void stop();
  void shutdown();
  void kill();

  void fireOn();
  void fireOn(float intensity);
  void fireOff();

  void checkTemperatures();
  void airPumpOn();
  void airPumpOff();


  void setMode(laser_e mode);
  void setIntensity(float intensity);  // value between 0.0 and 1.0
  void setPpm(unsigned long ppm);
  void setDuration(unsigned long duration);
  void setLifetime(unsigned long lifetime);
  void setTime(unsigned int time);
  unsigned int getTime();
  unsigned int getLifetime();
  unsigned int getPpm();

private:

  Laser_t laser;
  uint8_t id;
  float intensity;        // Laser firing instensity (value between 0 and 1)
  unsigned long ppm;      // pulses per millimeter, for pulsed firing mode
  bool armed;             // is the laser armed
//  bool status;            // LASER_ON / LASER_OFF - buffered
  bool firing;            // LASER_ON / LASER_OFF - instantaneous
  laser_e mode;           // CONTINUOUS, PULSED, RASTER
  unsigned long duration; // laser firing duration in microseconds, for pulsed firing mode
  unsigned long dur;      // instantaneous duration
  unsigned long last_firing; // microseconds since last laser firing
  unsigned int time;      // temporary counter to limit eeprom writes (millisec)
  unsigned int lifetime;  // laser lifetime firing counter in minutes
  #ifdef LASER_RASTER
    unsigned char raster_data[LASER_MAX_RASTER_LINE];
    unsigned char rasterlaserpower;

    float raster_aspect_ratio;
    float raster_mm_per_pulse;
    int raster_raw_length;
    int raster_num_pixels;
    bool raster_direction;
  #endif // LASER_RASTER
  #ifdef MUVE_Z_PEEL
    float peel_distance;
    float peel_speed;
    float peel_pause;
  #endif // MUVE_Z_PEEL

};


#endif // LASER_H


