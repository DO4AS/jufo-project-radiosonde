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
