#include <Arduino.h>
#include <avr/pgmspace.h>

#include "geofence.h"
#include "config.h" 

geofence::geofence() 
{  
  
}

geofence::~geofence() 
{  

}

const float region1_vertices_lat[] = {
    28.299,
    44.586,
    74.114,
    78.767,
    61.603
};

const float region1_vertices_long[] = {  
    60.293,
    -0.176,
    14.238,
    102.128,
    -178.067
};

const float region2_vertices_lat[] = {
    7.327,
    -10.868,
    -20.666,
    -15.657,
    34.857,
    60.047,
    70.833,
    81.718,
    84.085,
    83.75,
    61.253,
    34.568
};

const float region2_vertices_long[] = {  
    -79.109,
    -65.398,
    -71.023,
    -112.859,
    -149.422,
    -166.648,
    -168.758,
    -153.64,
    -87.547,
    -59.773,
    -60.476,
    -45.359
};

const float brazil_vertices_lat[] = {
    -35.668,
    -33.497,
    -21.526,
    -2.363,
    11.446,
    10.238,
    -1.485
};

const float brazil_vertices_long[] = {  
    -44.701,
    -63.685,
    -79.154,
    -76.869,
    -66.498,
    -42.767,
    -28.705
};

const float china_vertices_lat[] = {
    17.302,
    17.47,
    23.073,
    34.88,
    45.084,
    51.285,
    50.954,
    50.732,
    50.732,
    44.209,
    24.681
};

const float china_vertices_long[] = {  
    110.932,
    97.924,
    81.577,
    73.139,
    76.655,
    89.663,
    101.44,
    116.909,
    124.467,
    131.85,
    123.061
};

const float japan_vertices_lat[] = {
    41.741,
    38.859,
    36.635,
    35.57,
    32.364,
    30.413,
    32.29,
    38.447,
    45.555,
    53.306,
    54.497
};

const float japan_vertices_long[] = {  
    137.825,
    136.77,
    133.958,
    130.266,
    128.596,
    134.573,
    147.405,
    154.524,
    151.624,
    147.493,
    142.132
};

const float thailand_vertices_lat[] = {
    7.724,
    6.678,
    7.812,
    17.154,
    21.384,
    24.299,
    28.471,
    27.384,
    24.139
};

const float thailand_vertices_long[] = {  
    97.432,
    105.079,
    113.604,
    111.759,
    108.594,
    102.003,
    97.608,
    92.335,
    88.907
};

const float newzealand_vertices_lat[] = {
    -51.63,
    -48.936,
    -44.605,
    -39.79,
    -33.301,
    -30.162,
    -47.293
};

const float newzealand_vertices_long[] = {  
    166.988,
    173.668,
    178.59,
    179.996,
    177.711,
    164.351,
    155.914
};

const float australia_vertices_lat[] = {
    -7.646,
    -15.136,
    -33.523,
    -40.864,
    -42.569,
    -40.731,
    -35.907,
    -24.147,
    -12.49,
    -2.833
};

const float australia_vertices_long[] = {  
    124.411,
    109.909,
    110.084,
    114.391,
    138.649,
    149.196,
    153.854,
    160.358,
    157.545,
    143.219
};

// check if point is in geographic region - latitude and longitude in deg
// function based on example from W. Randolph Franklin - https://wrf.ecse.rpi.edu/Research/Short_Notes/pnpoly.html
bool geofence::check_if_point_is_in_geographic_region (int number_of_vertices, const float *vertices_latitude_degrees_list, const float *vertices_longitude_degrees_list, float test_point_latitude, float test_point_longitude)
{
  bool c = 0;
  int i, j = 0;
  for (i = 0, j = number_of_vertices-1; i < number_of_vertices; j = i++) {
    if ( ((vertices_longitude_degrees_list[i]>test_point_longitude) != (vertices_longitude_degrees_list[j]>test_point_longitude)) &&
	 (test_point_latitude < (vertices_latitude_degrees_list[j]-vertices_latitude_degrees_list[i]) * (test_point_longitude-vertices_longitude_degrees_list[i]) / (vertices_longitude_degrees_list[j]-vertices_longitude_degrees_list[i]) + vertices_latitude_degrees_list[i]) )
       c = !c;
  }
  return c;
}

// get aprs frequency depending on the region - latitude and longitude in deg
float geofence::get_aprs_frequency(float gps_latitude, float gps_longitude)
{
    // invalid gnss position
    if(gps_latitude == 0 && gps_longitude == 0) return aprs_frequency_default;
    
    // australia
    if (check_if_point_is_in_geographic_region (sizeof(australia_vertices_lat)/sizeof(australia_vertices_lat[0]), australia_vertices_lat, australia_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_australia;
    
    // newzealand
    if (check_if_point_is_in_geographic_region (sizeof(newzealand_vertices_lat)/sizeof(newzealand_vertices_lat[0]), newzealand_vertices_lat, newzealand_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_newzealand;
    
    // thailand
    if (check_if_point_is_in_geographic_region (sizeof(thailand_vertices_lat)/sizeof(thailand_vertices_lat[0]), thailand_vertices_lat, thailand_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_thailand;
    
    // japan
    if (check_if_point_is_in_geographic_region (sizeof(japan_vertices_lat)/sizeof(japan_vertices_lat[0]), japan_vertices_lat, japan_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_japan;
    
    // china
    if (check_if_point_is_in_geographic_region (sizeof(china_vertices_lat)/sizeof(china_vertices_lat[0]), china_vertices_lat, china_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_china;
    
    // brazil
    if (check_if_point_is_in_geographic_region (sizeof(brazil_vertices_lat)/sizeof(brazil_vertices_lat[0]), brazil_vertices_lat, brazil_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_brazil;
    
    // region2
    if (check_if_point_is_in_geographic_region (sizeof(region2_vertices_lat)/sizeof(region2_vertices_lat[0]), region2_vertices_lat, region2_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_region2;
    
    // region1
    if (check_if_point_is_in_geographic_region (sizeof(region1_vertices_lat)/sizeof(region1_vertices_lat[0]), region1_vertices_lat, region1_vertices_long, gps_latitude, gps_longitude)) return aprs_frequency_region1;
    
    // if no position found: transmit on default frequency
    return aprs_frequency_default;
}
