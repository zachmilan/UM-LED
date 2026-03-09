/**
 * @file    drv2605l.h
 * @brief   DRV2605L Haptic Driver for STM32 HAL
 * @author  Zachary Milan
 */

#ifndef INC_DRV2605L_H_
#define INC_DRV2605L_H_

#include "stm32f3xx_hal.h"

// I2C Address of DRV2605L, per datasheet
#define DRV2605L_ADDR (0x5A << 1)

// DRV Internal Register Addresses
#define DRV2605L_STATUS               0x00
#define DRV2605L_MODE                 0x01
#define DRV2605L_LIB_SELECT           0x03
#define DRV2605L_WAVEFORM_SEQ_START   0x04
#define DRV2605L_GO                   0x0C
#define DRV2605L_RATED_VOLTAGE        0x16
#define DRV2605L_OVERDRIVE_CLAMP_VOLT 0x17
#define DRV2605L_FEEDBACK_CONTROL     0x1A
#define DRV2605L_CONTROL1             0x1B
#define DRV2605L_CONTROL2             0x1C
#define DRV2605L_CONTROL4             0x1E
#define DRV2605L_CONTROL5             0x1F

// LRA calibration values, calculated via page 29 of DRV2605L datasheet
#define DRV2605L_CAL_FEEDBACK_CTRL    0xAA
#define DRV2605L_CAL_RATED_VOLTAGE    0x46
#define DRV2605L_CAL_OVERDRIVE_CLAMP  0x60
#define DRV2605L_CAL_CTRL1            0x90
#define DRV2605L_CAL_CTRL2            0xF5
#define DRV2605L_CAL_CTRL4            0x30
#define DRV2605L_CAL_CTRL5            0x80

// DRV2605L Modes (there are 7, but only use 3-4 for our use case)
typedef enum {
	INTERNAL_TRIG,
	EXTERNAL_TRIG_EDGE,
	DIAGNOSTICS,
	AUTO_CALIBRATION
} DRV2605L_mode_t;

// Initialization and Calibration
void DRV2605L_Init(I2C_HandleTypeDef* hi2c);

// Enable Control
void DRV2605L_Enable(void);
void DRV2605L_Disable(void);

// Standby Control (Low Power, Fast Turn On)
void DRV2605L_EnterStandby(void);
void DRV2605L_ExitStandby(void);

// Register Access
uint8_t DRV2605L_Read(uint8_t regAddr);
void DRV2605L_Write(uint8_t regAddr, uint8_t writeData);

// Haptic Control
void DRV2605L_SequenceWaveform(uint8_t waveformIndex, uint8_t sequence);
void DRV2605L_TriggerHaptics(void);

// Mode control
void DRV2605L_SetMode(DRV2605L_mode_t mode);



#endif /* INC_DRV2605L_H_ */
