/**
 * @file    slider.c
 * @brief   Touch Processing for STM32 HAL
 * @author  Zachary Milan
 */

#include "slider.h"
#include "bm71.h"

static int8_t current_haptic_step = -1;
static int8_t last_haptic_step = -1;
static float smoothed_pos = 0;


/* ProcessTouch is needed since the STM32 cannot interpolate position correctly
 * with my particular electrode setup
 *
 * This algorithm is a rough draft and will likely need some tweaks.
 */
void ProcessTouch(void) {

	// grab each delta value from the slider. these are the raw values we work with
	float d0 = (float)MyLinRots[0].p_ChD[0].Delta;
	float d1 = (float)MyLinRots[0].p_ChD[1].Delta;
	float d2 = (float)MyLinRots[0].p_ChD[2].Delta;
	float d3 = (float)MyLinRots[0].p_ChD[3].Delta;
	float d4 = (float)MyLinRots[0].p_ChD[4].Delta;

	float delta_sum = d0 + d1 + d2 + d3 + d4;

	if (delta_sum < TOUCH_THRESHOLD) { // no touch detected. reset haptics.
		last_haptic_step = -1;
	}

	else { // touch detected, use deltas to find position on slider

		// normalize each delta to around 4500 when touched
		d0 *= 1.0f;
		d1 *= 1.05f;
		d2 *= 1.6f;
		d3 *= 1.8f;
		d4 *= 3.0f;

		// use deltas as 'weights' to determine a position based on pwm value
		float weighted_sum = d0*65535.0f + d1*49151.0f + d2*32767.0f + d3*16384.0f + d4*100.0f;
		float raw_pos = weighted_sum / delta_sum;

		// this touch is a new touch. init smoothed_pos to raw_pos. trigger led/haptics.
		if (last_haptic_step == -1) {
			smoothed_pos = raw_pos;
			last_haptic_step = (uint8_t)(smoothed_pos / HAPTIC_SENSITIVITY);
			Trigger_Blue_Pulse();
			DRV2605L_TriggerHaptics();
		}

		else { // continuous touch detected. integrate raw_pos into smooth pos. trigger haptic if smoothed_pos is new zone

			smoothed_pos = (raw_pos * EMA_ALPHA) + (1.0f - EMA_ALPHA)*smoothed_pos;
			current_haptic_step = (uint8_t)(smoothed_pos / HAPTIC_SENSITIVITY);

			if (current_haptic_step != last_haptic_step) {
				Trigger_Blue_Pulse();
				DRV2605L_TriggerHaptics();
				last_haptic_step = current_haptic_step;
			}
		}
		BM71_CommandDisplay(BRIGHTNESS, (uint16_t)smoothed_pos);
	}

	Update_Blue_LED_Pulse();

}

// print raw deltas for our linear tsc bank
void printDeltas(void) {
	for (int i = MyLinRots[0].NbChannels - 1; i >= 0; --i) {
		int16_t delta = MyLinRots[0].p_ChD[i].Delta;
		printf("[%d]:%d ", i, delta);
	}
}
