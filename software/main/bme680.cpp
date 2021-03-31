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
