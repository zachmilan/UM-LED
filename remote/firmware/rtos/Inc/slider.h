/**
 * @file    slider.c
 * @brief   Touch Processing for STM32 HAL
 * @author  Zachary Milan
 */

#ifndef INC_SLIDER_H_
#define INC_SLIDER_H_

#include "tsl_user.h"
#include "drv2605l.h"
#include "math.h"
#include "main.h"
#include "stdlib.h"
#include "stdio.h"
#include "cmsis_os.h"
#include "led.h"


#define EMA_ALPHA 0.35f
#define TOUCH_THRESHOLD 800
#define HAPTIC_STEPS 20
#define HAPTIC_SENSITIVITY (65536.0f / HAPTIC_STEPS)


void ProcessTouch(void);
void printDeltas(void);

#endif /* INC_SLIDER_H_ */
