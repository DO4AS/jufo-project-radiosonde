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

#include "config.h"

#ifdef used_aprs
  #include "aprs.h"
#elif defined used_lora_ttn
  #include "lora.h"
#endif

#include "gps.h"
#include "geofence.h"
#include "voltage.h"

#ifdef used_ds18b20
  #include "ds18b20.h"
#elif used_bme680
  #include "bme680.h"
#endif

#ifdef used_aprs
  aprs aprs_module;
#elif defined used_lora_ttn
  // references for TinyLora library
  // network Session Key (MSB)
  uint8_t NwkSkey[16] = lora_network_session_key;
  // application Session Key (MSB)
  uint8_t AppSkey[16] = lora_app_session_key;
  // device Address (MSB)
  uint8_t DevAddr[4] = device_address;
  
  lora lora_module;
#endif

gps gps_module;
geofence geofence_module;
voltage_measurement voltage_measurement_module;

#ifdef used_ds18b20
  ds18b20 ds18b20_module;
#elif defined used_bme680
  bme680 bme680_module;
#endif

float gps_latitude;
float gps_longitude;
long int gps_altitude;

int gps_satellites;
int gps_speed;
int gps_course;

String gps_latitude_DMH;
String gps_longitude_DMH;

int mcu_input_voltage;
int solar_input_voltage;

#ifdef used_ds18b20
  int ds18b20_temperature;
#elif defined used_bme680
  long int bme680_temperature;
  long int bme680_humidity;
  long int bme680_pressure;
  long int bme680_gas_resistance;
#endif

float ref_millis = float(radio_packet_delay) * -1000;

int radio_packet_counter = 0;

// forward function declarations
#ifdef used_aprs
  // begin sx1278 connection for afsk packet radio
  void send_aprs_packet();
#elif defined used_lora_ttn
  // begin sx1278 connection for lora
  void send_lora_packet();
#endif

void setup()
{
  // turn serial debug on or off
  #ifdef enable_serial_debug
    Serial.begin(serial_baud_rate);
    #define DEBUG_PRINT(x)  Serial.print (x)
    #define DEBUG_PRINTLN(x)  Serial.println (x)
  #else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
  #endif

  #ifdef used_bme680
    bme680_module.begin();
  #endif

  #ifdef used_aprs
    // begin sx1278 connection for afsk packet radio
    aprs_module.begin();
  #elif defined used_lora_ttn
    // begin sx1278 connection for lora
    lora_module.begin();
  #endif

  // set serial baud of gps module
  gps_module.m_pgps_serial->begin(gps_baud_rate);
}

void loop()
{
  while (gps_module.m_pgps_serial->available())
  {
    // read gnss position data
    if (gps_module.m_pgps_device->encode(gps_module.m_pgps_serial->read())) 
    {
      // valid gnss position-fix
      if (gps_module.m_pgps_device->location.isValid())
      {
        DEBUG_PRINTLN(millis() - ref_millis);
        
        // no packet data rate limitation - send radio packet
        if((millis() - ref_millis) > float(radio_packet_delay) * 1000) 
        {
          #ifdef used_aprs
            send_aprs_packet();
          #elif defined used_lora_ttn
            send_lora_packet();
          #endif

          ref_millis = millis();
        }     
      }
      // no packet data rate limitation
      else 
      {
        DEBUG_PRINTLN("invalid gps fix");
        
        // send radio packet after gps timeout
        if(millis() >= float(gps_timeout) * 1000)
        {
          DEBUG_PRINTLN(millis() - ref_millis);
        
          if((millis() - ref_millis) > float(radio_packet_delay) * 1000)
          {
            #ifdef used_aprs
              send_aprs_packet();
            #elif defined used_lora_ttn
              send_lora_packet();
            #endif

            ref_millis = millis();
          }  
        }
      }
    }
  }
}

