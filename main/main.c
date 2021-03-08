/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <unistd.h>
#include "esp_system.h"
#include "onboard_led.h"
#include "motor.h"

void app_main(void)
{
    onboard_led_init();
    onboard_led_setrgb(0x000000);

    gpio_set_direction(11, GPIO_MODE_INPUT);
    gpio_set_pull_mode(11, GPIO_PULLUP_ONLY);

    motor_install();
    motor_t *m = motor_init(1, 2, LEDC_CHANNEL_0, LEDC_CHANNEL_1, LEDC_LOW_SPEED_MODE, LEDC_LOW_SPEED_MODE);
    motor_t *m = motor_init(3, 4, LEDC_CHANNEL_2, LEDC_CHANNEL_3, LEDC_LOW_SPEED_MODE, LEDC_LOW_SPEED_MODE);

    while(true)
    {
        
        usleep(1000 * 10);
    }

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
