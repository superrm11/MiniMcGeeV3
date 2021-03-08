#include "motor.h"

/**
 * Set up the motor module
 */
void motor_install()
{
    // Configure the "low speed" timer
    ledc_timer_config_t timer_config_0 = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 500,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_config_0);

    #ifdef SOC_LEDC_SUPPORT_HS_MODE
    // Configure the "high speed" timer if it's supported by the SOC
    ledc_timer_config_t timer_config_1 = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_1,
        .freq_hz = 500,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_config_1);
    #endif
}

/**
 * Initialize a single motor with pin A and pin B.
 * There are 8 channels per mode available.
 * 
 * LEDC_LOW_SPEED_MODE is universal on the esp32 series,
 * LEDC_HIGH_SPEED_MODE is only available on the ESP32-WROOM/ESP32-WROVER series.
 */
motor_t* motor_init(gpio_num_t gpio_a, gpio_num_t gpio_b, 
                    ledc_channel_t channel_a, ledc_channel_t channel_b, 
                    ledc_mode_t mode_a, ledc_mode_t mode_b)
{
    motor_t* m = malloc(sizeof(motor_t));
    
    m->gpio_a = gpio_a;
    m->gpio_b = gpio_b;
    m->channel_a = channel_a;
    m->channel_b = channel_b;
    m->mode_a = mode_a;
    m->mode_b = mode_b;

    ledc_channel_config_t channel_a_config = {
        .gpio_num = m->gpio_a,
        .speed_mode = m->mode_a,
        .channel = m->channel_a,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = (m->mode_a == LEDC_LOW_SPEED_MODE) ? LEDC_TIMER_0 : LEDC_TIMER_1,
        .duty = 256,
        .hpoint = 0
    };
    ledc_channel_config(&channel_a_config);

    ledc_channel_config_t channel_b_config = {
        .gpio_num = m->gpio_b,
        .speed_mode = m->mode_b,
        .channel = m->channel_b,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = (m->mode_b == LEDC_LOW_SPEED_MODE) ? LEDC_TIMER_0 : LEDC_TIMER_1,
        .duty = 256,
        .hpoint = 0
    };
    ledc_channel_config(&channel_b_config);

    return m;
}

/**
 * Set the speed of the motor. Speed is 8 bit, between -255 and +255, with 0 being stopped.
 */
void motor_set(motor_t *motor, int speed)
{
    int speed_a = (speed > 0) ? speed : 0;
    int speed_b = (speed < 0) ? -speed : 0;

    // Motor speed is inverse to duty cycle, for some reason.
    // 0 = full speed, 256 = stopped
    speed_a = 256 - speed_a;
    speed_b = 256 - speed_b;

    ledc_set_duty(motor->mode_a, motor->channel_a, speed_a);
    ledc_set_duty(motor->mode_b, motor->channel_b, speed_b);
    ledc_update_duty(motor->mode_a, motor->channel_a);
    ledc_update_duty(motor->mode_b, motor->channel_b);
}