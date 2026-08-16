#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define LED_BLINK 4

void app_main() {
    vTaskDelay(pdMS_TO_TICKS(2000));

    gpio_reset_pin(LED_BLINK);
    gpio_set_direction(LED_BLINK, GPIO_MODE_OUTPUT);

    while(1) {
    gpio_set_level(LED_BLINK, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("LED ON");

    gpio_set_level(LED_BLINK, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("LED OFF");
    }
}