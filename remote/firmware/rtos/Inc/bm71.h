/**
 * @file    bm71.h
 * @brief   BM71 Bluetooh Driver for STM32 HAL
 * @author  Zachary Milan
 */

#ifndef INC_BM71_H_
#define INC_BM71_H_

#include "stm32f3xx_hal.h"
#include "stdbool.h"

// BM71 Command Opcodes
#define BM71_READ_LOCAL_INFO        0x01
#define BM71_RESET                  0x02
#define BM71_READ_STATUS            0x03
#define BM71_READ_ADC               0x04
#define BM71_SHUTDOWN               0x05
#define BM71_READ_DEVICE_NAME       0x07
#define BM71_WRITE_DEVICE_NAME      0x08
#define BM71_SET_ADV_PARAMS         0x13
#define BM71_ADV_ENABLE             0x1C
#define BM71_SET_SCAN_PARAMS        0x15
#define BM71_SET_SCAN_ENABLE        0x16
#define BM71_CREATE_CONNECTION      0x17
#define BM71_DISCONNECT             0x1B
#define BM71_TRANS_UART_ENABLE      0x35
#define BM71_TRANS_UART_SEND        0x3F

// BM71 Event Opcodes
#define BM71_CONNECTION_COMPLETE_EVT 0x71
#define COMMAND_SUCCESS 0x00

// display commands
#define CMD_DISPLAY_OFF 0x00
#define CMD_DISPLAY_ON 0x01
#define CMD_DISPLAY_AUTODIM 0x02
#define CMD_DISPLAY_NEXT_MODE 0x03
#define CMD_DISPLAY_PREV_MODE 0x04

// Type of command display will receive
typedef enum {
	COMMAND,
	BRIGHTNESS
} display_op_t;

// Four modes the BM71 can be in (based on status pins)
typedef enum {
	SHUTDOWN,
	STANDBY,
	BLE_CONNECTED,
	DATA_SESSION_OPEN
} BM71_status;

// Init function
void BM71_Init(UART_HandleTypeDef *huart);

// User-Level Commands
void BM71_ReadLocalInfo(void);
void BM71_ConnectToDisplay(void);
void BM71_CommandDisplay(display_op_t opType, uint16_t opValue);
void BM71_ReadDeviceName(void);
void BM71_Disconnect(void);
void BM71_Reset(void);
void BM71_Sleep(void);
void BM71_Wake(void);

// Getters and setters
UART_HandleTypeDef* BM71_GetHandle(void);
uint8_t* BM71_GetRxBuf(void);
void BM71_SetRxReady(void);
bool BM71_GetRxReady(void);
bool BM71_ConnectionPending(void);
void BM71_SetRxSize(uint16_t size);
BM71_status BM71_GetStatus(void);


// Lower Level Commands
void BM71_SetMode_Configure(void);
void BM71_SetMode_Run(void);
void BM71_Transmit(uint8_t *data, uint16_t len);
void BM71_PackMessage(uint8_t opcode, uint8_t* params, uint16_t paramSize);
void BM71_ParseResponse(void);

#endif /* INC_BM71_H_ */
