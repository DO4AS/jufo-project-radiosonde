#ifndef CONFIG_H_
#define CONFIG_H_

/*
 * payload  config
 */

  #define payload_flight_number 0

  #define enable_watchdog

/*
 * radio protocol  config
 */

  // note: ttn-network payload formatter for LORA is pending
  // note: position caching requires APRS
  #define radio_protocol 1 // 1 == APRS | 2 == LORA | 0 == NONE

  #define radio_packet_delay 60  // radio packet delay in sec

/*
 * radio  config
 */

  #define sx1278_radio_pwm_pin 6
  #define sx1278_frequency_correction 2700 // frequency offset in Hz
  #define sx1278_tx_power 17  // tx power in dbm (1-17 dbm)
  #define sx1278_deviation 0x32

/*
 * aprs config
 */
 
  #define aprs_source_callsign "DO4AS"
  #define aprs_source_ssid 7
  
  #define aprs_destination_callsign "APRS"
  
  #define aprs_digipeater "WIDE1"
  #define aprs_digipeater_ssid 1
  
  #define aprs_symbol_overlay '/'
  #define aprs_symbol 'O'
   
  #define aprs_frequency_default     144.800  // aprs frequency default in MHz
 
  #define aprs_frequency_region1     144.800  // aprs frequency region1 in MHz
  #define aprs_frequency_region2     144.390  // aprs frequency region2 in MHz
  #define aprs_frequency_brazil      145.570  // aprs frequency brazil in MHz
  #define aprs_frequency_china       144.640  // aprs frequency china in MHz
  #define aprs_frequency_japan       144.660  // aprs frequency japan in MHz
  #define aprs_frequency_thailand    145.525  // aprs frequency thailand in MHz
  #define aprs_frequency_newzealand  144.575  // aprs frequency newzealand in MHz
  #define aprs_frequency_australia   145.175  // aprs frequency australia in MHz

  #define aprs_afsk_tone_adjustment 0.975

  #define additional_aprs_comment "73"

  /*
  * beta: position chaching config (APRS only)
  */

  // #define enable_position_caching

  #define caching_aprs_source_callsign "DO4AS"
  #define caching_aprs_source_ssid 10

  #define position_caching_delay 2  // position caching delay in hours

  #define position_caching_max_packets_transmitted_at_once 9

  // description of APRS comment with cached positions
  /*
    JO6212152118JO6212152120JO6212152122JO6212152100JO6212152102
    |-----------|__________________________
    |                                      |
    |JO62              |12   |15 |21  |18  |
    |Maidenhead Locator|Month|Day|Year|Hour|
  */

/*
 * lora config
 */

  // note: for now set lora frequency band in TinyLora\TinyLoRa.h

  #define lora_network_session_key { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
  #define lora_app_session_key { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
  #define device_address { 0x00, 0x00, 0x00, 0x00 }

  #define lora_channel CH0 // set lora tx channel (CH0 - CH7 or MULTI)

  #define lora_datarate SF9BW125 // set lora datarate (SF7BW125 - SF12BW125)
  
/*
 * gps  config
 */

  #define gps_baud_rate 9600 

  #define gps_timeout 60  // gps timeout in sec

/*
 * environment sensor config
 */

  // note: BME680 for aprs only - due to package length restrictions with lora  
  #define environmental_sensor 1 // choose: 1 == DS18B20 | 2 == NTC | 3 == BME680 | 0 == NONE

  #define ntc_analog_pin A2
  #define ntc_number_of_analog_readings_for_averaging 5
  #define ntc_seriesresistor 10000                        // resistance in Ohm
  #define ntc_nominal_resistance 10000                    // resistance in Ohm
  #define ntc_nominal_temperature 25                      // temperature in °C
  #define ntc_beta_coefficient 3435
  
/*
 * serial  config
 */
  #define enable_serial_debug

  #define serial_baud_rate 9600

/*
 * solar config
 */
  #define input_voltage_measurement_correction_factor 1.3455

  #define solar_vin_pin A3
  
#endif /* CONFIG_H_ */