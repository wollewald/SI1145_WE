/*****************************************
* This is a library for the SI1145 ALS, IR, UV and PS Sensor Module
*
* You'll find an example which should enable you to use the library. 
*
* You are free to use it, change it or build on it. In case you like 
* it, it would be cool if you give it a star.
* 
* If you find bugs, please inform me!
* 
* Written by Wolfgang (Wolle) Ewald
* https://wolles-elektronikkiste.de
*
*******************************************/

#include "SI1145_WE.h"

SI1145_WE::SI1145_WE(){
	i2cAddress = 0x60;
}
	
void SI1145_WE::init(){	
	/* sets default parameters */
		
	resetSI1145();
	
	/* UV default calibration values */
	setRegister(SI1145_REG_UCOEFF0, 0x29);
	setRegister(SI1145_REG_UCOEFF1, 0x89);
	setRegister(SI1145_REG_UCOEFF2, 0x02);
	setRegister(SI1145_REG_UCOEFF3, 0x00);
	
	/* Interrupt Settings */
	enableInterrupt(PSALS_INT);
	
	/* pause between measurements =  31.25 µs x rate */
	setMeasurementRate(0x00FF);
	
	/* set LED Current, 22.4 mA */
	setLEDCurrent(3); 
	
	/* prox sensor 1 uses LED1 */
	setParameter(SI1145_PARA_PSLED12_SELECT, 0x01);
	
	/* PS Photodiode Selection */
	selectPsDiode(LARGE_DIODE);
	
	/* Pulse Width Selection of IR LED pulse; pulse width = pulse width * 2^PS_ADC_Gain 
		No value > 5 shall be taken */
	setPsAdcGain(0);
	disableHighSignalPsRange();
	
	/* ALS-IR Photodiode Selection */
	selectIrDiode(SMALL_DIODE);
	
	/* Integration time for ALS-VIS measurements = integration time * 2^ALS_VIS_ADC_GAIN 
		No value > 7 (factor 128) shall be taken */
	setAlsVisAdcGain(0);
	disableHighSignalVisRange();
	
	/* Integration time for ALS-IR measurements = integration time * 2^ALS_IR_ADC_GAIN 
		No value > 7 (factor 128) shall be taken */
	setAlsIrAdcGain(0);
	disableHighSignalIrRange();
}

void SI1145_WE::resetSI1145(){
	setRegister(SI1145_REG_COMMAND, SI1145_RESET);
	delay(10);
	setHwKey();
	delay(10);
}

void SI1145_WE::setI2CAddress(int addr){
	setParameter(SI1145_PARA_I2C_ADDR, (uint8_t)addr); delay(10);
	setRegister(SI1145_REG_COMMAND, SI1145_BUSADDR); delay(10);
	i2cAddress = addr;
}

void SI1145_WE::enableMeasurements(SI1145MeasureType t, SI1145MeasureMode m){
	deviceMeasureMode = m;
	deviceMeasureType = t;
	uint8_t chListValue = 0;
	uint8_t typeAndMode = 0;
	
	uint8_t parameter; 
	switch(t){
		case PS_TYPE:
			chListValue = 0b00000001;
			break;
		case ALS_TYPE:
			chListValue = 0b00110000;   // IR wird immer mit ALS zusammen aktiviert
			break;
		case PSALS_TYPE:
			chListValue = 0b00110001;
			break;
		case ALSUV_TYPE:
			chListValue = 0b10110000;
			break;
		case PSALSUV_TYPE:
			chListValue = 0b10110001;
			break;
	}
	typeAndMode = m | t; 
	setParameter(SI1145_PARA_CHLIST, chListValue);
	setRegister(SI1145_REG_COMMAND, typeAndMode);
	delay(10);
}

void SI1145_WE::startSingleMeasurement(){
	uint8_t typeAndMode = deviceMeasureMode | deviceMeasureType;
	setRegister(SI1145_REG_COMMAND, typeAndMode);
}

void SI1145_WE::enableInterrupt(SI1145IntType t){
	setRegister(SI1145_REG_INT_CFG, 1);
	setRegister(SI1145_REG_IRQEN, t);
}

void SI1145_WE::disableAllInterrupts(){
	setRegister(SI1145_REG_INT_CFG, 0);
}

