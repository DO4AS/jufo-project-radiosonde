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

#include "position_cache.h"
#include <EEPROM.h>

position_cache::position_cache()
{ 
  
}

position_cache::~position_cache() 
{  

}

void position_cache::write_packet_to_eeprom(int packet_index, char maidenhead_field_latitude, char maidenhead_field_longitude, int maidenhead_square, long int custom_timestamp)
{
    int start_address = packet_index * 6;

    // write maidenhead field longitude to eeprom
    EEPROM.update(start_address, maidenhead_field_longitude);
    
    // write maidenhead field latitude to eeprom
    EEPROM.update(start_address + 1, maidenhead_field_latitude);
    
    // write maidenhead square to eeprom
    EEPROM.update(start_address + 2, maidenhead_square);
      
    // write custom timestamp to eeprom
    EEPROM.update(start_address + 3, custom_timestamp >> 0);
    EEPROM.update(start_address + 4, custom_timestamp >> 8);
    EEPROM.update(start_address + 5, custom_timestamp >> 16);
}

void position_cache::read_packet_from_eeprom(int packet_index, char* maidenhead_field_latitude, char* maidenhead_field_longitude, int* maidenhead_square, long int* custom_timestamp)
{
    int start_address = packet_index * 6;

    // read maidenhead field longitude from eeprom
    *maidenhead_field_longitude = char (EEPROM.read(start_address));
    
    // read maidenhead field latitude from eeprom
    *maidenhead_field_latitude = char (EEPROM.read(start_address + 1));
    
    // read maidenhead square from eeprom
    *maidenhead_square = EEPROM.read(start_address + 2);
      
    // read custom timestamp from eeprom
    *custom_timestamp = ((long) EEPROM.read(start_address + 3) << 0) | 
                        ((long) EEPROM.read(start_address + 4) << 8) | 
                        ((long) EEPROM.read(start_address + 5) << 16);
}

void position_cache::read_packet_from_eeprom_raw(int packet_index, char* packet_buffer)
{
    int start_address = packet_index * 6;

    // read maidenhead field longitude from eeprom
    char maidenhead_field_longitude;
    byte maidenhead_field_longitude_dec_raw = EEPROM.read(start_address);
    // validate maidenhead field longitude first
    if(maidenhead_field_longitude_dec_raw >= 65 and maidenhead_field_longitude_dec_raw <= 90)
    {
        maidenhead_field_longitude = char (maidenhead_field_longitude_dec_raw);
    }
    else
    {
        maidenhead_field_longitude = '!';
    }

    // read maidenhead field latitude from eeprom
    char maidenhead_field_latitude;
    byte maidenhead_field_latitude_dec_raw = EEPROM.read(start_address + 1);
    // validate maidenhead field latitude first
    if(maidenhead_field_latitude_dec_raw >= 65 and maidenhead_field_latitude_dec_raw <= 90)
    {
        maidenhead_field_latitude = char (maidenhead_field_latitude_dec_raw);
    }
    else
    {
        maidenhead_field_latitude = '!';
    }

    // read maidenhead square from eeprom
    int maidenhead_square = EEPROM.read(start_address + 2);
        
    // read custom timestamp from eeprom
    long int timestamp = (long) EEPROM.read(start_address + 3) << 0 | 
                         (long) EEPROM.read(start_address + 4) << 8 | 
                         (long) EEPROM.read(start_address + 5) << 16;

    sprintf(packet_buffer, "%c%c%02d%08ld", maidenhead_field_longitude, maidenhead_field_latitude, maidenhead_square, timestamp);
    packet_buffer[14] = '\0';
}

unsigned long int position_cache::read_timestamp_from_eeprom(int packet_index)
{
    unsigned long int timestamp = ((long) EEPROM.read(packet_index*6 + 3) << 0) | 
                                  ((long) EEPROM.read(packet_index*6  + 4) << 8) | 
                                  ((long) EEPROM.read(packet_index*6  + 5) << 16);
    
    return timestamp;
}

int position_cache::get_eeprom_length(void)
{
    return(EEPROM.length());
}

byte position_cache::get_eeprom_stored_packet_counter(void)
{
    return(EEPROM.read(eeprom_stored_packet_counter_adress));
}

byte position_cache::get_eeprom_send_packet_counter(void)
{
    return(EEPROM.read(eeprom_already_send_packet_counter_adress));
}

void position_cache::update_eeprom_stored_packet_counter(byte stored_packet_counter)
{
    EEPROM.update(eeprom_stored_packet_counter_adress, stored_packet_counter);
}

void position_cache::update_eeprom_send_packet_counter(byte send_packet_counter)
{
    EEPROM.update(eeprom_already_send_packet_counter_adress, send_packet_counter);
}