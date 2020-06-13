/***************************************************************************
* Example sketch for the SI1145_WE library
*
* This sketch shows how to perform interrupt controlled, forced proximity 
* measurements. In order to get stable values the proximity measurement is 
* repeated 10 times. 
* 
* Further information can be found on:
* https://wolles-elektronikkiste.de/si1145
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

  /* choices: PS_TYPE, ALS_TYPE, PSALS_TYPE, ALSUV_TYPE, PSALSUV_TYPE || FORCE, AUTO, PAUSE */
  mySI1145.enableMeasurements(PS_TYPE, FORCE);
  
  /* choose gain value: 0, 1, 2, 3, 4, 5 */
  mySI1145.setPsAdcGain(0);
  mySI1145.enableInterrupt(PS_INT);
  
  /* choose LEDCurrent: value between 1 (= 5.6 mA) and 15 (= 359 mA)
   * ensure your LED is compatible */
  mySI1145.setLEDCurrent(10);

  /* select PS diode: SMALL_DIODE or LARGE_DIODE (default)
   * LARGE_DIODE is more sensitive, but might lead to 
   * overflow e.g. in sunlight */
  //mySI1145.selectPsDiode(SMALL_DIODE);
  Serial.println("SI1145 - advanced proximity");
}

void loop() {
  byte failureCode = 0;
  unsigned long proximity = 0;
   
  /* 10 proximity measurements are done to increase repeatibility */
  for(int i=0; i<10; i++){
    mySI1145.clearAllInterrupts();
    mySI1145.startSingleMeasurement();
    while(digitalRead(interruptPin)){
    /* just wait for interruptpin to go LOW; if you have not set up the 
       interrupt correctly you might wait forever! */  
    }
    proximity += mySI1145.getPsData();
  }
  proximity /= 10;
   
  Serial.print("Proximity: ");
  Serial.println(proximity);
 
  failureCode = mySI1145.getFailureMode();  // reads the response register
  if((failureCode&128)){ // if bit 7 is set in response register, there is a failure
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
