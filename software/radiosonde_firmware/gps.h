#ifndef GPS_H_
#define GPS_H_

#include <TinyGPS++.h>
#include <AltSoftSerial.h>

class gps
{
  public:

    gps();
    ~gps();
  
    TinyGPSPlus *m_pgps_device;
    AltSoftSerial *m_pgps_serial;
     
    void convert_gps_coordinates_to_DMH(float gps_latitude, float gps_longitude, String* gps_latitude_DMH, String* gps_longitude_DMH);
};

#endif /* GPS_H_ */
