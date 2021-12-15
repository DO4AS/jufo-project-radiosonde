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

#include "config.h"

#ifdef enable_watchdog
  #include <avr/wdt.h>
#endif

#if radio_protocol == 1
  #include "aprs.h"
#elif radio_protocol == 2
  #include "lora.h"
#endif

#include "gps.h"
#include "geofence.h"
#include "voltage.h"

#if environmental_sensor == 1
  #include "ds18b20.h"
#elif environmental_sensor == 2
  #include "ntc.h"
#elif environmental_sensor == 3
  #include "bme680.h"
#endif

#ifdef enable_position_caching
  #include "position_cache.h"
#endif

#if radio_protocol == 1
  aprs aprs_module;
#elif radio_protocol == 2
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

#if environmental_sensor == 1
  ds18b20 ds18b20_module;
#elif environmental_sensor == 2
  ntc_temperature_sensor ntc_temperature_sensor_module;
#elif environmental_sensor == 3
  bme680 bme680_module;
#endif

#ifdef enable_position_caching
  position_cache position_cache_module;
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

#if environmental_sensor == 1
  int ds18b20_temperature;
#elif environmental_sensor == 2
  int ntc_temperature;
#elif environmental_sensor == 3
  long int bme680_temperature;
  long int bme680_humidity;
  long int bme680_pressure;
  long int bme680_gas_resistance;
#endif

bool position_caching_handler_already_executed_in_loop = false;

float ref_millis = float(radio_packet_delay) * -1000;

int radio_packet_counter = 0;

// forward function declarations
#if radio_protocol == 1
  void send_aprs_packet();
#elif radio_protocol == 2
  void send_lora_packet();
#endif
#ifdef enable_position_caching
  void position_caching_handler();
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

  #if radio_protocol == 1
    // begin sx1278 connection for afsk packet radio
    aprs_module.begin();
  #elif radio_protocol == 2
    // begin sx1278 connection for lora
    lora_module.begin();
  #endif

  #ifdef enable_watchdog
    // initialize watchdog
    wdt_enable(WDTO_8S);
    DEBUG_PRINTLN("initialized watchdog");
  #endif

  // set serial baud of gps module
  gps_module.m_pgps_serial->begin(gps_baud_rate);
}

void loop()
{
  while (gps_module.m_pgps_serial->available())
  {
    #ifdef enable_watchdog
      // reset watchdog time
      wdt_reset();
    #endif

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
          #if radio_protocol == 1
            send_aprs_packet();
          #elif radio_protocol == 2
            send_lora_packet();
          #endif

          // reset position caching function execution flag
          position_caching_handler_already_executed_in_loop = false;
          ref_millis = millis();
        }

        #ifdef enable_position_caching
          // between to regular data packets - cache position data
          else if((millis() - ref_millis) > float(radio_packet_delay) * 500 and !position_caching_handler_already_executed_in_loop)
          {
            position_caching_handler();
            position_caching_handler_already_executed_in_loop = true;
          }
        #endif
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
            #if radio_protocol == 1
              send_aprs_packet();
            #elif radio_protocol == 2
              send_lora_packet();
            #endif

            ref_millis = millis();
          }
        }
      }
    }
  }
}

