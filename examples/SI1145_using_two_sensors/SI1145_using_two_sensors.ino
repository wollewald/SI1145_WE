/***************************************************************************
* Example sketch for the SI1145_WE library
*
* This sketch shows how to use two SI1145 sensors.
*  
* Further information can be found on:
* https://wolles-elektronikkiste.de/en/si1145-als-ir-uv-und-ps-sensor
* 
***************************************************************************/

#include <SI1145_WE.h>
#include <Wire.h>
const int sensor2EnablePin = 10;

SI1145_WE sensor1 = SI1145_WE();
SI1145_WE sensor2 = SI1145_WE();
// You can also pass a Wire object like wire2:
// SI1145_WE sensor1 = SI1145_WE(&wire2);

void setup() {
  pinMode(sensor2EnablePin, OUTPUT);
  Serial.begin(9600);
  Wire.begin();
  sensor1.init();
  sensor1.setI2CAddress(0x59); // change from 0x60 to 0x59
  digitalWrite(sensor2EnablePin, HIGH);
  sensor2.init(); // gets the standard address 0x60
  
  sensor1.enableMeasurements(ALS_TYPE, AUTO);
  sensor2.enableMeasurements(ALS_TYPE, AUTO);
  
  Serial.println("SI1145 - using two sensors");
}

void loop() {
  byte failureCode = 0;
  unsigned int amb1_als = 0, amb2_als = 0;
   
  amb1_als = sensor1.getAlsVisData();
  amb2_als = sensor2.getAlsVisData();

  Serial.print("Ambient Light Sensor 1: ");
  Serial.println(amb1_als);

  Serial.print("Ambient Light Sensor 2: ");
  Serial.println(amb2_als);
 
  Serial.println("---------");
  delay(1000);
}
