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
#include "Zanshin_BME680.h"

#include "bme680.h"


bme680::bme680()
: m_pbme_680(NULL)
{ 
  
}

bme680::~bme680() 
{  
  if (m_pbme_680)
  {
    delete m_pbme_680;
    m_pbme_680 = NULL;
  }
}

void bme680::begin(void)
{
  // initialize I2C connection 
  m_pbme_680->begin(I2C_STANDARD_MODE, 0x76);

  m_pbme_680->setOversampling(TemperatureSensor, Oversample16);
  m_pbme_680->setOversampling(HumiditySensor, Oversample16);
  m_pbme_680->setOversampling(PressureSensor, Oversample16);
  m_pbme_680->setIIRFilter(IIR4);
  m_pbme_680->setGas(320, 150);  // heat at 593.15 k for 150 milliseconds
}

// returns environmental snesor data readings from BME680
void bme680::get_environmental_readings(long int* temperature, long int* humidity, long int* pressure, long int* gas_resistance)
{
  m_pbme_680->getSensorData(*temperature, *humidity, *pressure, *gas_resistance);
}
