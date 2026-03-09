/**
 * @file    bm71.c
 * @brief   BM71 Bluetooh Driver for STM32 HAL
 * @author  Zachary Milan
 */

#include "BM71.h"
#include "main.h"
#include "stdio.h"

static UART_HandleTypeDef* BM71;
static uint8_t TxBuf[256]; // buffer we send over uart to BM71
static uint8_t RxBuf[256]; // buffer we send over uart to BM71
static uint16_t TxBufSize; // size of buffer we pass into rawTransmit
static uint16_t RxBufSize;
const uint8_t DISPLAY_MAC_ADDR[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //
static bool RxReady = false;
static bool connectionPending = false;
static uint8_t connectionHandle;

void BM71_Init(UART_HandleTypeDef *huart) {
	BM71 = huart;
	HAL_UARTEx_ReceiveToIdle_IT(BM71, RxBuf, 256); // initially sets up UART line for reception.
}

UART_HandleTypeDef* BM71_GetHandle(void) {
	return BM71;
}

// transmit function
void BM71_Transmit(uint8_t *data, uint16_t len) {
	uint8_t retries = 3;
	while (retries--) {
		HAL_StatusTypeDef status = HAL_UART_Transmit(BM71, data, len, 1000);
		if (status == HAL_OK) {
			return;
		}
		HAL_Delay(100);
	}
//	printf("BM71 UART Transmit Failed\n"); // avoid printf in RTOS
}

// takes opcode and parameter data, calculate length and checksum. pack into buf.
// user passes in NULL to params if no params exist for that command
void BM71_PackMessage(uint8_t opcode, uint8_t* params, uint16_t paramLen) {
	uint16_t length = paramLen + 1; // accounts for opcode
	TxBuf[0] = 0xAA;
	TxBuf[1] = length >> 8;
	TxBuf[2] = length;
	TxBuf[3] = opcode;
	for (uint16_t i = 0; i < paramLen; ++i) {
		TxBuf[i+4] = params[i];
	}
	uint8_t checksum = 0;
	for (int i = 1; i < paramLen + 4; ++i) {
		checksum += TxBuf[i];
	}
	checksum = -(checksum);
	TxBuf[paramLen + 4] = checksum;
	TxBufSize = paramLen + 5;
}

void BM71_Sleep(void) {
	HAL_GPIO_WritePin(UART_RX_IND_GPIO_Port, UART_RX_IND_Pin, 1);
}

void BM71_Wake(void) {
	HAL_GPIO_WritePin(UART_RX_IND_GPIO_Port, UART_RX_IND_Pin, 0);
	HAL_Delay(4); // spec says must wait > 3ms before sending uart data.
}
//
void BM71_ReadLocalInfo(void) {
	// will require receiving data too!!
	BM71_PackMessage(BM71_READ_LOCAL_INFO, NULL, 0);
	BM71_Transmit(TxBuf, TxBufSize);
	return;
	// need to parse a response here.. if you want.
}

void BM71_SetRxSize(uint16_t size) {
	RxBufSize = size;
}

void BM71_ConnectToDisplay(void) {
	uint8_t paramLength = 8;
	uint8_t params[paramLength];
	params[0] = 0x00; // reserved; write as 0x00
	params[1] = 0x00; // 0x00 for public address (0x01 for private)
	for (uint8_t i = 0; i < 6; ++i) {
		params[i+2] = DISPLAY_MAC_ADDR[i];
	}
	BM71_PackMessage(BM71_CREATE_CONNECTION, params, paramLength);
	BM71_Transmit(TxBuf, TxBufSize);
	connectionPending = true;
}

void BM71_CommandDisplay(display_op_t opType, uint16_t opValue) {
	uint8_t paramLen = 4;
	uint8_t params[paramLen];

	// format: 0xAA, Len1, len0, opcode, connection_handle, optype, opvalue_msb, opvalue_lsb, checksum
	params[0] = connectionHandle; // Need to parse LE CONNECTION COMPLETE EVENT (OPCODE 0x71).
	params[1] = opType;
	params[2] = opValue >> 8;
	params[3] = opValue;
	BM71_PackMessage(BM71_TRANS_UART_SEND, params, paramLen);
	BM71_Transmit(TxBuf, TxBufSize);
}

uint8_t* BM71_GetRxBuf(void) {
	return RxBuf;
}

void BM71_SetRxReady(void) {
	RxReady = true;
}

bool BM71_GetRxReady(void) {
	return RxReady;
}

bool BM71_ConnectionPending(void) {
	return connectionPending;
}

void BM71_ParseResponse(void) {

	// future tweak - add checksum check here.

	// not sure if opcode is at index 3 for all cases due to datasheet errors
	uint16_t paramLength = ((uint16_t)RxBuf[1] << 8) + RxBuf[2];
	uint16_t paramStart = 4;
	uint8_t opcode = RxBuf[3]; // due to datasheet inconsistencies, may not be true for all cases

	switch (opcode) {
		case 0x80: // Regular Command Complete Synchronous Response.
			uint8_t status = RxBuf[5];
			if (status != COMMAND_SUCCESS) { // command failed if rxbuf[5] (status code) is not success
//				printf("COMMAND FAILED. ERROR CODE: %u\n", RxBuf[5]); // avoid printf in RTOS
			} else {
				for (int i = 6; i < RxBufSize - 1; ++i) {
//					printf("%c", (char)RxBuf[i]); // avoid printf in RTOS
				}
//				printf("\n"); // avoid printf in RTOS
			}
			break;

		case BM71_CONNECTION_COMPLETE_EVT:
			uint8_t successCode = RxBuf[4];
			if (successCode == 0) {
				connectionHandle = RxBuf[5];
			}
			connectionPending = false; // connection pending no longer since we either succeeded or failed.
			break;

		default:
			break;
	}
	RxReady = false;
}

void BM71_ReadDeviceName(void) {
	BM71_PackMessage(BM71_READ_DEVICE_NAME, NULL, 0);
	BM71_Transmit(TxBuf, TxBufSize);
	return;

}

BM71_status BM71_GetStatus(void) {
	bool statusPin1 = (bool)HAL_GPIO_ReadPin(BLE_STATUS1_IND_GPIO_Port, BLE_STATUS1_IND_Pin);
	bool statusPin2 = (bool)HAL_GPIO_ReadPin(BLE_STATUS2_IND_GPIO_Port, BLE_STATUS2_IND_Pin);
	if (statusPin1 && statusPin2) {
		return SHUTDOWN;
	} else if (statusPin1 && !statusPin2) {
		return STANDBY;
	} else if (!statusPin1 && !statusPin2) {
		return BLE_CONNECTED;
	} else {
		return DATA_SESSION_OPEN;
	}
}

void BM71_SetMode_Configure(void) {
	HAL_GPIO_WritePin(BLE_MODE_GPIO_Port, BLE_MODE_Pin, 0);
	BM71_Reset();
}

void BM71_SetMode_Run(void) {
	HAL_GPIO_WritePin(BLE_MODE_GPIO_Port, BLE_MODE_Pin, 1);
	BM71_Reset();
}

void BM71_Disconnect(void) {
	BM71_PackMessage(BM71_DISCONNECT, NULL, 0);
	BM71_Transmit(TxBuf, TxBufSize);
}

void BM71_Reset(void) {
	HAL_GPIO_WritePin(BLE_RST_N_GPIO_Port, BLE_RST_N_Pin, 0);
	HAL_Delay(100);
	HAL_GPIO_WritePin(BLE_RST_N_GPIO_Port, BLE_RST_N_Pin, 1);
	HAL_Delay(500);
}



// -------- experimental advertising code snippet that seemed to work --------

// Command 1: Set Advertising Parameters (0x13)
//  uint8_t adv_params[18];
//  adv_params[0] = 0xAA;
//  adv_params[1] = 0x00;
//  adv_params[2] = 0x0C;           // 12 bytes of parameters
//  adv_params[3] = 0x13;           // OP Code
//  adv_params[4] = 0x01;           // Interval MSB (0x0100 = 160ms)
//  adv_params[5] = 0x00;           // Interval LSB
//  adv_params[6] = 0x00;           // Type: Connectable undirected advertising
//  adv_params[7] = 0x00;           // Own address type: Public
//  adv_params[8] = 0x00;           // Peer address (not used for undirected)
//  adv_params[9] = 0x00;
//  adv_params[10] = 0x00;
//  adv_params[11] = 0x00;
//  adv_params[12] = 0x00;
//  adv_params[13] = 0x00;
//  adv_params[14] = 0x07;          // All 3 advertising channels
//  adv_params[15] = 0x00;          // Filter policy: allow all
//
//  uint8_t sum1 = 0;
//  for(int i = 1; i < 16; i++) sum1 += adv_params[i];
//  adv_params[16] = -sum1;
//
//  HAL_UART_Transmit(&huart5, adv_params, 17, HAL_MAX_DELAY);
//  HAL_Delay(200);
//
//  // Command 2: Enable Advertising (0x1C)
//  uint8_t adv_enable[6];
//  adv_enable[0] = 0xAA;
//  adv_enable[1] = 0x00;
//  adv_enable[2] = 0x02;
//  adv_enable[3] = 0x1C;
//  adv_enable[4] = 0x01;           // Enable advertising
//  adv_enable[5] = -(adv_enable[1] + adv_enable[2] + adv_enable[3] + adv_enable[4]);
//
//  HAL_UART_Transmit(&huart5, adv_enable, 6, HAL_MAX_DELAY);

















