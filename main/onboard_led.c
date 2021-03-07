#include "onboard_led.h"
#include "led_strip.h"

led_strip_t *onboard_led;
uint32_t last_stored_color = 0;

/**
 * Initialize the onboard LED for future use
 */
void onboard_led_init()
{
    led_strip_install();

    onboard_led = malloc(sizeof(led_strip_t));
    
    onboard_led->type = LED_STRIP_WS2812;
    onboard_led->is_rgbw = false;
    onboard_led->length = 1;
    onboard_led->gpio = 18;
    onboard_led->channel = RMT_CHANNEL_0;

    led_strip_init(onboard_led);
    
}

/**
 * Set the integrated RGBLED in hex format:
 * onboard_led_setrgb(0xRRGGBB)
 */
void onboard_led_setrgb(uint32_t rgb_hex)
{
    last_stored_color = rgb_hex;

    uint8_t red = (rgb_hex & 0xFF0000)>>(8*2);
    uint8_t green = (rgb_hex & 0x00FF00)>>(8*1);
    uint8_t blue = (rgb_hex & 0x0000FF);

    rgb_t color = {
        .r = red,
        .g = green,
        .b = blue
    };

    if(!led_strip_busy(onboard_led))
    {
        led_strip_set_pixel(onboard_led, 0, color);
        led_strip_flush(onboard_led);
    }
}

void onboard_led_disable()
{
    onboard_led_setrgb(0);
}

void onboard_led_enable()
{
    onboard_led_enable(last_stored_color);
}