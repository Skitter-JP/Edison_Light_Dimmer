#include <I2C_Anything.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ACS712.h"
#include <Wire.h>

// Triac1 0x28, 0x16, 0x44, 0x85, 0x08, 0x00, 0x00, 0x7C 
// Triac2 0x28, 0x7B, 0xC5, 0x85, 0x08, 0x00, 0x00, 0xF3
// Ind1 0x28, 0x55, 0x0D, 0x85, 0x08, 0x00, 0x00, 0x4A
// Ind2 0x28, 0xFF, 0xC2, 0x8E, 0xC1, 0x16, 0x04, 0xFA
// Relay1 0x28, 0xAB, 0x47, 0x84, 0x08, 0x00, 0x00 ,0xF0
// Relay2 0x28, 0xFF, 0x61, 0xB9, 0xB3, 0x16, 0x03, 0x6A


ACS712 Isensor(ACS712_20A, A0); 

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2 

// Lower resolution
#define TEMPERATURE_PRECISION 9 

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found

unsigned long previousMillis = 0;      

const long interval = 1000; //Update time for sensors

//In case there is an error this flag will be set true

//Addresses for the Sensors
uint8_t temp1[8] = {0x28, 0x7B, 0xC5, 0x85, 0x08, 0x00, 0x00, 0xF3 }; 	//TriTemp1
uint8_t temp2[8] = {0x28, 0x16, 0x44, 0x85, 0x08, 0x00, 0x00, 0x7C };	//TriTemp2
uint8_t temp3[8] = {0x28, 0x55, 0x0D, 0x85, 0x08, 0x00, 0x00, 0x4A };	//IndTemp1
uint8_t temp4[8] = {0x28, 0xFF, 0xC2, 0x8E, 0xC1, 0x16, 0x04, 0xFA };	//IndTemp2
uint8_t temp5[8] = {0x28, 0xAB, 0x47, 0x84, 0x08, 0x00, 0x00 ,0xF0 };	//RelayTemp1
uint8_t temp6[8] = {0x28, 0xFF, 0x61, 0xB9, 0xB3, 0x16, 0x03, 0x6A };	//RelayTemp2

//Array to send over I2C
float data[7] = {0,0,0,0,0,0,0};

void setup(void)
{

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  Serial.begin(9600);
  sensors.begin(); //Begin Temps  
  sensors.setResolution(temp1, 9); 
  sensors.setResolution(temp2, 9); 
  sensors.setResolution(temp3, 9);
  sensors.setResolution(temp4, 9);
  sensors.setResolution(temp5, 9);
  sensors.setResolution(temp6, 9);
  Isensor.calibrate(); // Set up Current Sensor. Maybe this is not needed.
  
}

void loop(void)
{ 

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval ) {   
      previousMillis = currentMillis;
      sensors.requestTemperatures(); // Send the command to get temperatures
      delay(2);
      data[0] = sensors.getTempC(temp1);
      data[1] = sensors.getTempC(temp2);
      data[2] = sensors.getTempC(temp3);
      data[3] = sensors.getTempC(temp4);
      data[4] = sensors.getTempC(temp5);
      data[5] = sensors.getTempC(temp6);
      data[6] = Isensor.getCurrentAC();
      data[6]=  round(data[6] * 100.0) / 100.0;
      /*
      For Testing
      data[0] = 50;
      data[1] = 60;
      data[2] = 10;
      data[3] = 20;
      data[4] = 30;
      data[5] = 40;
      data[6] = 2;
      
      */
  }
  requestEvent();
}

void requestEvent() {
  
  I2C_writeAnything( data[0]);
  I2C_writeAnything( data[1]);
  I2C_writeAnything( data[2]);
  I2C_writeAnything( data[3]);
  I2C_writeAnything( data[4]);
  I2C_writeAnything( data[5]);
  I2C_writeAnything( data[6]);
  
}
