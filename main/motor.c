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
                    ledc_channel_t channel, ledc_mode_t mode)
{
    motor_t* m = malloc(sizeof(motor_t));
    
    m->gpio_a = gpio_a;
    m->gpio_b = gpio_b;
    m->channel = channel;
    m->mode = mode;

    gpio_set_direction(m->gpio_a, GPIO_MODE_OUTPUT);
    gpio_set_direction(m->gpio_b, GPIO_MODE_OUTPUT);

    gpio_set_level(m->gpio_a, 0);
    gpio_set_level(m->gpio_b, 0);

    ledc_channel_config_t channel_config = {
        .gpio_num = m->gpio_a,
        .speed_mode = m->mode,
        .channel = m->channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = (m->mode == LEDC_LOW_SPEED_MODE) ? LEDC_TIMER_0 : LEDC_TIMER_1,
        .duty = 256,
        .hpoint = 0
    };
    ledc_channel_config(&channel_config);

    m->channel_config = channel_config;

    return m;
}

/**
 * Set the speed of the motor. Speed is 8 bit, between -255 and +255, with 0 being stopped.
 */
void motor_set(motor_t *motor, int speed)
{

    // Whenever the motor changes direction, switch the PWM pin and steady-state pin
    if(speed > 0 && motor->channel_config.gpio_num == motor->gpio_b)
    {
        motor->channel_config.gpio_num = motor->gpio_a;
        ledc_channel_config(&(motor->channel_config));
        gpio_set_direction(motor->gpio_b, GPIO_MODE_OUTPUT);
        
    } else if(speed < 0 && motor->channel_config.gpio_num == motor->gpio_a)
    {
        motor->channel_config.gpio_num = motor->gpio_b;
        ledc_channel_config(&(motor->channel_config));
        gpio_set_direction(motor->gpio_a, GPIO_MODE_OUTPUT);
        
    }
        
    // Motor speed is inverse to duty cycle, for some reason.
    // 0 = full speed, 256 = stopped
    ledc_set_duty(motor->mode, motor->channel, 256 - abs(speed));
    gpio_set_level((speed > 0) ? motor->gpio_b : motor->gpio_a, 1);

    ledc_update_duty(motor->mode, motor->channel);
}