/**
 * @file    drv2605l.c
 * @brief   DRV2605L Haptic Driver for STM32 HAL
 * @author  Zachary Milan
 */

#include "drv2605l.h"
#include "main.h"
#include "stdio.h"

static I2C_HandleTypeDef* DRV2605L;
static DRV2605L_mode_t CurrentMode;

uint8_t DRV2605L_Read(uint8_t regAddr) {
	uint8_t data = 0x00;
	uint8_t retries = 3;
	while (retries--) { //
		if (HAL_I2C_Mem_Read(DRV2605L, DRV2605L_ADDR, regAddr, I2C_MEMADD_SIZE_8BIT, &data, 1, 10) == HAL_OK){
			return data;
		}
		HAL_Delay(1);
	}
	printf("DRV2605L Register Read Failed\n");
	return 0x00;
}

void DRV2605L_Write(uint8_t regAddr, uint8_t writeData) {
	uint8_t retries = 3;
	while (retries--) {
		if (HAL_I2C_Mem_Write(DRV2605L, DRV2605L_ADDR, regAddr, I2C_MEMADD_SIZE_8BIT, &writeData, 1, 10) == HAL_OK) {
			return;
		}
		HAL_Delay(1);
	}
	printf("DRV2605L Register Write Failed\n");
}

void DRV2605L_Init(I2C_HandleTypeDef* hi2c) {

	DRV2605L_Enable(); // enable device if not already enabled

	DRV2605L = hi2c; // set internal i2c handle

	HAL_Delay(1); // Device needs 250us before accepting I2C transactions.

	// Auto-Calibrate my specific LRA using DRV2605L Datasheet Page 29
	DRV2605L_SetMode(AUTO_CALIBRATION);

	// set specific values based on our LRA.
	DRV2605L_Write(DRV2605L_FEEDBACK_CONTROL,       DRV2605L_CAL_FEEDBACK_CTRL);
	DRV2605L_Write(DRV2605L_RATED_VOLTAGE,          DRV2605L_CAL_RATED_VOLTAGE);
	DRV2605L_Write(DRV2605L_OVERDRIVE_CLAMP_VOLT, DRV2605L_CAL_OVERDRIVE_CLAMP);
	DRV2605L_Write(DRV2605L_CONTROL4,              DRV2605L_CAL_CTRL4);
	DRV2605L_Write(DRV2605L_CONTROL1,              DRV2605L_CAL_CTRL1);
	DRV2605L_Write(DRV2605L_CONTROL2,              DRV2605L_CAL_CTRL2);
	DRV2605L_Write(DRV2605L_CONTROL5,              DRV2605L_CAL_CTRL5);
	DRV2605L_Write(DRV2605L_GO, 0x01); // set GO bit

	// poll GO bit with 5 second timeout. GO bit clearing signals auto-cal is complete.
	uint32_t calib_start = HAL_GetTick();
	while (DRV2605L_Read(DRV2605L_GO) & 0x01) {
		if (HAL_GetTick() - calib_start > 5000) {
			printf("Auto-Calibration Timeout\n");
			return;
		}
	}

	// check DIAG bit in status register to ensure successful calibration
	if ((DRV2605L_Read(DRV2605L_STATUS) >> 3) & 0x01) { // auto calibration failed.
		printf("DRV2605L Auto-Calibration Failed\n");
		return;
	}
	printf("DRV2605L Auto-Calibration Successful!\n");

	// Select LRA library for use
	DRV2605L_Write(DRV2605L_LIB_SELECT, 0x06);

	// Set mode back to internal trigger
	DRV2605L_SetMode(INTERNAL_TRIG);

}

// Puts device into active operation. Can R/W via I2C.
void DRV2605L_Enable(void) {
	HAL_GPIO_WritePin(LRA_EN_GPIO_Port, LRA_EN_Pin, GPIO_PIN_SET);
}

// DISABLED == SHUTDOWN/LOW POWER MODE. Can acknowledge I2C transactions, but not read/write.
// Device registers are not reset.
void DRV2605L_Disable(void) {
	HAL_GPIO_WritePin(LRA_EN_GPIO_Port, LRA_EN_Pin, GPIO_PIN_RESET);
}

void DRV2605L_SequenceWaveform(uint8_t waveformID, uint8_t sequenceNum) {
	if (sequenceNum > 8 || sequenceNum < 1) {
		printf("Sequence Number out of range. Returning..\n");
		return;
	}
	uint8_t waveformRegAddr = DRV2605L_WAVEFORM_SEQ_START + sequenceNum - 1;
	DRV2605L_Write(waveformRegAddr, waveformID); // queue medium click to the first waveform sequence register.
}

// This function also brings the device out of standby mode (writes 0's to upper 2 bits)
void DRV2605L_SetMode(DRV2605L_mode_t mode) {
	uint8_t modeValue;
	switch (mode) {
		case INTERNAL_TRIG:
			modeValue = 0x00;
			break;
		case EXTERNAL_TRIG_EDGE:
			modeValue = 0x01;
			break;
		case DIAGNOSTICS:
			modeValue = 0x06;
			break;
		case AUTO_CALIBRATION:
			modeValue = 0x07;
			break;
		default:
			modeValue = 0;
			break;
	}
	DRV2605L_Write(DRV2605L_MODE, modeValue);
	CurrentMode = mode;
}

void DRV2605L_TriggerHaptics(void) {
	if (CurrentMode == INTERNAL_TRIG) {
		DRV2605L_Write(DRV2605L_GO, 0x01); // set GO bit
	} else {
		HAL_GPIO_WritePin(LRA_TRIG_GPIO_Port, LRA_TRIG_Pin, GPIO_PIN_SET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(LRA_TRIG_GPIO_Port, LRA_TRIG_Pin, GPIO_PIN_RESET);
	}
}

void DRV2605L_EnterStandby(void) {
	uint8_t val = DRV2605L_Read(DRV2605L_MODE);
	val |= (1 << 6);
	DRV2605L_Write(DRV2605L_MODE, val); // set GO bit
}

void DRV2605L_ExitStandby(void) {
	uint8_t val = DRV2605L_Read(DRV2605L_MODE);
	val &= ~(1 << 6);
	DRV2605L_Write(DRV2605L_MODE, val); // set GO bit
}



