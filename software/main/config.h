#ifndef CONFIG_H_
#define CONFIG_H_

/*
 * payload  config
 */

  #define payload_flight_number 6

/*
 * aprs config
 */
 
  #define aprs_source_callsign "DO4AS"
  #define aprs_source_ssid 12
  
  #define aprs_destination_callsign "APRS"
  
  #define aprs_digipeater "WIDE1"
  #define aprs_digipeater_ssid 1
  
  #define aprs_symbol_overlay '/'
  #define aprs_symbol 'O'
  
  #define aprs_packet_delay 35  // aprs packet delay in sec
 
  #define aprs_frequency_default     144.800  // aprs frequency default in MHz
 
  #define aprs_frequency_region1     144.800  // aprs frequency region1 in MHz
  #define aprs_frequency_region2     144.390  // aprs frequency region2 in MHz
  #define aprs_frequency_brazil      145.570  // aprs frequency brazil in MHz
  #define aprs_frequency_china       144.640  // aprs frequency china in MHz
  #define aprs_frequency_japan       144.660  // aprs frequency japan in MHz
  #define aprs_frequency_thailand    145.525  // aprs frequency thailand in MHz
  #define aprs_frequency_newzealand  144.575  // aprs frequency newzealand in MHz
  #define aprs_frequency_australia   145.175  // aprs frequency australia in MHz

  #define additional_aprs_comment "RHG"

/*
 * radio  config
 */

  #define sx1278_radio_pwm_pin 6
  #define sx1278_frequency_correction 2100 // frequency offset in Hz
  #define sx1278_tx_power 17  // tx power in dbm
  #define sx1278_deviation 0x32
  
/*
 * gps  config
 */

  #define gps_baud_rate 9600 

  #define gps_timeout 35  // gps timeout in sec

/*
 * environment sensor config
 */

  #define used_ds18b20
  //#define used_bme680
  
/*
 * serial  config
 */
  // #define enable_serial_debug

  #define serial_baud_rate 9600

/*
 * solar config
 */
  #define input_voltage_measurment_correction_factor 1.3455

  #define solar_vin_pin A3
  
#endif /* CONFIG_H_ */
