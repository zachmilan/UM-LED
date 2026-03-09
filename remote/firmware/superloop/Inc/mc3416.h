/**
 * @file    mc3416.h
 * @brief   MC3416 Accelerometer Driver for STM32 HAL
 * @author  Zachary Milan
 */

#ifndef INC_MC3416_H_
#define INC_MC3416_H_

#include "stm32f3xx_hal.h"
#include <stdint.h>
#include "main.h"
#include "stdio.h"

// MC3416 I2C Address -- VPP tied to ground on PCB therefore I2C address is 0x4C (datasheet)
#define MC3416_ADDR (0x4C << 1)

// MC3416 Registers
#define MC3416_DEV_STAT 0x05 // READ ONLY
#define MC3416_INTR_CTRL 0x06
#define MC3416_MODE 0x07
#define MC3416_SR 0x08
#define MC3416_MOTION_CTRL 0x09
#define MC3416_STATUS_2 0x13
#define MC3416_INTR_STAT_2 0x14
#define MC3416_CHIP_ID 0x18 // READ ONLY
#define MC3416_RANGE 0x20
#define MC3416_TF_THRESH_LSB 0x40
#define MC3416_TF_THRESH_MSB 0x41
#define MC3416_TF_DB 0x42
#define MC3416_AM_THRESH_LSB 0x43
#define MC3416_AM_THRESH_MSB 0x44
#define MC3416_AM_DB 0x45
#define MC3416_SHK_THRESH_LSB 0x46
#define MC3416_SHK_THRESH_MSH 0x47
#define MC3416_P2P_DUR_THRESH_LSB 0x48
#define MC3416_P2P_DUR_THRESH_MSB 0x49
#define MC3416_TIMER_CTRL 0x4A
#define MC3416_XOUT_LSB        0x0D
#define MC3416_XOUT_MSB        0x0E
#define MC3416_YOUT_LSB        0x0F
#define MC3416_YOUT_MSB        0x10
#define MC3416_ZOUT_LSB        0x11
#define MC3416_ZOUT_MSB        0x12

// Sample Rates 128HZ to 1024HZ
typedef enum {
	MC3416_SR_128HZ = 0x00,
	MC3416_SR_256HZ = 0x01,
	MC3416_SR_512HZ = 0x02,
	MC3416_SR_1024HZ = 0x05,

} MC3416_SampleRate_t;

// Acceleration Ranges 0-16G
typedef enum {
	MC3416_RANGE_2G = 0x00,
	MC3416_RANGE_4G = 0x01,
	MC3416_RANGE_8G = 0x02,
	MC3416_RANGE_16G = 0x03,
	MC3416_RANGE_12G = 0x04,
} MC3416_Range_t;

// Init (pass off I2C handle, Initialize settings for our use)
void MC3416_Init(I2C_HandleTypeDef *hi2c);

// Register Read/Write
uint8_t MC3416_ReadReg(uint8_t reg);
void MC3416_WriteReg(uint8_t reg, uint8_t data);
uint8_t MC3416_ReadInterruptReg(void);\
void MC3416_ReadXYZ(int16_t *x, int16_t *y, int16_t *z);

// Interrupt Settings
void MC3416_SetInterruptPushPull(void);
void MC3416_SetInterruptActiveHighLow(uint8_t HL);
void MC3416_EnableInterruptAutoClear(void);

//  AnyMotion Functionality
void MC3416_EnableAnyMotionFeature(void);
void MC3416_DisableAnyMotionFeature(void);
void MC3416_EnableAnyMotionInterrupt(void);
void MC3416_DisableAnyMotionInterrupt(void);

// Power/Mode settings
void MC3416_Standby(void);
void MC3416_Wake(void);

// Specific Settings
void MC3416_SetSampleRate(MC3416_SampleRate_t rate);
void MC3416_SetRange(MC3416_Range_t range);
void MC3416_SetAnyMotionDebounce(uint8_t debounce);
void MC3416_SetAnyMotionThreshold(uint16_t threshold);







#endif /* INC_MC3416_H_ */