#ifdef used_aprs
  void send_aprs_packet()
  {
    // get gps location,altitude and satellite data
    gps_latitude = gps_module.m_pgps_device->location.lat();
    gps_longitude = gps_module.m_pgps_device->location.lng();
    gps_altitude = long (gps_module.m_pgps_device->altitude.meters());
    gps_satellites = gps_module.m_pgps_device->satellites.value();
    gps_speed = int (gps_module.m_pgps_device->speed.kmph());
    gps_course = int (gps_module.m_pgps_device->course.deg());  
    
    gps_module.m_pgps_serial->end();   // needed for aprs to properly work

    // get aprs frequency using geofence
    float freq = geofence_module.get_aprs_frequency(gps_latitude, gps_longitude);
    
    // convert gps coordinates to DMH format needed for aprs
    gps_module.convert_gps_coordinates_to_DMH(gps_latitude, gps_longitude, &gps_latitude_DMH, &gps_longitude_DMH);
    
    // print out gps data
    DEBUG_PRINTLN(gps_latitude);
    DEBUG_PRINTLN(gps_longitude);
    DEBUG_PRINTLN(gps_altitude);
    DEBUG_PRINTLN(gps_satellites);

    // get mcu and solar input voltage measurement
    voltage_measurement_module.get_voltage_measurements(&mcu_input_voltage, &solar_input_voltage);

    #ifdef used_ds18b20
      // get temperature value from DS18B20 sensor
      ds18b20_temperature = ds18b20_module.get_ds18b20_temperature();
    #elif defined used_bme680
      // get environmental data from BME680 sensor
      bme680_module.get_environmental_readings(&bme680_temperature, &bme680_humidity, &bme680_pressure, &bme680_gas_resistance);
    #endif


    #ifdef used_ds18b20
      // create aprs comment with payload data
      String aprs_comment = aprs_module.create_comment(gps_altitude, radio_packet_counter, ds18b20_temperature, mcu_input_voltage, solar_input_voltage, gps_speed, gps_course, gps_satellites, additional_aprs_comment);
    #elif defined used_bme680
      // create aprs comment with payload data
      String aprs_comment = aprs_module.create_comment(gps_altitude, radio_packet_counter, bme680_temperature, bme680_humidity, bme680_pressure, bme680_gas_resistance, mcu_input_voltage, solar_input_voltage, gps_speed, gps_course, gps_satellites, additional_aprs_comment);
    #else
      // create aprs comment with payload data
      String aprs_comment = aprs_module.create_comment(gps_altitude, radio_packet_counter, mcu_input_voltage, solar_input_voltage, gps_speed, gps_course, gps_satellites, additional_aprs_comment); 
    #endif
    
    aprs_module.send(gps_latitude_DMH, gps_longitude_DMH, aprs_comment, freq);

    // increase aprs packet counter by one
    radio_packet_counter = radio_packet_counter + 1;
    
    gps_module.m_pgps_serial->begin(gps_baud_rate);   // needed for aprs to properly work
  }

  #elif defined used_lora_ttn
    void send_lora_packet()
    {
      // get gps location,altitude and satellite data
      gps_latitude = gps_module.m_pgps_device->location.lat();
      gps_longitude = gps_module.m_pgps_device->location.lng();
      gps_altitude = gps_module.m_pgps_device->altitude.meters();
      gps_satellites = gps_module.m_pgps_device->satellites.value();
      gps_speed = int (gps_module.m_pgps_device->speed.kmph());
      gps_course = int (gps_module.m_pgps_device->course.deg());  
      
      gps_module.m_pgps_serial->end();   // needed for lora to properly work
      
      // print out gps data
      DEBUG_PRINTLN(gps_latitude);
      DEBUG_PRINTLN(gps_longitude);
      DEBUG_PRINTLN(gps_altitude);
      DEBUG_PRINTLN(gps_satellites);

      // get mcu and solar input voltage measurement
      voltage_measurement_module.get_voltage_measurements(&mcu_input_voltage, &solar_input_voltage);

      #ifdef used_ds18b20
      // get temperature value from DS18B20 sensor
        ds18b20_temperature = ds18b20_module.get_ds18b20_temperature();
      #elif defined used_bme680
      // get environmental data from BME680 sensor
        DEBUG_PRINTLN("BME680 cannot be used with lora");
      #endif

      // encode flight number to base91 format
      char base91_encoded_flight_number[2];
      gps_module.base91_encode_gps_coordinates(base91_encoded_flight_number, payload_flight_number, 1);

      // encode GNSS latitude to base91 format
      char base91_encoded_gps_latitude[5];
      gps_module.base91_encode_gps_coordinates(base91_encoded_gps_latitude, long (gps_latitude*100000+9000000), 4);

      // encode GNSS longitude to base91 format
      char base91_encoded_gps_longitude[5];
      gps_module.base91_encode_gps_coordinates(base91_encoded_gps_longitude, long (gps_longitude*100000+18000000), 4);

      // encode GNSS altitude to base91 format
      char base91_encoded_gps_altitude[4];
      gps_module.base91_encode_gps_coordinates(base91_encoded_gps_altitude, (long) gps_altitude+20000, 3);

      // encode GNSS speed and course to base91 format
      char base91_encoded_input_speed_course[4];
      gps_module.base91_encode_gps_coordinates(base91_encoded_input_speed_course, gps_module.combine_two_numbers(gps_speed+100, gps_course+100, 3), 3);

      // encode radio packet counter to base91 format
      char base91_encoded_radio_packet_counter[3];
      gps_module.base91_encode_gps_coordinates(base91_encoded_radio_packet_counter, radio_packet_counter, 2);

      // encode mcu input voltage and solar input voltage to base91 format
      char base91_encoded_input_voltages[4];
      gps_module.base91_encode_gps_coordinates(base91_encoded_input_voltages, gps_module.combine_two_numbers(mcu_input_voltage+100, solar_input_voltage+100, 3), 3);

      // encode GNSS satellite count to base91 format
      char base91_encoded_gps_satellites[2];
      gps_module.base91_encode_gps_coordinates(base91_encoded_gps_satellites, gps_satellites, 1);
      
      #ifdef used_ds18b20
        // encode ds18b20 temperature
        char base91_encoded_ds18b20_temperature[3];
        gps_module.base91_encode_gps_coordinates(base91_encoded_ds18b20_temperature, ds18b20_temperature+1000, 2);
        
        // create aprs comment with payload data
        String payload_data = lora_module.create_payload_data(base91_encoded_flight_number, base91_encoded_gps_latitude, base91_encoded_gps_longitude, base91_encoded_gps_altitude, base91_encoded_input_speed_course, base91_encoded_radio_packet_counter, base91_encoded_ds18b20_temperature, base91_encoded_input_voltages, base91_encoded_gps_satellites);
      #else
        // create aprs comment with payload data
        String payload_data = lora_module.create_payload_data(base91_encoded_flight_number, base91_encoded_gps_latitude, base91_encoded_gps_longitude, base91_encoded_gps_altitude, base91_encoded_input_speed_course, base91_encoded_radio_packet_counter, base91_encoded_input_voltages, base91_encoded_gps_satellites);
      #endif

      DEBUG_PRINTLN(payload_data);
      lora_module.send_payload_data(payload_data);

      // increase aprs packet counter by one
      radio_packet_counter = radio_packet_counter + 1;
    
      gps_module.m_pgps_serial->begin(gps_baud_rate);   // needed for lora to properly work
    }
#endif
