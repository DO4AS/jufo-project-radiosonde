// Hello LoRa Single Channel - ABP TTN Single Channel Packet Sender
// Tutorial Link: https://learn.adafruit.com/the-things-network-for-feather/using-a-feather-32u4
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Copyright 2015, 2016 Ideetron B.V.
//
// Modified by Brent Rubell for Adafruit Industries, 2018
/************************** Configuration ***********************************/
#include <TinyLoRa.h>
#include <SPI.h>

// Visit your thethingsnetwork.org device console
// to create an account, or if you need your session keys.

// Network Session Key (MSB)
uint8_t NwkSkey[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Application Session Key (MSB)
uint8_t AppSkey[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Device Address (MSB)
uint8_t DevAddr[4] = { 0x00, 0x00, 0x00, 0x00 };

/************************** Example Begins Here ***********************************/
// Data Packet to Send to TTN
unsigned char loraData[11] = {"hello LoRa"};

// How many times data transfer should occur, in seconds
const unsigned int sendInterval = 30;

// Pinout for Feather 32u4 LoRa
TinyLoRa lora = TinyLoRa(7, 8, 4);

// Pinout for Feather M0 LoRa
//TinyLoRa lora = TinyLoRa(3, 8, 4);

void setup()
{
  delay(2000);
  Serial.begin(9600);
  while (! Serial);
 
  // Initialize pin LED_BUILTIN as an output
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Initialize LoRa
  Serial.print("Starting LoRa...");
  // define channel to send data on
  lora.setChannel(CH2);
  // set datarate
  lora.setDatarate(SF7BW125);
    if(!lora.begin())
  {
    Serial.println("Failed");
    Serial.println("Check your radio");
    while(true);
  }
  Serial.println("OK");
}

void loop()
{
  Serial.println("Sending LoRa Data...");
  lora.sendData(loraData, sizeof(loraData), lora.frameCounter);
  Serial.print("Frame Counter: ");Serial.println(lora.frameCounter);
  lora.frameCounter++;

  // blink LED to indicate packet sent
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.println("delaying...");
  delay(sendInterval * 1000);
}
