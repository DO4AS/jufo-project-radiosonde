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

#ifndef LORA_H_
#define LORA_H_

#include <TinyLoRa.h>

#include "config.h"

// SX1278 hardware pin definitions 
#define NSS 5                                 
#define NRESET 7                                                               
#define DIO0 4 

class lora
{
  public:

    lora();
    ~lora();

    void begin(void);

    #ifdef used_ds18b20
      String create_payload_data(char *flight_number, char *gps_latitude, char *gps_longitude, char *gps_altitude, char *gps_speed_course, char *aprs_packet_number, char *temperature, char *input_voltages, char *number_of_gps_satellites);
    #else
      String create_payload_data(char *flight_number, char *gps_latitude, char *gps_longitude, char *gps_altitude, char *gps_speed_course, char *aprs_packet_number, char *input_voltages, char *number_of_gps_satellites);
    #endif

    void send_payload_data(String payload); 

  private:

    TinyLoRa  *lora_device;

    // turn serial debug on or off
    #ifdef enable_serial_debug
      #define DEBUG_PRINT(x)  Serial.print (x)
      #define DEBUG_PRINTLN(x)  Serial.println (x)
    #else
      #define DEBUG_PRINT(x)
      #define DEBUG_PRINTLN(x)
    #endif

};

#endif /* LORA_H_ */
