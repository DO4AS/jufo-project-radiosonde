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
#include <TinyLoRa.h>

#include "lora.h"

lora::lora()
: lora_device(NULL)
{ 
  if (!lora_device)
  {
    lora_device = new TinyLoRa(DIO0, NSS, NRESET);
  }
}

lora::~lora() 
{  
  if (lora_device)
  {
    delete lora_device;
    lora_device = NULL;
  }
}

void lora::begin(void) 
{
  // lora channel for data transmission
  lora_device->setChannel(lora_channel);

  // lora datarate
  lora_device->setDatarate(lora_datarate);

  // begin lora radio
  while(!lora_device->begin())
  {
    DEBUG_PRINTLN("no lora radio (sx1278) connected");
  }

  // lora power
  lora_device->setPower(sx1278_tx_power);
}


#if environmental_sensor == 1 or environmental_sensor == 2
  String lora::create_payload_data(char *flight_number, char *gps_latitude, char *gps_longitude, char *gps_altitude, char *gps_speed_course, char *aprs_packet_number, char *temperature, char *input_voltages, char *number_of_gps_satellites)
  {
      char payload_data_buffer[80];
      sprintf(payload_data_buffer, "%s%s%s%s%s%s%s%s%s", flight_number, gps_latitude, gps_longitude, gps_altitude, gps_speed_course, aprs_packet_number, temperature, input_voltages, number_of_gps_satellites);

      return String(payload_data_buffer);
  }
#else
  String lora::create_payload_data(char *flight_number, char *gps_latitude, char *gps_longitude, char *gps_altitude, char *gps_speed_course, char *aprs_packet_number, char *input_voltages, char *number_of_gps_satellites)
  {
    
      char payload_data_buffer[80];
      sprintf(payload_data_buffer, "%s%s%s%s%s%s%s%s", flight_number, gps_latitude, gps_longitude, gps_altitude, gps_speed_course, aprs_packet_number, input_voltages, number_of_gps_satellites);

      return String(payload_data_buffer);
  }
#endif

void lora::send_payload_data(String payload) 
{
  lora_device->sendData((unsigned char*) payload.c_str(), payload.length(), lora_device->frameCounter);

  lora_device->frameCounter++;
}