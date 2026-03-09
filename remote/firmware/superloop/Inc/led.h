/**
 * @file    led.h
 * @brief   LED Driver for STM32 HAL
 * @author  Zachary Milan
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "main.h"
#include <stdbool.h>

// LED TypeDef
typedef enum {
	INACTIVE,
	ON_1,
	OFF,
	ON_2
} BlueLED_State;


// LED Function Declaration
void CheckLowBatt(ADC_HandleTypeDef *hadc);
void ToggleBlueLED(void);
void TurnOnBlueLED(void);
void TurnOffBlueLED(void);
void ToggleBlueLEDTwice(void);
void Update_Blue_LED_Pulse(void);
void Trigger_Blue_Pulse(void);
void ToggleBlueLEDOnce(void);
void TurnOnRedLED(void);
void TurnOffRedLED(void);
void TurnOffAllLED(void);


#endif /* INC_LED_H_ */
