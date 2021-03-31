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
