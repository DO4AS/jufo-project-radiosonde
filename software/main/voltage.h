#ifndef VOLTAGE_H_
#define VOLTAGE_H_

#include "config.h"

class voltage_measurement
{
  public:

    voltage_measurement();
    ~voltage_measurement();

    void get_voltage_measurements(int* p_mcu_voltage, int* p_solar_voltage);
};

#endif /* VOLTAGE_H_ */
