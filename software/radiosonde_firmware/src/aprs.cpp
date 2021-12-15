// AFSK modulator based on example from github.com/handiko/ Arduino-APRS

#include <Arduino.h>
#include <math.h>
#include <stdio.h>
#include <SPI.h>                                       
#include <SX127XLT.h>

#include "aprs.h"
#include "config.h"

aprs::aprs()
: m_pSX1278(NULL)
{   
  if (!m_pSX1278)
  {
    m_pSX1278 = new SX127XLT();
  }
}

aprs::~aprs() 
{  
  if (m_pSX1278)
  {
    delete m_pSX1278;
    m_pSX1278 = NULL;
  }
}

void aprs::begin(void) 
{
  // init sx1278 radio connection
  SPI.begin();
  m_pSX1278->begin(NSS, NRESET, DIO0, DIO1, DIO2, LORA_DEVICE);
 
  pinMode(sx1278_radio_pwm_pin, OUTPUT);
}

#if environmental_sensor == 1 or environmental_sensor == 2
  String aprs::create_comment(long altitude, int aprs_packet_number, int temperature, int mcu_vin, int solar_vin, int gps_speed, int gps_course, int number_of_gps_satellites, String additional_comment)
  {
    char comment_buffer[55];

    // altitude*3.28084 - for conversion from meter to feet
    sprintf(comment_buffer, "/A=%06ld/F%dN%dT%dE%dY%dV%dC%dS%d %s" , long((altitude*3.28084)), payload_flight_number, aprs_packet_number, temperature, mcu_vin, solar_vin, gps_speed, gps_course, number_of_gps_satellites, additional_comment.c_str());

    return String(comment_buffer);
  }

#elif environmental_sensor == 3
  String aprs::create_comment(long altitude, int aprs_packet_number, int bme680_temperature, int bme680_humidity, long int bme680_pressure, int bme680_gas_resistance, int mcu_vin, int solar_vin, int gps_speed, int gps_course, int number_of_gps_satellites, String additional_comment)
  {
    char comment_buffer[80];

    // altitude*3.28084 - for conversion from meter to feet
    sprintf(comment_buffer, "/A=%06ld/F%dN%dT%dH%dP%6ldG%dE%dY%dV%dC%dS%d %s" , long((altitude*3.28084)), payload_flight_number, aprs_packet_number, bme680_temperature, bme680_humidity, bme680_pressure, bme680_gas_resistance, mcu_vin, solar_vin, gps_speed, gps_course, number_of_gps_satellites, additional_comment.c_str());

    return String(comment_buffer);
  }

#else
  String aprs::create_comment(long altitude, int aprs_packet_number, int mcu_vin, int solar_vin, int gps_speed, int gps_course, int number_of_gps_satellites, String additional_comment)
  {
    char comment_buffer[55];

    // altitude*3.28084 - for conversion from meter to feet
    sprintf(comment_buffer, "/A=%06ld/F%dN%dE%dY%dV%dC%dS%d %s" , long((altitude*3.28084)), payload_flight_number, aprs_packet_number, mcu_vin, solar_vin, gps_speed, gps_course, number_of_gps_satellites, additional_comment.c_str());

    return String(comment_buffer);
  }
#endif

void aprs::send_position_packet(String latitude, String longitude, String comment, float frequency) 
{
  // setup sx1278 radio in "continuous" mode and enable tx
  m_pSX1278->setupDirect(frequency * 1000000, sx1278_frequency_correction);
  m_pSX1278->setTXDirect();
  m_pSX1278->setTxParams(sx1278_tx_power, RADIO_RAMP_DEFAULT);             
  m_pSX1278->writeRegister(REG_FDEVLSB, sx1278_deviation);                  
  m_pSX1278->writeRegister(REG_PLLHOP, 0xAD); 

  // send ax.25 flag, header, payload information (comment), fcs (crc) and flag
  send_flag(100);
  crc = 0xffff;
  send_header(aprs_source_callsign, aprs_source_ssid);

  // send ! for uncompressed position packet
	send_char_NRZI(_DT_POS, HIGH);

  // send latitude
	send_string_len(latitude.c_str(), latitude.length());

  // send symbol table id
	send_char_NRZI(aprs_symbol_overlay, HIGH);

  // send longitude
	send_string_len(longitude.c_str(), longitude.length());

  // send symbol code
	send_char_NRZI(aprs_symbol, HIGH);

  // send further payload information (comment)
	send_string_len(comment.c_str(), comment.length());

  // send crc
  send_crc();
  send_flag(3);

  // reset sx1278 radio and disable tx
  m_pSX1278->resetDevice();
}

