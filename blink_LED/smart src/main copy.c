#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "led_strip.h"

#define BLINK_GPIO 8
static const char *TAG = "BLINK_TEST";

static led_strip_handle_t led_strip;

void configure_led(void){
    ESP_LOGI(TAG, "configuring hardware RMT...");

    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1,
    };

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    led_strip_clear(led_strip);
}

void app_main(void){
    vTaskDelay(pdMS_TO_TICKS(2000));

    configure_led();
    
    // blink loop
    while (1) {
        // Red Color: (Handle, LED Index, Red, Green, Blue)
        ESP_LOGI(TAG, "Setting color to RED");
        led_strip_set_pixel(led_strip, 0, 255, 0, 0); 
        led_strip_refresh(led_strip); // Push data to the LED hardware
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Green Color
        ESP_LOGI(TAG, "Setting color to GREEN");
        led_strip_set_pixel(led_strip, 0, 0, 255, 0);
        led_strip_refresh(led_strip);
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Blue Color
        ESP_LOGI(TAG, "Setting color to BLUE");
        led_strip_set_pixel(led_strip, 0, 0, 0, 255);
        led_strip_refresh(led_strip);
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        // Turn Off
        ESP_LOGI(TAG, "Turning LED OFF");
        led_strip_clear(led_strip);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}