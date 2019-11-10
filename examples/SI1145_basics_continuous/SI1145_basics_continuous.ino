/***************************************************************************
* Example sketch for the SI1145_WE library
*
* This sketch shows how to do PS, ALS, VIS an UV measurements in continuous mode.
*  
* Further information can be found on:
* http://wolles-elektronikkiste.de/si1145
* 
***************************************************************************/

#include <SI1145_WE.h>
#include <Wire.h>

SI1145_WE mySI1145 = SI1145_WE();

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mySI1145.init();

  /* in case you want to change the I2C Address */
  //mySI1145.setI2CAddress(0x59); 
  
  mySI1145.enableHighSignalVisRange(); // Gain divided by 14.5
  mySI1145.enableHighSignalIrRange(); // Gain divided by 14.5
  
  /* choices: PS, ALS, PSALS, ALSUV, PSALSUV || FORCE, AUTO, PAUSE */
  mySI1145.enableMeasurements(PSALSUV, AUTO);
  
  /* choose gain value: 0, 1, 2, 3, 4, 5, 6, 7 */ 
  //mySI1145.setAlsVisAdcGain(0);
  
  /* choose gain value: 0, 1, 2, 3, 4, 5 */
  //mySI1145.setPsAdcGain(0);

  /* if uncommented, the least 16 bit of the 17 bit ADC will be read */
  //mySI1145.enableHighResolutionVis();
  Serial.println("SI1145 - basics - continuous");
}

void loop() {
  byte failureCode = 0;
  unsigned int amb_als = 0;
  unsigned int amb_ir = 0;
  unsigned int proximity = 0;
  float uv = 0.0;
  
  amb_als = mySI1145.getAlsVisData();
  amb_ir = mySI1145.getAlsIrData();

  /* uncomment if you want to perform PS measurements */ 
  // proximity = mySI1145.getPsData();

  uv = mySI1145.getUvIndex();
  
  Serial.print("Ambient Light: ");
  Serial.println(amb_als);
  Serial.print("Infrared Light: ");
  Serial.println(amb_ir);
  Serial.print("UV-Index: ");
  Serial.println(uv);
  /* uncomment if you perform PS measurements */
  // Serial.print("Proximity: ");
  // Serial.println(proximity);
  failureCode = mySI1145.getFailureMode();  // reads the response register
  if((failureCode&128)){  // if bit 7 is set in response register, there is a failure
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