#ifdef enable_position_caching
  void aprs::send_custom_packet(char* custom_payload_info, float frequency) 
  {
    // setup sx1278 radio in "continuous" mode and enable tx
    m_pSX1278->setupDirect(frequency * 1000000, sx1278_frequency_correction);
    m_pSX1278->setTXDirect();
    m_pSX1278->setTxParams(sx1278_tx_power, RADIO_RAMP_DEFAULT);             
    m_pSX1278->writeRegister(REG_FDEVLSB, sx1278_deviation);                  
    m_pSX1278->writeRegister(REG_PLLHOP, 0xAD); 

    // send ax.25 flag, header, payload information , fcs (crc) and flag
    send_flag(100);
    crc = 0xffff;
    send_header(caching_aprs_source_callsign, caching_aprs_source_ssid);

    // send { for user-defined data format
    send_char_NRZI(_DT_UDDF, HIGH);
    // send { for user-defined data format - user id / type
    send_char_NRZI(_DT_UDDF_UI, HIGH);
    send_char_NRZI(_DT_UDDF_UI, HIGH);

    // send further payload information (comment)
    send_string_len(custom_payload_info, strlen(custom_payload_info));
    send_crc();
    send_flag(3);

    // reset sx1278 radio and disable tx
    m_pSX1278->resetDevice();
  }
#endif

void aprs::set_nada_1200(void) 
{
  digitalWrite(sx1278_radio_pwm_pin, HIGH);
  delayMicroseconds(tc1200);
  digitalWrite(sx1278_radio_pwm_pin, LOW);
  delayMicroseconds(tc1200);
}

void aprs::set_nada_2400(void) 
{
  digitalWrite(sx1278_radio_pwm_pin, HIGH);
  delayMicroseconds(tc2400);
  digitalWrite(sx1278_radio_pwm_pin, LOW);
  delayMicroseconds(tc2400);
  
  digitalWrite(sx1278_radio_pwm_pin, HIGH);
  delayMicroseconds(tc2400);
  digitalWrite(sx1278_radio_pwm_pin, LOW);
  delayMicroseconds(tc2400);
}

void aprs::set_nada(bool nada) 
{
  if(nada)
	set_nada_1200();
  else
	set_nada_2400();
}


void aprs::calc_crc(bool in_bit) 
{
  unsigned short xor_in;

  xor_in = crc ^ in_bit;
  crc >>= 1;

  if(xor_in & 0x01)
	crc ^= 0x8408;
}

void aprs::send_crc(void) 
{
  unsigned char crc_lo = crc ^ 0xff;
  unsigned char crc_hi = (crc >> 8) ^ 0xff;

  send_char_NRZI(crc_lo, HIGH);
  send_char_NRZI(crc_hi, HIGH);
}

void aprs::send_header(String source_callsign, char source_ssid) 
{
  char temp;

  // send destination adress
  temp = String(aprs_destination_callsign).length();
  for(int j=0; j<temp; j++)
    send_char_NRZI(String(aprs_destination_callsign)[j] << 1, HIGH);
  if(temp < 6) 
  {
    for(int j=0; j<(6 - temp); j++)
      send_char_NRZI(' ' << 1, HIGH);
  }
  send_char_NRZI('0' << 1, HIGH);


  // send source adress
  temp = source_callsign.length();
  for(int j=0; j<temp; j++)
    send_char_NRZI(String(source_callsign)[j] << 1, HIGH);
  if(temp < 6) 
  {
    for(int j=0; j<(6 - temp); j++)
      send_char_NRZI(' ' << 1, HIGH);
  }
 
  send_char_NRZI((source_ssid + '0') << 1, HIGH);


  // send digipeater adresses
  temp = String(aprs_digipeater).length();
  for(int j=0; j<temp; j++)
    send_char_NRZI(String(aprs_digipeater)[j] << 1, HIGH);
  if(temp < 6) 
  {
    for(int j=0; j<(6 - temp); j++)
      send_char_NRZI(' ' << 1, HIGH);
  }
  send_char_NRZI(((aprs_digipeater_ssid + '0') << 1) + 1, HIGH);

  // send control field and protocol field
  send_char_NRZI(_CTRL_ID, HIGH);
  send_char_NRZI(_PID, HIGH);
}

void aprs::send_char_NRZI(unsigned char in_byte, bool enBitStuff) 
{
  bool bits;

  for(int i = 0; i < 8; i++) 
  {
    bits = in_byte & 0x01;

    calc_crc(bits);

    if(bits)
    {
      set_nada(nada);
      bit_stuff++;

      if((enBitStuff) && (bit_stuff == 5))
      {
        nada ^= 1;
        set_nada(nada);

        bit_stuff = 0;
      }
    }
    else 
    {
      nada ^= 1;
      set_nada(nada);

      bit_stuff = 0;
    }

    in_byte >>= 1;
  }
}

void aprs::send_string_len(const char *in_string, int len) 
{
  for(int j=0; j<len; j++)
    send_char_NRZI(in_string[j], HIGH);
}

void aprs::send_flag(unsigned char flag_len) 
{
  for(int j=0; j<flag_len; j++)
    send_char_NRZI(_FLAG, LOW);
}
