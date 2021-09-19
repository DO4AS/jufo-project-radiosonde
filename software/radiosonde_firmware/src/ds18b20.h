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

#ifndef DS18B20_H_
#define DS18B20_H_

#include <OneWire.h> 
#include <DallasTemperature.h>

// ds18b20 pin - fixed
#define ds18b20_pin 2 

class ds18b20
{
  public:

    ds18b20();
    ~ds18b20();

    int get_ds18b20_temperature(void);

  private:

    OneWire* m_poneWire;
    DallasTemperature* m_pds18b20_temperature_sensor;
};

#endif /* DS18B20_H_ */