void SI1145_WE::setMeasurementRate(uint16_t rate){
	setRegister16bit(SI1145_REG_MEASRATE0, rate);
}

void SI1145_WE::setHwKey(){
	setRegister(SI1145_REG_HW_KEY, 0x17);
}

void SI1145_WE::setLEDCurrent(uint8_t curr){
	setRegister(SI1145_REG_PSLED21, curr);
}

void SI1145_WE::selectPsDiode(uint8_t diode){
	setParameter(SI1145_PARA_PS1_ADCMUX, diode);
}

void SI1145_WE::selectIrDiode(uint8_t diode){
	setParameter(SI1145_PARA_ALS_IR_ADCMUX, diode);
}

void SI1145_WE::enableHighResolutionPs(){
	setParameter(SI1145_PARA_PS_ENCODING, 0b0001000);
}

void SI1145_WE::disableHighResolutionPs(){
	setParameter(SI1145_PARA_PS_ENCODING, 0b0000000);
}

void SI1145_WE::enableHighResolutionVis(){
	uint8_t ALSEncodingStatus = getParameter(SI1145_PARA_ALS_ENCODING);
	ALSEncodingStatus |= 0b00010000;
	setParameter(SI1145_PARA_ALS_ENCODING, ALSEncodingStatus);
}

void SI1145_WE::disableHighResolutionVis(){
	uint8_t ALSEncodingStatus = getParameter(SI1145_PARA_ALS_ENCODING);
	ALSEncodingStatus &= 0b11101111;
	setParameter(SI1145_PARA_ALS_ENCODING, 0b0000000);
}

void SI1145_WE::enableHighResolutionIr(){
	uint8_t ALSEncodingStatus = getParameter(SI1145_PARA_ALS_ENCODING);
	ALSEncodingStatus |= 0b00100000;
	setParameter(SI1145_PARA_ALS_ENCODING, ALSEncodingStatus);
}

void SI1145_WE::disableHighResolutionIr(){
	uint8_t ALSEncodingStatus = getParameter(SI1145_PARA_ALS_ENCODING);
	ALSEncodingStatus &= 0b11011111;
	setParameter(SI1145_PARA_ALS_ENCODING, ALSEncodingStatus);
}

void SI1145_WE::setPsAdcGain(byte psAdcGain){      /* psAdcGain <= 5! */
	setParameter(SI1145_PARA_PS_ADC_GAIN, psAdcGain);
	/* data sheet recommendation: psAdcRec = one's complement of psAdcGain */
	byte psAdcRec = ((~psAdcGain) & 0b00000111) << 4; 
	setParameter(SI1145_PARA_PS_ADCOUNTER, psAdcRec);
}

void SI1145_WE::enableHighSignalPsRange(){
	setParameter(SI1145_PARA_PS_ADC_MISC, 0x24); // change of PS_ADC_MODE not implemented
}
	
void SI1145_WE::disableHighSignalPsRange(){
	setParameter(SI1145_PARA_PS_ADC_MISC, 0x04); // change of PS_ADC_MODE not implemented
}

void SI1145_WE::setAlsVisAdcGain(byte alsVisAdcGain){    /* alsVisAdcGain <= 7 */
	setParameter(SI1145_PARA_ALS_VIS_ADC_GAIN, alsVisAdcGain);
	/* data sheet recommendation: visAdcRec = one's complement of alsVisAdcGain */
	byte visAdcRec = ((~alsVisAdcGain) & 0b00000111) << 4;
	setParameter(SI1145_PARA_ALS_VIS_ADC_COUNTER, visAdcRec);
}

void SI1145_WE::enableHighSignalVisRange(){
	setParameter(SI1145_PARA_ALS_VIS_ADC_MISC, 0x20); // change of PS_ADC_MODE not implemented
}
	
void SI1145_WE::disableHighSignalVisRange(){
	setParameter(SI1145_PARA_ALS_VIS_ADC_MISC, 0x00); // change of PS_ADC_MODE not implemented
}

