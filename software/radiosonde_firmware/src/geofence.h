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

#ifndef __GEOFENCE_H__
#define __GEOFENCE_H__

class geofence
{
  public:
  
    geofence();
    ~geofence();
 
    float get_aprs_frequency(float gps_latitude, float gps_longitude, bool* area_valid_for_cached_position_transmission);

  private:

    bool check_if_point_is_in_geographic_region (int number_of_vertices, const float *vertices_latitude_degrees_list, const float *vertices_longitude_degrees_list, float test_point_latitude, float test_point_longitude);
};

#endif /* GEOFENCE_H_ */
