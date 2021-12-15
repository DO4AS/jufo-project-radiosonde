/*
 * radiosonde firmware as part of the 
 * "Jugend Forscht" project "In 80 Tagen um die Welt- kleine Sonden auf großer Mission"
 
 * Copyright (C) 2021  Amon Schumann / DO4AS
 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <Arduino.h>

#include "ntc.h"
#include "config.h"

ntc_temperature_sensor::ntc_temperature_sensor()
{

}

ntc_temperature_sensor::~ntc_temperature_sensor() 
{  

}

// returns temperature from NTC sensor
int ntc_temperature_sensor::get_ntc_temperature(void)
{

  // take n number of analog reads and average them
  float average_analog_reading = 0;

  for(int c = 0; c < ntc_number_of_analog_readings_for_averaging; c++)
  {
    average_analog_reading = average_analog_reading + analogRead(ntc_analog_pin);
    delay(5);
  }

  average_analog_reading = average_analog_reading / ntc_number_of_analog_readings_for_averaging;

  // compute temperature using Steinhart–Hart equation
  return(round(1 / (log(1 / (1023 / average_analog_reading - 1)) / ntc_beta_coefficient + 1 / (ntc_nominal_temperature + 273.15)) - 273.15));
}
