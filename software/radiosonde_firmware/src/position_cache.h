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

#ifndef POSITION_CACHE_H_
#define POSITION_CACHE_H_

#include "config.h"

#define eeprom_stored_packet_counter_adress 1022
#define eeprom_already_send_packet_counter_adress 1023

class position_cache
{
  public:

    position_cache();
    ~position_cache();

    void write_packet_to_eeprom(int packet_index, char maidenhead_field_latitude, char maidenhead_field_longitude, int maidenhead_square, long int custom_timestamp);
    void read_packet_from_eeprom(int packet_index, char* maidenhead_field_latitude, char* maidenhead_field_longitude, int* maidenhead_square, long int* custom_timestamp);
    void read_packet_from_eeprom_raw(int packet_index, char* packet_buffer);
    unsigned long int read_timestamp_from_eeprom(int packet_index);
    byte get_eeprom_stored_packet_counter(void);
    byte get_eeprom_send_packet_counter(void);
    void update_eeprom_stored_packet_counter(byte stored_packet_counter);
    void update_eeprom_send_packet_counter(byte send_packet_counter);
    int get_eeprom_length(void);
};

#endif /* POSITION_CACHE_H_ */
