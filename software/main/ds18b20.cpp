#include <Arduino.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

#include "ds18b20.h"

ds18b20::ds18b20()
: m_poneWire(NULL),
  m_pds18b20_temperature_sensor(NULL)
{
  if (!m_poneWire)
  {
    m_poneWire = new OneWire(ds18b20_pin);
  }

  if(!m_pds18b20_temperature_sensor)
  {
  m_pds18b20_temperature_sensor = new DallasTemperature(m_poneWire);
  }
}

ds18b20::~ds18b20() 
{  
  if (m_poneWire)
  {
    delete m_poneWire;
    m_poneWire = NULL;
  }

  if (m_pds18b20_temperature_sensor)
  {
    delete m_pds18b20_temperature_sensor;
    m_pds18b20_temperature_sensor = NULL;
  }
}

// returns temperature from DS18B20 sensor
int ds18b20::get_ds18b20_temperature(void)
{
  m_pds18b20_temperature_sensor->requestTemperatures();
  return(m_pds18b20_temperature_sensor->getTempCByIndex(0));
}
