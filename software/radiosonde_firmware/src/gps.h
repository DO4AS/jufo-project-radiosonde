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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GPS_H_
#define GPS_H_

#include <TinyGPS++.h>
#include <AltSoftSerial.h>

#include "config.h"

class gps
{
  public:

    gps();
    ~gps();
  
    TinyGPSPlus *m_pgps_device;
    AltSoftSerial *m_pgps_serial;
    
    #ifdef used_aprs
      void convert_gps_coordinates_to_DMH(float gps_latitude, float gps_longitude, String* gps_latitude_DMH, String* gps_longitude_DMH);

    #elif defined used_lora_ttn
      char *base91_encode_gps_coordinates(char *base91_encoded_buffer, long number_to_base91_encode, int base91_length);
      long combine_two_numbers(int number_1, int number_2, int number_2_max_length);
    #endif
};

#endif /* GPS_H_ */
