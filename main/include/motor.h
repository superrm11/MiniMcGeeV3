#ifndef _MOTOR_
#define _MOTOR_

#include "driver/ledc.h"

/**
 * Designed for a 2-pin H-bridge setup, using 2 PWM signals
 */
typedef struct 
{
    gpio_num_t gpio_a;
    gpio_num_t gpio_b;
    ledc_channel_t channel;
    ledc_mode_t mode;
    ledc_channel_config_t channel_config;
} motor_t;

/**
 * Setup the PWM timers before creating / using any motors.
 */
void motor_install();

/**
 * Initialize a single motor with pin A and pin B.
 * There are 8 channels per mode available.
 * 
 * LEDC_LOW_SPEED_MODE is universal on the esp32 series,
 * LEDC_HIGH_SPEED_MODE is only available on the ESP32-WROOM/ESP32-WROVER series.
 */
motor_t* motor_init(gpio_num_t gpio_a, gpio_num_t gpio_b, 
                    ledc_channel_t channel, ledc_mode_t mode);

/**
 * Set the speed of the motor. Speed is 8 bit, between -255 and +255, with 0 being stopped.
 */
void motor_set(motor_t *motor, int speed);

#endif