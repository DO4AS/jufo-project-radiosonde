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

#include <TinyGPS++.h>
#include <AltSoftSerial.h>

#include "gps.h"

#include "config.h"

gps::gps()
: m_pgps_device(NULL),
  m_pgps_serial(NULL)
{  
  if (!m_pgps_device)
  {
    m_pgps_device = new TinyGPSPlus();
  }

  if (!m_pgps_serial)
  {
    m_pgps_serial = new AltSoftSerial();
  }
}

gps::~gps() 
{  
  if (m_pgps_device) 
  {  
    delete m_pgps_device;
    m_pgps_device = NULL;
  }
  
  if (m_pgps_serial)
  {
    m_pgps_serial = NULL;
  }
}

/*
 * GNSS coordinates format conversion in degrees, minutes and hundredths of a minute (needed for aprs)
 * N = north; S = south; E = east; W = west
 * more information at: http://www.aprs.org/doc/APRS101.PDF
 */

#if radio_protocol == 1
  void gps::convert_gps_coordinates_to_DMH(float gps_latitude, float gps_longitude, String* gps_latitude_DMH, String* gps_longitude_DMH)
  {
    int degrees_buffer;
    int minutes_buffer;
    int h_minutes_buffer;
    
    char latitude_compass_direction;
    
    // determine whether to append "N" or "S"
    if ((int) gps_latitude > 0) 
    {
      latitude_compass_direction = 'N'; 
    }
    else
    {
      gps_latitude = -(gps_latitude);
      latitude_compass_direction = 'S';
    }
    
    // determine full degrees
    degrees_buffer = (int) (gps_latitude);

    // determine degree minutes from the remainder
    gps_latitude = gps_latitude - (degrees_buffer);
    gps_latitude = (gps_latitude) * 60;
    minutes_buffer = (int) (gps_latitude);

    // determine hundredths of degree minutes from the remainder
    h_minutes_buffer = (int) ((gps_latitude - minutes_buffer) * 100);
    
    // generate postion data string
    char latitude_coordinates_buffer[12];
    sprintf(latitude_coordinates_buffer, "%02d%02d.%02d%c" , degrees_buffer, minutes_buffer, h_minutes_buffer, latitude_compass_direction);
    *gps_latitude_DMH = String(latitude_coordinates_buffer);

    char longitude_compass_direction;
  
    // determine whether to append "E" or "W"
    if ((int) gps_longitude > 0) 
    {
      longitude_compass_direction = 'E'; 
    }
    else
    {
      gps_longitude = -(gps_longitude);
      longitude_compass_direction = 'W';
    }
    
    // determine full degrees
    degrees_buffer = (int) (gps_longitude);

    // determine degree minutes from the remainder
    gps_longitude = gps_longitude - (degrees_buffer);
    gps_longitude = (gps_longitude) * 60;
    minutes_buffer = (int) (gps_longitude);

    // determine hundredths of degree minutes from the remainder
    h_minutes_buffer = (int) ((gps_longitude - minutes_buffer) * 100);

    // generate postion data string
    char longitude_coordinates_buffer[13];
    sprintf(longitude_coordinates_buffer, "%03d%02d.%02d%c" , degrees_buffer, minutes_buffer, h_minutes_buffer, longitude_compass_direction);
    *gps_longitude_DMH = String(longitude_coordinates_buffer);
  }

#elif radio_protocol == 2

  /*
  * base-91 encoding
  */
  char *gps::base91_encode_gps_coordinates(char *base91_encoded_buffer, long number_to_base91_encode, int base91_length)
  {
      // alternative to "pow(base, exponent)" function wihtout operation on floating point numbers
      for (int c = 0; c < base91_length; c++)
      {
        long power_of_91 = 1;
        for (int cp = 0; cp < (base91_length - 1) - c; cp++)
        {
          power_of_91 = power_of_91 * 91;
        }
        base91_encoded_buffer[c] =  (char) (number_to_base91_encode / power_of_91 + 33);
        number_to_base91_encode = number_to_base91_encode % power_of_91;
      }

      // append end flag to char array
      base91_encoded_buffer[base91_length] = '\0';

      return &(base91_encoded_buffer[0]);
  }

  // used to combine two numbers for a more efficient conversion to base91
  long gps::combine_two_numbers(int number_1, int number_2, int number_2_max_length)
  {
    int power_of_10 = 1;
    for (int c = 0; c < number_2_max_length; c++)
    {
      power_of_10 *= 10;
    }

    return (long) number_1*power_of_10 + number_2;
  }
#endif
