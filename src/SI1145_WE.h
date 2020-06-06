/******************************************************************************
 *
 * This is a library for the SI1145 ALS, IR, UV and PS Sensor Module
 *
 * You'll find several example sketches which should enable you to use the library. 
 *
 * You are free to use it, change it or build on it. In case you like it, it would
 * be cool if you give it a star.
 *
 * If you find bugs, please inform me!
 * 
 * Written by Wolfgang (Wolle) Ewald
 * https://wolles-elektronikkiste.de
 *
 * 
 ******************************************************************************/

#ifndef SI1145_WE_H_
#define SI1145_WE_H_

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

/* registers */
#define SI1145_REG_PART_ID  		0x00
#define SI1145_REG_REV_ID  			0x01
#define SI1145_REG_SEQ_ID  			0x02
#define SI1145_REG_INT_CFG  		0x03
#define SI1145_REG_IRQEN  			0x04
#define SI1145_REG_HW_KEY			0x07
#define SI1145_REG_MEASRATE0 		0x08
#define SI1145_REG_MEASRATE1  		0x09
#define SI1145_REG_PSLED21  		0x0F
#define SI1145_REG_UCOEFF0  		0x13
#define SI1145_REG_UCOEFF1  		0x14
#define SI1145_REG_UCOEFF2  		0x15
#define SI1145_REG_UCOEFF3  		0x16
#define SI1145_REG_PARAM_WR  		0x17
#define SI1145_REG_COMMAND  		0x18
#define SI1145_REG_RESPONSE  		0x20
#define SI1145_REG_IRQ_STAT  	 	0x21
#define SI1145_REG_ALS_VIS_DATA0 	0x22
#define SI1145_REG_ALS_VIS_DATA1 	0x23
#define SI1145_REG_ALS_IR_DATA0 	0x24
#define SI1145_REG_ALS_IR_DATA1 	0x25
#define SI1145_REG_PS1_DATA0 		0x26
#define SI1145_REG_PS1_DATA1 		0x27
#define SI1145_REG_UV_INDEX0 		0x2C
#define SI1145_REG_UV_INDEX1 		0x2D
#define SI1145_REG_PARAM_RD 		0x2E
#define SI1145_REG_CHIP_STAT 		0x30

/* commands */
#define SI1145_PARAM_QUERY	0b10000000
#define SI1145_PARAM_SET 	0b10100000
#define SI1145_NOP 			0b00000000
#define SI1145_RESET    	0b00000001
#define SI1145_BUSADDR    	0b00000010
#define SI1145_GET_CAL    	0b00010010

/* response register error codes */
#define SI1145_RESP_NO_ERROR				0b00000000
#define SI1145_RESP_INVALID_SETTING			0b10000000
#define SI1145_RESP_PS1_ADC_OVERFLOW		0b10001000
#define SI1145_RESP_ALS_VIS_ADC_OVERFLOW	0b10001100
#define SI1145_RESP_ALS_IR_ADC_OVERFLOW		0b10001101
#define SI1145_RESP_AUX_ADC_OVERFLOW		0b10001110

/* parameters */
#define SI1145_PARA_I2C_ADDR 			0x00
#define SI1145_PARA_CHLIST   			0x01
#define SI1145_PARA_PSLED12_SELECT		0x02
#define SI1145_PARA_PS_ENCODING			0x05
#define SI1145_PARA_ALS_ENCODING  		0x06
#define SI1145_PARA_PS1_ADCMUX   		0x07
#define SI1145_PARA_PS_ADCOUNTER   		0x0A
#define SI1145_PARA_PS_ADC_GAIN 		0x0B
#define SI1145_PARA_PS_ADC_MISC 		0x0C
#define SI1145_PARA_ALS_IR_ADCMUX 		0x0E
#define SI1145_PARA_AUX_ADCMUX   		0x0F
#define SI1145_PARA_ALS_VIS_ADC_COUNTER	0x10
#define SI1145_PARA_ALS_VIS_ADC_GAIN 	0x11
#define SI1145_PARA_ALS_VIS_ADC_MISC 	0x12
#define SI1145_PARA_LED_REC				0x1C
#define SI1145_PARA_ALS_IR_ADC_COUNTER  0x1D
#define SI1145_PARA_ALS_IR_ADC_GAIN 	0x1E
#define SI1145_PARA_ALS_IR_ADC_MISC 	0x1F

/* diode selection */
#define SMALL_DIODE  0x00
#define LARGE_DIODE  0x03

typedef enum SI1145MeasureMode{
	FORCE = 0b00000100,   
	PAUSE = 0b00001000,
	AUTO  = 0b00001100
} mode;

typedef enum SI1145MeasureType{
	PS_TYPE 	 = 0b00000001,
	ALS_TYPE	 = 0b00000010,
	PSALS_TYPE	 = 0b00000011,
	ALSUV_TYPE	 = 0b10000010,
	PSALSUV_TYPE = 0b10000011
} type;

typedef enum SI1145IntType{
	ALS_INT 	= 0x01,
	PS_INT		= 0x04,
	PSALS_INT 	= 0x05,
	CMD_INT 	= 0x20
} intType;

class SI1145_WE
{
public:	
	SI1145_WE();
  
	void init();
	void resetSI1145();
	void setI2CAddress(int);
	void enableMeasurements(SI1145MeasureType t, SI1145MeasureMode m);
	void startSingleMeasurement();
	void enableInterrupt(SI1145IntType t);
	void disableAllInterrupts();
	void setMeasurementRate(uint16_t rate);
	void setLEDCurrent(uint8_t curr);
	void selectPsDiode(uint8_t diode);
	void selectIrDiode(uint8_t diode);
	void enableHighResolutionPs();
	void disableHighResolutionPs();
	void enableHighResolutionVis();
	void disableHighResolutionVis();
	void enableHighResolutionIr();
	void disableHighResolutionIr();
	void setPsAdcGain(byte psAdcGain);
	void enableHighSignalPsRange();
	void disableHighSignalPsRange();
	void setAlsVisAdcGain(byte alsVisAdcGain);
	void enableHighSignalVisRange();
	void disableHighSignalVisRange();
	void setAlsIrAdcGain(byte alsIrAdcGain);
	void enableHighSignalIrRange();
	void disableHighSignalIrRange();
	uint16_t getAlsVisData();
	uint16_t getAlsIrData();
	uint16_t getPsData();
	float getUvIndex();
	uint8_t getFailureMode();
	void clearFailure();
	void clearAllInterrupts();
	void clearAlsInterrupt();
	void clearPsInterrupt();
	void clearCmdInterrupt();
	uint8_t getInterruptStatus();

private:

	SI1145MeasureMode deviceMeasureMode; 
	SI1145MeasureType deviceMeasureType;
	int i2cAddress;
	
	void setHwKey();
	
	uint8_t setParameter(uint8_t param, uint8_t cmd);
	uint8_t getParameter(uint8_t param);
	
	uint8_t getRegister(uint8_t registerAddr);
	uint16_t getRegister16bit(uint8_t registerAddr);

	void setRegister(uint8_t registerAddr, uint8_t data);
	void setRegister16bit(uint8_t registerAddr, uint16_t data);

};

#endif

