#include "config.h"

#include "aprs.h"
#include "gps.h"
#include "geofence.h"
#include "voltage.h"

#ifdef used_ds18b20
  #include "ds18b20.h"
#elif defined used_bme680
  #include "bme680.h"
#endif

aprs aprs_module;
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
float gps_altitude;

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


float ref_millis = float(aprs_packet_delay) * -1000;

int aprs_packet_counter = 0;

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

  // begin sx1278 spi connection
  aprs_module.begin();

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
        
        // no packet data rate limitation - send aprs paket
        if((millis() - ref_millis) > float(aprs_packet_delay) * 1000) 
        {
          send_aprs_packet();
         
          ref_millis = millis();
        }     
      }
      // no packet data rate limitation
      else 
      {
        DEBUG_PRINTLN("INVALID");
        
        // send aprs packet after gps timeout
        if(millis() >= float(gps_timeout) * 1000)
        {
          DEBUG_PRINTLN(millis() - ref_millis);
        
          if((millis() - ref_millis) > float(aprs_packet_delay) * 1000)
          {
            send_aprs_packet();
           
            ref_millis = millis();
          }  
        }
      }
    }
  }
}

void send_aprs_packet()
{
  // get gps location,altitude and satellite data
  gps_latitude = gps_module.m_pgps_device->location.lat();
  gps_longitude = gps_module.m_pgps_device->location.lng();
  gps_altitude = gps_module.m_pgps_device->altitude.meters();
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
  DEBUG_PRINTLN(gps_latitude);
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
    String aprs_comment = aprs_module.create_comment(gps_altitude, aprs_packet_counter, ds18b20_temperature, mcu_input_voltage, solar_input_voltage, gps_speed, gps_course, gps_satellites, additional_aprs_comment);
  #elif defined used_bme680
    // create aprs comment with payload data
    String aprs_comment = aprs_module.create_comment(gps_altitude, aprs_packet_counter, bme680_temperature, bme680_humidity, bme680_pressure, bme680_gas_resistance, mcu_input_voltage, solar_input_voltage, gps_speed, gps_course, gps_satellites, additional_aprs_comment);
  #else
    // create aprs comment no payload data
    String aprs_comment = additional_aprs_comment;
  #endif
  
  aprs_module.send(gps_latitude_DMH, gps_longitude_DMH, aprs_comment, freq);

  // increase aprs packet counter by one
  aprs_packet_counter = aprs_packet_counter + 1;
  
  gps_module.m_pgps_serial->begin(9600);   // needed for aprs to properly work
}
