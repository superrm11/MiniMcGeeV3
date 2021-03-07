#ifndef _ONBOARD_LED_
#define _ONBOARD_LED_

#include "stdint.h"

void onboard_led_init();

void onboard_led_setrgb(uint32_t rgb_hex);

void onboard_led_disable();

void onboard_led_enable();

#endif