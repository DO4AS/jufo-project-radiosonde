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

#ifndef BME680_H_
#define BME680_H_

#include "Zanshin_BME680.h"


class bme680
{
  public:

    bme680();
    ~bme680();

    void begin(void);
    void get_environmental_readings(long int* temperature, long int* humidity, long int* pressure, long int* gas_resistance);
    
  private:
  
    BME680_Class* m_pbme_680;
     
};

#endif /* BME680_H_ */