#ifdef enable_position_caching

  void position_caching_handler()
  {
    DEBUG_PRINTLN("in position caching handler");
        
    // get aprs frequency using geofence
    bool valid_flag;
    float current_aprs_freq = geofence_module.get_aprs_frequency(gps_latitude, gps_longitude, &valid_flag);

    // read packet counter from EEPROM
    byte eeprom_stored_packet_counter = position_cache_module.get_eeprom_stored_packet_counter();

    if(valid_flag)
    {
      DEBUG_PRINTLN("currently in valid area");
      byte eeprom_already_send_packet_counter = position_cache_module.get_eeprom_send_packet_counter();

      // check if packets pending for transmission
      if(eeprom_stored_packet_counter != eeprom_already_send_packet_counter)
      {
        byte number_of_cached_packets_to_transmit;
        if(eeprom_stored_packet_counter - eeprom_already_send_packet_counter >= position_caching_max_packets_transmitted_at_once)
        {
          number_of_cached_packets_to_transmit = position_caching_max_packets_transmitted_at_once;
        }
        else if(eeprom_stored_packet_counter - eeprom_already_send_packet_counter <= 0)
        {
          if(170 - eeprom_already_send_packet_counter >= position_caching_max_packets_transmitted_at_once)
          {
            number_of_cached_packets_to_transmit = position_caching_max_packets_transmitted_at_once;
          }

          else
          {
            number_of_cached_packets_to_transmit = 170 - eeprom_already_send_packet_counter;
          }
        }
        else
        {
          number_of_cached_packets_to_transmit = eeprom_stored_packet_counter - eeprom_already_send_packet_counter;
        }

        char packet_transmission_buffer[12*number_of_cached_packets_to_transmit+1];
        packet_transmission_buffer[0] = '\0';

        for (int c = 0; c < number_of_cached_packets_to_transmit; c++)
        {
          char packet_buffer[15];
          position_cache_module.read_packet_from_eeprom_raw(eeprom_already_send_packet_counter+c+1, packet_buffer);
          strcat(packet_transmission_buffer, packet_buffer);
        }

        packet_transmission_buffer[12*number_of_cached_packets_to_transmit] = '\0';

        gps_module.m_pgps_serial->end();   // needed for aprs to properly work
        aprs_module.send_custom_packet(packet_transmission_buffer, current_aprs_freq);
        gps_module.m_pgps_serial->begin(gps_baud_rate);   // needed for aprs to properly work

        // increment already send packet counter
        if (eeprom_already_send_packet_counter < int(position_cache_module.get_eeprom_length()/6)-1)
        {
          eeprom_already_send_packet_counter = eeprom_already_send_packet_counter + number_of_cached_packets_to_transmit;
        }
        else
        {
          eeprom_already_send_packet_counter = 0;
        }

        // write updated packet send counter to EEPROM
        position_cache_module.update_eeprom_send_packet_counter(eeprom_already_send_packet_counter);
      }
    }
    else
    {
      DEBUG_PRINTLN("currently in invalid area");

      long int last_stored_timestamp = position_cache_module.read_timestamp_from_eeprom(eeprom_stored_packet_counter);
      long int current_timestamp = gps_module.m_pgps_device->date.month()*1000000 + (gps_module.m_pgps_device->date.day()*1000000)/100 + (gps_module.m_pgps_device->date.year()-2000)*100 + gps_module.m_pgps_device->time.hour();

      if(current_timestamp - last_stored_timestamp >= position_caching_delay or last_stored_timestamp - current_timestamp >= position_caching_delay)
      {  
        DEBUG_PRINTLN("about to write new position to eeprom");

        // increment packet counter
        if (eeprom_stored_packet_counter < int(position_cache_module.get_eeprom_length()/6)-1)
        {
          eeprom_stored_packet_counter++;
        }
        else
        {
          eeprom_stored_packet_counter = 0;
        }

        // write updated packet counter to EEPROM
        position_cache_module.update_eeprom_stored_packet_counter(eeprom_stored_packet_counter);

        // get gps location
        gps_latitude = gps_module.m_pgps_device->location.lat();
        gps_longitude = gps_module.m_pgps_device->location.lng();

        // generate maidenhead locator from gps location
        char field_longitude = char (gps_longitude / 20 + 74);
        char field_latitude = char(gps_latitude / 10 + 74);
        int square =  (int) (gps_longitude - (int(field_longitude) - 74) * 20) / 2 * 10 + (int) (gps_latitude - (int(field_latitude) - 74) * 10) / 1;

        // write new position and timestamp to EEPROM
        position_cache_module.write_packet_to_eeprom(eeprom_stored_packet_counter, field_latitude, field_longitude, square, current_timestamp);
      }
    }
  }
#endif


