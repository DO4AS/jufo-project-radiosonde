#include <Arduino.h>

#include <TinyGPS++.h>
#include <AltSoftSerial.h>

#include "gps.h"

#include "config.h"

gps::gps()
: m_pgps_device(NULL),
  m_pgps_serial(NULL)
{  
  if (!m_pgps_device)
  {
    m_pgps_device = new TinyGPSPlus();
  }

  if (!m_pgps_serial)
  {
    m_pgps_serial = new AltSoftSerial();
  }
}

gps::~gps() 
{  
  if (m_pgps_device) 
  {  
    delete m_pgps_device;
    m_pgps_device = NULL;
  }
  
  if (m_pgps_serial)
  {
    delete m_pgps_serial;
    m_pgps_serial = NULL;
  }
}

/*
 * GNSS coordinates the format conversion in degrees, minutes and hundredths of a minute
 * N = north; S = south; E = east; W = west
 * more information at: http://www.aprs.org/doc/APRS101.PDF
 */

void gps::convert_gps_coordinates_to_DMH(float gps_latitude, float gps_longitude, String* gps_latitude_DMH, String* gps_longitude_DMH)
{
  int degrees_buffer;
  int minutes_buffer;
  int h_minutes_buffer;
  
  char latitude_compass_direction;
  
  // determine whether to append "N" or "S".
  if ((int) gps_latitude > 0 == true) 
  {
    latitude_compass_direction = 'N'; 
  }
  else
  {
    gps_latitude = -(gps_latitude);
    latitude_compass_direction = 'S';
  }
  
  // determine full degrees
  degrees_buffer = (int) (gps_latitude);

  // determine degree minutes from the remainder
  gps_latitude = gps_latitude - (degrees_buffer);
  gps_latitude = (gps_latitude) * 60;
  minutes_buffer = (int) (gps_latitude);

  // determine hundredths of degree minutes from the remainder
  h_minutes_buffer = (int) ((gps_latitude - minutes_buffer) * 100);
  
  // generate postion data string
  char latitude_coordinates_buffer[12];
  sprintf(latitude_coordinates_buffer, "%02d%02d.%02d%c" , degrees_buffer, minutes_buffer, h_minutes_buffer, latitude_compass_direction);
  *gps_latitude_DMH = String(latitude_coordinates_buffer);

  char longitude_compass_direction;
 
   // determine whether to append "E" or "W".
  if ((int) gps_longitude > 0 == true) 
  {
    longitude_compass_direction = 'E'; 
  }
  else
  {
    gps_longitude = -(gps_longitude);
    longitude_compass_direction = 'W';
  }
  
  // determine full degrees
  degrees_buffer = (int) (gps_longitude);

  // determine degree minutes from the remainder
  gps_longitude = gps_longitude - (degrees_buffer);
  gps_longitude = (gps_longitude) * 60;
  minutes_buffer = (int) (gps_longitude);

  // determine hundredths of degree minutes from the remainder
  h_minutes_buffer = (int) ((gps_longitude - minutes_buffer) * 100);

  // generate postion data string
  char longitude_coordinates_buffer[13];
  sprintf(longitude_coordinates_buffer, "%03d%02d.%02d%c" , degrees_buffer, minutes_buffer, h_minutes_buffer, longitude_compass_direction);
  *gps_longitude_DMH = String(longitude_coordinates_buffer);
}
