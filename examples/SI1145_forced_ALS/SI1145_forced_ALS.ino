/***************************************************************************
* Example sketch for the SI1145_WE library
*
* This sketch shows how to do ALS VIS/IR measurements interrupt controlled in
* forced mode. In order to get stable results the light measurement is performed 
* 50 times. 
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
  
  /* choices: PS_TYPE, ALS_TYPE, PSALS_TYPE, ALSUV_TYPE, PSALSUV_TYPE || FORCE, AUTO, PAUSE */
  mySI1145.enableMeasurements(ALS_TYPE, FORCE);

   /* choose gain value: 0, 1, 2, 3, 4, 5, 6, 7 */ 
  mySI1145.setAlsVisAdcGain(0);

  //mySI1145.enableHighResolutionVis();
  Serial.println("SI1145 - forced ALS");
}

void loop() {
  byte failureCode = 0;
  unsigned long amb_als = 0;
  unsigned long amb_ir = 0;
   
  for(int i=0; i<50; i++){
    mySI1145.clearAllInterrupts();
    mySI1145.startSingleMeasurement();
    while(digitalRead(interruptPin)){
    /* just wait for interruptpin to go LOW; if you have not set up the 
       interrupt correctly you might wait forever! */  
    }
    amb_als += mySI1145.getAlsVisData();
    amb_ir += mySI1145.getAlsIrData();
  }
  amb_als /= 50;
  amb_ir /= 50;
  Serial.print("Ambient Light: ");
  Serial.println(amb_als);
  Serial.print("Infrared Light: ");
  Serial.println(amb_ir);
  failureCode = mySI1145.getFailureMode();  // reads the response register
  if((failureCode&128)){   // if bit 7 is set in response register, there is a failure
    handleFailure(failureCode);
  }
  Serial.println("---------");
  delay(1000);
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
