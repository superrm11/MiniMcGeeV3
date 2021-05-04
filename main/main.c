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
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "mdns.h"
#include "lwip/sockets.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/i2c.h"

void send_i2c(uint8_t device, uint8_t register_num, uint8_t* data, size_t data_size)
{
    i2c_cmd_handle_t command = i2c_cmd_link_create();
    i2c_master_start(command);
    i2c_master_write_byte(command, (device << 1) | I2C_MASTER_WRITE, true); // Device address
    i2c_master_write_byte(command, register_num, false);
    i2c_master_write(command, data, data_size, false); // payload
    i2c_master_stop(command);

    i2c_master_cmd_begin(I2C_NUM_0, command, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(command);
}

uint8_t recv_i2c(uint8_t device, uint8_t register_num)
{
    uint8_t data;

    i2c_cmd_handle_t command = i2c_cmd_link_create();
    i2c_master_start(command);
    i2c_master_write_byte(command, (device << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(command, register_num, true);
    i2c_master_stop(command);
    esp_err_t e = i2c_master_cmd_begin(I2C_NUM_0, command, 1000 / portTICK_RATE_MS);
    if(e != ESP_OK)
    {
        printf("Error code: %d\n", e);
    }
    i2c_cmd_link_delete(command);

    i2c_cmd_handle_t cmd2 = i2c_cmd_link_create();
    i2c_master_start(cmd2);
    i2c_master_write_byte(cmd2, (device << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd2, &data, true);
    i2c_master_stop(cmd2);

    e = i2c_master_cmd_begin(I2C_NUM_0, cmd2, 1000 / portTICK_RATE_MS);
    if(e != ESP_OK)
    {
        printf("Error code: %d\n", e);
    }

    i2c_cmd_link_delete(cmd2);

    return data;
}

void app_main(void)
{
    // onboard_led_init();
    // onboard_led_setrgb(0x000000);

    //Initialize NVS
    // nvs_flash_init();


    adc1_config_width(ADC_WIDTH_BIT_10);
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11);

    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 15,
        .scl_io_num = 0,
        .sda_pullup_en = false,
        .scl_pullup_en = false,
        .master.clk_speed = 100000
    };
    if(i2c_param_config(I2C_NUM_0, &i2c_conf) != ESP_OK)
        printf("Config Error\n");
    if(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0) != ESP_OK)
        printf("Driver Install Error\n");

    uint8_t data = 0b00000000;
    send_i2c(0x68, 0x1B, &data, 1); // Config the gyro to be +/- 250 dps
    // printf("received: %x\n", recv_i2c(0x68, 0x75));
    


    motor_install();
    motor_t *lf_dir = motor_init(33, 25, LEDC_CHANNEL_0, LEDC_LOW_SPEED_MODE);
    motor_t *lf_drive = motor_init(26, 27, LEDC_CHANNEL_1, LEDC_LOW_SPEED_MODE);

    motor_t *lr_dir = motor_init(12, 14, LEDC_CHANNEL_2, LEDC_LOW_SPEED_MODE);
    motor_t *lr_drive = motor_init(4, 13, LEDC_CHANNEL_3, LEDC_LOW_SPEED_MODE);

    motor_t *rf_dir = motor_init(23, 22, LEDC_CHANNEL_4, LEDC_LOW_SPEED_MODE);
     motor_t *rf_drive = motor_init(16, 17, LEDC_CHANNEL_5, LEDC_LOW_SPEED_MODE);

    motor_t *rr_dir = motor_init(18, 5, LEDC_CHANNEL_6, LEDC_LOW_SPEED_MODE);  
    motor_t *rr_drive = motor_init(21, 19, LEDC_CHANNEL_7, LEDC_LOW_SPEED_MODE);



    while(true)
    {
    
        // int motorval = (adc1_get_raw(ADC1_CHANNEL_4) - 512) / 2.0;

        // printf("ADC1: %d\n", motorval);

        // motor_set(rr_dir, motorval);

        uint8_t hz = recv_i2c(0x68, 0x47);
        uint8_t lz = recv_i2c(0x68, 0x48);

        int16_t z = (hz << 8) | lz;


        printf("Z axis: %d\n", z);
        


        usleep(1000 * 10);
    }

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
