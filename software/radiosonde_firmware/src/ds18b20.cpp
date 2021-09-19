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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