#if radio_protocol == 1
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
    bool valid_flag;
    float freq = geofence_module.get_aprs_frequency(gps_latitude, gps_longitude, &valid_flag);
 
    // convert gps coordinates to DMH format needed for aprs
    gps_module.convert_gps_coordinates_to_DMH(gps_latitude, gps_longitude, &gps_latitude_DMH, &gps_longitude_DMH);
    
    // print out gps data
    DEBUG_PRINTLN(gps_latitude);
    DEBUG_PRINTLN(gps_longitude);
    DEBUG_PRINTLN(gps_altitude);
    DEBUG_PRINTLN(gps_satellites);

    // get mcu and solar input voltage measurement
    voltage_measurement_module.get_voltage_measurements(&mcu_input_voltage, &solar_input_voltage);

    #if environmental_sensor == 1
      // get temperature value from DS18B20 sensor
      ds18b20_temperature = ds18b20_module.get_ds18b20_temperature();
    #elif environmental_sensor == 2
      // get temperature value from DS18B20 sensor
      ntc_temperature = ntc_temperature_sensor_module.get_ntc_temperature();
    #elif environmental_sensor == 3
      // get environmental data from BME680 sensor
      bme680_module.get_environmental_readings(&bme680_temperature, &bme680_humidity, &bme680_pressure, &bme680_gas_resistance);
    #endif

    #if environmental_sensor == 1
      // create aprs comment with payload data
      String aprs_comment = aprs_module.create_comment(gps_altitude, radio_packet_counter, ds18b20_temperature, mcu_input_voltage, solar_input_voltage, gps_speed, gps_course, gps_satellites, additional_aprs_comment);
    #elif environmental_sensor == 2
      // create aprs comment with payload data
      String aprs_comment = aprs_module.create_comment(gps_altitude, radio_packet_counter, ntc_temperature, mcu_input_voltage, solar_input_voltage, gps_speed, gps_course, gps_satellites, additional_aprs_comment);
    #elif environmental_sensor == 3
      // create aprs comment with payload data
      String aprs_comment = aprs_module.create_comment(gps_altitude, radio_packet_counter, bme680_temperature, bme680_humidity, bme680_pressure, bme680_gas_resistance, mcu_input_voltage, solar_input_voltage, gps_speed, gps_course, gps_satellites, additional_aprs_comment);
    #else
      // create aprs comment with payload data
      String aprs_comment = aprs_module.create_comment(gps_altitude, radio_packet_counter, mcu_input_voltage, solar_input_voltage, gps_speed, gps_course, gps_satellites, additional_aprs_comment); 
    #endif
    
    aprs_module.send_position_packet(gps_latitude_DMH, gps_longitude_DMH, aprs_comment, freq);

    // increase aprs packet counter by one
    radio_packet_counter = radio_packet_counter + 1;
    
    gps_module.m_pgps_serial->begin(gps_baud_rate);   // needed for aprs to properly work
  }

  #elif radio_protocol == 2
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

      #if environmental_sensor == 1
      // get temperature value from DS18B20 sensor
        ds18b20_temperature = ds18b20_module.get_ds18b20_temperature();
      #elif environmental_sensor == 2
        // get temperature value from DS18B20 sensor
        ntc_temperature = ntc_temperature_sensor_module.get_ntc_temperature();
      #elif environmental_sensor == 3
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
      
      #if environmental_sensor == 1
        // encode ds18b20 temperature
        char base91_encoded_ds18b20_temperature[3];
        gps_module.base91_encode_gps_coordinates(base91_encoded_ds18b20_temperature, ds18b20_temperature+1000, 2);
        
        // create aprs comment with payload data
        String payload_data = lora_module.create_payload_data(base91_encoded_flight_number, base91_encoded_gps_latitude, base91_encoded_gps_longitude, base91_encoded_gps_altitude, base91_encoded_input_speed_course, base91_encoded_radio_packet_counter, base91_encoded_ds18b20_temperature, base91_encoded_input_voltages, base91_encoded_gps_satellites);
      #elif environmental_sensor == 2
        // encode ntc temperature
        char base91_encoded_ntc_temperature[3];
        gps_module.base91_encode_gps_coordinates(base91_encoded_ntc_temperature, ntc_temperature+1000, 2);
        
        // create aprs comment with payload data
        String payload_data = lora_module.create_payload_data(base91_encoded_flight_number, base91_encoded_gps_latitude, base91_encoded_gps_longitude, base91_encoded_gps_altitude, base91_encoded_input_speed_course, base91_encoded_radio_packet_counter, base91_encoded_ntc_temperature, base91_encoded_input_voltages, base91_encoded_gps_satellites);
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
