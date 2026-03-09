/**
 * @file    mc3416.c
 * @brief   MC3416 Accelerometer Driver for STM32 HAL
 * @author  Zachary Milan
 */


#include "mc3416.h"

static I2C_HandleTypeDef* mc3416;

uint8_t MC3416_ReadReg(uint8_t reg) {
	uint8_t data = 0x00;
	uint8_t retries = 3;
	while (retries--) {
		if (HAL_I2C_Mem_Read(mc3416, MC3416_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 10) == HAL_OK){
			return data;
		}
		HAL_Delay(1);
	}
//	printf("MC3416 Read Failed\n"); // avoid printf in RTOS
	return 0x00;
}

void MC3416_WriteReg(uint8_t reg, uint8_t data) {
	uint8_t retries = 3;
	while (retries--) {
		if (HAL_I2C_Mem_Write(mc3416, MC3416_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 10) == HAL_OK) {
			return;
		}
		HAL_Delay(1);
	}
//	printf("MC3416 Write Failed\n"); // avoid printf in RTOS
}

void MC3416_EnableAnyMotionInterrupt(void) {
	uint8_t val = MC3416_ReadReg(MC3416_INTR_CTRL);
	val |= 0x04;
	MC3416_WriteReg(MC3416_INTR_CTRL, val);
}

void MC3416_DisableAnyMotionInterrupt(void) {
	uint8_t val = MC3416_ReadReg(MC3416_INTR_CTRL);
	val &= ~(0x01 << 2);
	MC3416_WriteReg(MC3416_INTR_CTRL, val);
}

void MC3416_EnableInterruptAutoClear(void) {
	uint8_t val = MC3416_ReadReg(MC3416_INTR_CTRL);
	val |= (0x01 << 6);
	MC3416_WriteReg(MC3416_INTR_CTRL, val);
}

void MC3416_EnableAnyMotionFeature(void) {
	uint8_t val = MC3416_ReadReg(MC3416_MOTION_CTRL);
	val |= (0x01 << 2);
	MC3416_WriteReg(MC3416_MOTION_CTRL, val);
}

void MC3416_DisableAnyMotionFeature(void) {
	uint8_t val = MC3416_ReadReg(MC3416_MOTION_CTRL);
	val &= ~(0x01 << 2);
	MC3416_WriteReg(MC3416_MOTION_CTRL, val);
}

void MC3416_Wake(void) {
	uint8_t val = MC3416_ReadReg(MC3416_MODE);
	val = ((val & ~(0x07)) | 0x01); // clears 3 LSB, sets STATE to 01 (WAKE)
	MC3416_WriteReg(MC3416_MODE, val);
}

void MC3416_Standby(void) {
	uint8_t val = MC3416_ReadReg(MC3416_MODE);
	val = val & ~(0x07); // clears 3 LSB. sets STATE to 00 (standby)
	MC3416_WriteReg(MC3416_MODE, val);
}

void MC3416_SetInterruptPushPull(void) {
	uint8_t val = MC3416_ReadReg(MC3416_MODE);
	val |= (0x01 << 6); //
	MC3416_WriteReg(MC3416_MODE, val);
}

void MC3416_SetInterruptActiveHighLow(uint8_t HL) {
	uint8_t val = MC3416_ReadReg(MC3416_MODE);
	if (HL) {
		val |= (0x01 << 7);
	} else {
		val &= ~(0x01 << 7);
	}
	MC3416_WriteReg(MC3416_MODE, val);
}

uint8_t MC3416_ReadInterruptReg(void) {
	return MC3416_ReadReg(MC3416_MODE);
}

// in future, could make easier for user by passing in milliseconds instead of raw byte value, and do conversion based on Sample Rate.
void MC3416_SetAnyMotionDebounce(uint8_t debounce) {
	MC3416_WriteReg(MC3416_AM_DB, debounce); // each bit is 1.952ms. use about 50 counts before interrupt is rearmed.
}

void MC3416_SetSampleRate(MC3416_SampleRate_t rate) {
	MC3416_WriteReg(MC3416_SR, (uint8_t)rate);
}

void MC3416_SetRange(MC3416_Range_t range) {
	range = range << 4; // align with bits 6-4 in range register
	range &= (0x07 << 4); // clear bottom 4 bits per datasheet
	range |= (0x09); // set bits 3, 0 per datasheet
	MC3416_WriteReg(MC3416_RANGE, (uint8_t)range);
}

void MC3416_SetAnyMotionThreshold(uint16_t threshold) {
	// split thresh into two 8bit regisers
	// USE A VALUE OF 1638 (0.1g in +-2G mode).
	uint8_t val = (uint8_t)(threshold & 0xFF);
	MC3416_WriteReg(MC3416_AM_THRESH_LSB, val);
	val = (uint8_t)((threshold >> 8) & 0x7F);
	MC3416_WriteReg(MC3416_AM_THRESH_MSB, val);
}

void MC3416_Init(I2C_HandleTypeDef *hi2c) {

	// set i2c handle
	mc3416 = hi2c;

	// Set standby mode to r/w to all registers
	MC3416_Standby();

	// settings for our use case.
	MC3416_SetSampleRate(MC3416_SR_128HZ);
	MC3416_SetRange(MC3416_RANGE_2G);
	MC3416_SetInterruptPushPull();
	MC3416_SetInterruptActiveHighLow(1);
	MC3416_SetAnyMotionDebounce(64); // 64 counts at 128HZ = 0.5ms. If moving continously, anymotion fires every 0.5ms
	MC3416_SetAnyMotionThreshold(20); // changed from 1638 to 100 for exp.
	MC3416_EnableAnyMotionFeature();
	MC3416_EnableAnyMotionInterrupt();
	MC3416_EnableInterruptAutoClear();

	// wake device to enable clocking/sampling
	MC3416_Wake();
}

void MC3416_ReadXYZ(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t buf[6];
    uint8_t retries = 3;
    while (retries--) {
        if (HAL_I2C_Mem_Read(mc3416, MC3416_ADDR, MC3416_XOUT_LSB, I2C_MEMADD_SIZE_8BIT, buf, 6, 100) == HAL_OK) {
            *x = (int16_t)((buf[1] << 8) | buf[0]);
            *y = (int16_t)((buf[3] << 8) | buf[2]);
            *z = (int16_t)((buf[5] << 8) | buf[4]);
            return;
        }
    }
//        printf("MC3416 XYZ Read Failed\n"); // avoid printf in RTOS
}






