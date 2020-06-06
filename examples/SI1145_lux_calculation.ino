
/***************************************************************************
* Example sketch for the SI1145_WE library
*
* This sketch shows how lux calculation is basically done. Depending on the 
* source of light, usage of covers or if you change measurement adjustments 
* you will need adjust the calculation factors. In order to get stable values 
* the light measurement is performed 50 times. 
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
  mySI1145.enableMeasurements(ALS_TYPE, FORCE);
  
  /* if gain is changed, this must be considered in lux calculation */ 
  mySI1145.setAlsVisAdcGain(0);
  
 /* if gain is changed, this must be considered in lux calculation */ 
  mySI1145.setPsAdcGain(0);
 
  Serial.println("SI1145 - lux calculation");
}

void loop() {
  byte failureCode = 0;
  unsigned long amb_als = 0;
  unsigned long amb_ir = 0;
  float lx = 0.0;
   
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
  lx = calcLux(amb_als,amb_ir); 
  Serial.print("Ambient Light: ");
  Serial.println(amb_als);
  Serial.print("Infrared Light: ");
  Serial.println(amb_ir);
  Serial.print("Lux: ");
  Serial.println(lx);

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

float calcLux(uint16_t vis, uint16_t ir){
  const unsigned int vis_dark = 256; // empirical value
  const unsigned int ir_dark = 250; // empirical value
  const float gainFactor = 1.0;
  const float visCoeff = 5.41; // application notes AN523
  const float irCoeff = 0.08; // application notes AN523
  const float visCountPerLux = 0.319; // for incandescent bulb (datasheet)
  const float irCountPerLux = 8.46; // for incandescent bulb (datasheet)
  const float corrFactor = 0.18; // my empirical correction factor
  
  // According to application notes AN523: 
  float lux = ((vis - vis_dark) * visCoeff - (ir - ir_dark) * irCoeff) * gainFactor;

  // the equation above does not consider the counts/Lux depending on light source type
  // I suggest the following equation
  // float lux = (((vis - vis_dark) / visCountPerLux) * visCoeff - ((ir - ir_dark) / irCountPerLux) * irCoeff) * gainFactor * corrFactor;
  
  return lux;
}
