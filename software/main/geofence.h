#ifndef __GEOFENCE_H__
#define __GEOFENCE_H__

class geofence
{
  public:
  
    geofence();
    ~geofence();
 
    float get_aprs_frequency(float gps_latitude, float gps_longitude);

  private:

    bool check_if_point_is_in_geographic_region (int number_of_vertices, const float *vertices_latitude_degrees_list, const float *vertices_longitude_degrees_list, float test_point_latitude, float test_point_longitude);
};

#endif /* GEOFENCE_H_ */