void SI1145_WE::setAlsIrAdcGain(byte alsIrAdcGain){      /* irAdcGain <= 7 */
	setParameter(SI1145_PARA_ALS_IR_ADC_GAIN, alsIrAdcGain);
	/* data sheet recommendation: irAdcRec = one's complement of alsIrAdcGain */
	byte irAdcRec = ((~alsIrAdcGain) & 0b00000111) << 4; 
	setParameter(SI1145_PARA_ALS_IR_ADC_COUNTER, irAdcRec);
}

void SI1145_WE::enableHighSignalIrRange(){
	setParameter(SI1145_PARA_ALS_IR_ADC_MISC, 0x20); 
}
	
void SI1145_WE::disableHighSignalIrRange(){
	setParameter(SI1145_PARA_ALS_IR_ADC_MISC, 0x00); 
}

uint16_t SI1145_WE::getAlsVisData(){
	return getRegister16bit(SI1145_REG_ALS_VIS_DATA0);
}

uint16_t SI1145_WE::getAlsIrData(){
	return getRegister16bit(SI1145_REG_ALS_IR_DATA0);
}

uint16_t SI1145_WE::getPsData(){
	return getRegister16bit(SI1145_REG_PS1_DATA0);
}

float SI1145_WE::getUvIndex(){
	float uvi = getRegister16bit(SI1145_REG_UV_INDEX0)/100.0;
	return uvi;
}

uint8_t SI1145_WE::getFailureMode(){
	return getRegister(SI1145_REG_RESPONSE);
}

void SI1145_WE::clearFailure(){
	setRegister(SI1145_REG_COMMAND, SI1145_NOP);
}

void SI1145_WE::clearAllInterrupts(){
	setRegister(SI1145_REG_IRQ_STAT, 0xFF);
}

void SI1145_WE::clearAlsInterrupt(){
	setRegister(SI1145_REG_IRQ_STAT, ALS_INT);
}

void SI1145_WE::clearPsInterrupt(){
	setRegister(SI1145_REG_IRQ_STAT, PS_INT);
}

void SI1145_WE::clearCmdInterrupt(){
	setRegister(SI1145_REG_IRQ_STAT, CMD_INT);
}

uint8_t SI1145_WE::getInterruptStatus(){
	return getRegister(SI1145_REG_IRQ_STAT);
}


/************************************************ 
	private functions
*************************************************/

uint8_t SI1145_WE::setParameter(uint8_t param, uint8_t cmd){
	param |= SI1145_PARAM_SET;
	setRegister(SI1145_REG_PARAM_WR, cmd);
	setRegister(SI1145_REG_COMMAND, param);
	return getRegister(SI1145_REG_PARAM_RD);
}

uint8_t SI1145_WE::getParameter(uint8_t param){
	param |= SI1145_PARAM_QUERY;
	setRegister(SI1145_REG_COMMAND, param);
	return getRegister(SI1145_REG_PARAM_RD);
}


uint8_t SI1145_WE::getRegister(uint8_t registerAddr)
{
	uint8_t data;

	Wire.beginTransmission(i2cAddress); 
	Wire.write(registerAddr); 
	Wire.endTransmission(false); 
	Wire.requestFrom(i2cAddress , 1);
	data = Wire.read(); 

	return data;
}

uint16_t SI1145_WE::getRegister16bit(uint8_t registerAddr)
{
	uint8_t data_low;
	uint8_t data_high;
	uint16_t data;

	Wire.beginTransmission(i2cAddress); 
	Wire.write(registerAddr); 
	Wire.endTransmission(false); 

	Wire.requestFrom(i2cAddress, 2);
	data_low = Wire.read(); 
	data_high = Wire.read(); 
	data = (data_high << 8)|data_low;

	return data;
}

void SI1145_WE::setRegister(uint8_t registerAddr, uint8_t data)
{
	Wire.beginTransmission(i2cAddress); 
	Wire.write(registerAddr); 
	Wire.write(data); 
	Wire.endTransmission(); 
}

void SI1145_WE::setRegister16bit(uint8_t registerAddr, uint16_t data)
{
	Wire.beginTransmission(i2cAddress); 
	Wire.write(registerAddr); 
	uint8_t temp;
	temp = data & 0xff;
	Wire.write(temp); 
	temp = (data >> 8) & 0xff;
	Wire.write(temp); 
	Wire.endTransmission(); 
}
	


