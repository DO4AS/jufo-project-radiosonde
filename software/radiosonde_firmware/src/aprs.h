#ifndef APRS_H_
#define APRS_H_

#include <SPI.h>                                                                                   
#include <SX127XLT.h>

#include "config.h"

#define _1200   1
#define _2400   0

#define _FLAG       0x7e
#define _CTRL_ID    0x03
#define _PID        0xf0
#define _DT_POS     '!'
#define _DT_UDDF	'{'
#define _DT_UDDF_UI	'{'


// SX1278 hardware pin definitions 
#define NSS 5                                 
#define NRESET 7                                                               
#define DIO0 -1                                
#define DIO1 -1                               
#define DIO2 -1     

// use SX1278 radio module
#define LORA_DEVICE DEVICE_SX1278 
class aprs 
{

	public:

    aprs();
    ~aprs();
    
    SX127XLT* m_pSX1278; 

    void begin(void);

    #if environmental_sensor == 1 or environmental_sensor == 2
      String create_comment(long altitude, int aprs_packet_number, int temperature, int mcu_vin, int solar_vin, int gps_speed, int gps_course, int number_of_gps_satellites, String additional_comment);
    #elif environmental_sensor == 3
      String create_comment(long altitude, int aprs_packet_number, int bme680_temperature, int bme680_humidity, long int bme680_pressure, int bme680_gas_resistance, int vin_voltage, int solar_vin, int gps_speed, int gps_course, int number_of_gps_satellites, String additional_comment);
    #else
       String create_comment(long altitude, int aprs_packet_number, int vin_voltage, int solar_vin, int gps_speed, int gps_course, int number_of_gps_satellites, String additional_comment);
    #endif

	void send_position_packet(String latitude, String longitude, String comment, float frequency);

	#ifdef enable_position_caching
		void send_custom_packet(char* custom_payload_info, float frequency);
	#endif


	private:

		bool nada = _2400;

		unsigned int tc1200 = (unsigned int)(0.5 * aprs_afsk_tone_adjustment * 1000000.0 / 1200.0);
		unsigned int tc2400 = (unsigned int)(0.5 * aprs_afsk_tone_adjustment * 1000000.0 / 2400.0);

		unsigned int tx_delay = 5000;
		unsigned int str_len = 400;

		char bit_stuff = 0;
		unsigned short crc=0xffff;
    
		void set_nada_1200(void);
		void set_nada_2400(void);
		void set_nada(bool nada);
		void calc_crc(bool in_bit);
		void send_crc(void);
		void send_header(String source_callsign, char source_ssid);
		void send_char_NRZI(unsigned char in_byte, bool enBitStuff);
		void send_string_len(const char *in_string, int len);
		void send_flag(unsigned char flag_len);
};

#endif /* APRS_H_ */
