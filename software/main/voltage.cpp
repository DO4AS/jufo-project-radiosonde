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

  raw_voltage = (1125300L / raw_voltage) * float(input_voltage_measurment_correction_factor);
  *p_mcu_voltage = int (raw_voltage / 10);

  // measure and calculate solar input voltage
  analogReference(DEFAULT);   // adc reference = mcu input voltage
  delay(3);
  analogRead(solar_vin_pin);  // ignore first adc reading after changing adc reference
  *p_solar_voltage = int (float(*p_mcu_voltage/1023.00) * analogRead(solar_vin_pin));
}