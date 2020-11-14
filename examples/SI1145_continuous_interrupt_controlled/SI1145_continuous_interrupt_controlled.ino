/***************************************************************************
* Example sketch for the SI1145_WE library
*
* This sketch shows how to do the basic measurements interrupt controlled and
* in continuous mode. The time between tests is determined by the SI1145's 
* rate.  
* 
* Further information can be found on:
* https://wolles-elektronikkiste.de/si1145-als-ir-uv-und-ps-sensor?lang=en
* 
***************************************************************************/

#include <SI1145_WE.h>
#include <Wire.h>
const int interruptPin = 2;

SI1145_WE mySI1145 = SI1145_WE();

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(interruptPin, INPUT_PULLUP);
  mySI1145.init();
  //mySI1145.enableHighSignalVisRange();
  //mySI1145.enableHighSignalIrRange();
  
  /* SI1145 wakes up periodically - a measurement rate of x means that every x 
   *  wake up a measurement is done; chose between 0 and 65535 - if you chose a low value 
   *  you should uncomment the delay in the loop */
  mySI1145.setMeasurementRate(0xFFFF);
  
  /* choices: PS_TYPE, ALS_TYPE, PSALS_TYPE, ALSUV_TYPE, PSALSUV_TYPE || FORCE, AUTO, PAUSE */
  mySI1145.enableMeasurements(PSALSUV_TYPE, AUTO);

   /* choose gain value: 0, 1, 2, 3, 4, 5, 6, 7 */ 
  mySI1145.setAlsVisAdcGain(0);
  
   /* choose gain value: 0, 1, 2, 3, 4, 5 */
  mySI1145.setPsAdcGain(0);
  Serial.println("SI1145 - basics, interrupt controlled");
}

void loop() {
  byte failureCode = 0;
  unsigned int amb_als = 0;
  unsigned int amb_ir = 0;
  unsigned int proximity = 0;
  float uv = 0.0;
  
  mySI1145.clearAllInterrupts();

  /* wait until a measurement is completed -> interrupt goes LOW */
  while(digitalRead(interruptPin)){
  /* just wait for interruptpin to go LOW; if you have not set up the 
     interrupt correctly you might wait forever! */  
  } 
  
  amb_als = mySI1145.getAlsVisData();
  amb_ir = mySI1145.getAlsIrData();
  proximity = mySI1145.getPsData();
  uv = mySI1145.getUvIndex();
  
  Serial.print("Ambient Light: ");
  Serial.println(amb_als);
  Serial.print("Infrared Light: ");
  Serial.println(amb_ir);
  Serial.print("UV-Index: ");
  Serial.println(uv);
  Serial.print("Proximity: ");
  Serial.println(proximity);
  
  failureCode = mySI1145.getFailureMode();  // reads the response register
  if((failureCode&128)){  // if bit 7 is set in response register, there is a failure
    handleFailure(failureCode);
  }
  Serial.println("---------");
  
  //Measuring frequency in this sketch is controlled by setMeasurementRate
  //delay(500); 
}

void handleFailure(byte code){
  String msg = "";
  switch(code){
    case SI1145_RESP_INVALID_SETTING:
      msg = "Invalid Setting";
      break;
    case SI1145_RESP_PS1_ADC_OVERFLOW:
      msg = "PS ADC Overflow";
      break;
    case SI1145_RESP_ALS_VIS_ADC_OVERFLOW:
      msg = "ALS VIS ADC Overflow";
      break;
    case SI1145_RESP_ALS_IR_ADC_OVERFLOW:
      msg = "ALS IR Overflow";
      break;
    case SI1145_RESP_AUX_ADC_OVERFLOW:
      msg = "AUX ADC Overflow";
      break;
    default:
      msg = "Unknown Failure";
      break;
  }
  Serial.println(msg); 
  mySI1145.clearFailure();
}
