/**
 * @file    led.c
 * @brief   LED Driver for STM32 HAL
 * @author  Zachary Milan
 */


#include "led.h"

static bool blueLEDActive;
static BlueLED_State Blue_LED_State = INACTIVE;
static uint32_t blueLED_time = 0;
static uint32_t blueLED_timer = 0;
extern volatile bool toggleBlueActive; // change to get/set later?


void ToggleBlueLED(void) {
	HAL_GPIO_TogglePin(BUTTN_PRESS_LED_GPIO_Port, BUTTN_PRESS_LED_Pin);
}
void TurnOnBlueLED(void) {
	HAL_GPIO_WritePin(BUTTN_PRESS_LED_GPIO_Port, BUTTN_PRESS_LED_Pin, RESET);
}
void TurnOffBlueLED(void) {
	HAL_GPIO_WritePin(BUTTN_PRESS_LED_GPIO_Port, BUTTN_PRESS_LED_Pin, SET);
}

void ToggleBlueLEDTwice(void) {

	switch (Blue_LED_State) {

	case INACTIVE:
			blueLED_time = HAL_GetTick();
			TurnOnBlueLED();
			Blue_LED_State = ON_1;
		break;

	case ON_1:
		if (HAL_GetTick() - blueLED_time > 50) {
			TurnOffBlueLED();
			Blue_LED_State = OFF;
		}
		break;

	case OFF:
		if (HAL_GetTick() - blueLED_time > 100) {
			TurnOnBlueLED();
			Blue_LED_State = ON_2;
		}
		break;

	case ON_2:
		if (HAL_GetTick() - blueLED_time > 150) {
			TurnOffBlueLED();
			Blue_LED_State = INACTIVE;
			toggleBlueActive = false;
		}
		break;
	}
} // ToggleBlueLEDTwice


void Update_Blue_LED_Pulse(void) {
	if (blueLEDActive) {
		if (HAL_GetTick() - blueLED_timer > 50) {
			TurnOffBlueLED();
            blueLEDActive = false;
		}
    }
}

void Trigger_Blue_Pulse(void) {
    blueLED_timer = HAL_GetTick();
    TurnOnBlueLED();
    blueLEDActive = true;
}

void TurnOnRedLED(void) {
	HAL_GPIO_WritePin(LOW_BATT_LED_GPIO_Port, LOW_BATT_LED_Pin, 0);
}

void TurnOffRedLED(void) {
	HAL_GPIO_WritePin(LOW_BATT_LED_GPIO_Port, LOW_BATT_LED_Pin, 1);
}



void CheckLowBatt(ADC_HandleTypeDef *hadc)
{
    uint32_t adcSum = 0;
    uint32_t adcRaw = 0;
    float vADC, vBat;

    // throw away the first sample
    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);
    (void)HAL_ADC_GetValue(hadc);
    HAL_ADC_Stop(hadc);

    for (int i = 0; i < 3; i++) {
        HAL_ADC_Start(hadc);
        HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);
        adcSum += HAL_ADC_GetValue(hadc);
        HAL_ADC_Stop(hadc);
    }

    adcRaw = adcSum / 3; // average out readings

    vADC = 3.3f * ((float)adcRaw / 4095.0f); // convert to voltage vs reference
    vBat = vADC * ((270.0f + 1000.0f) / 1000.0f); // convert to battery using reverse voltage divider

    if (vBat <= 3.4f) {
        TurnOnRedLED();
    } else {
        TurnOffRedLED();
    }
}

void TurnOffAllLED(void) {
	TurnOffRedLED();
	TurnOffBlueLED();
}


