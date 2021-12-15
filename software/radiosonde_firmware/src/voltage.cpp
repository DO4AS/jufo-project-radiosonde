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

#include "voltage.h"

voltage_measurement::voltage_measurement()
{ 
  
}

voltage_measurement::~voltage_measurement() 
{  

}

// get mcu and solar voltage
void voltage_measurement::get_voltage_measurements(int* p_mcu_voltage, int* p_solar_voltage)
{
  // measure mcu input voltage
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  
  delay(3);
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA,ADSC));

  long raw_voltage = (ADCH<<8) | ADCL;

  raw_voltage = (1125300L / raw_voltage) * float(input_voltage_measurement_correction_factor);
  *p_mcu_voltage = int (raw_voltage / 10);

  // measure and calculate solar input voltage
  analogReference(DEFAULT);   // adc reference = mcu input voltage
  delay(3);
  analogRead(solar_vin_pin);  // ignore first adc reading after changing adc reference
  *p_solar_voltage = int (float(*p_mcu_voltage/1023.00) * analogRead(solar_vin_pin));
}