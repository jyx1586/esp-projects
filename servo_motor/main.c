/* This code controls servo or LED with Pulse Width Modulation
 with the LEDC hardware peripheral. Current code applies to -90 to 90 
 degree rotation of servo motor. 
*/
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include <math.h>

#define LEDC_TIMER      LEDC_TIMER_0
#define LEDC_MODE       LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES   LEDC_TIMER_15_BIT
#define LEDC_FREQUENCY  50 // Frequency in Hertz. Set frequency at 50 Hz for servo control
#define LEDC_OUTPUT_IO  5 // define output GPIO
#define LEDC_CLK_CFG    LEDC_AUTO_CLK
#define LEDC_CHANNEL    LEDC_CHANNEL_0

static const char *TAG = "SERVO_TASK";

void servo_init(){
    ESP_LOGI(TAG, "Initializing Servo...");

    // set and run initialization functions
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_CLK_CFG
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));

    ledc_channel_config_t channel_conf = {
        .gpio_num = LEDC_OUTPUT_IO,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .duty = 0, // set duty to 0%
        .hpoint = 0 // 
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel_conf));

    ESP_LOGI(TAG, "Servo ready");
}

void set_duty(uint32_t duty){
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
    ESP_LOGI(TAG, "Set duty to %lu", duty);
}

// servo loop which moves from -90 to 90 degrees
void servo_loop(){
    // init servo
    servo_init();

    int total_cycles = 117;
    int step = 14;
    int duty = 1638;
    bool pos_direction = true;
    int i;
    while(1) {
        for (i=0; i<total_cycles; i++){
            if (pos_direction) {
                duty += step;
            } else {
                duty -= step;
            }
            set_duty(duty);
            ESP_LOGI(TAG, "set duty to %d", duty);

            vTaskDelay(pdMS_TO_TICKS(100));
        }
        pos_direction = !pos_direction;
    }
}

void app_main(){
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI("MAIN", "========== APP MAIN ==========");

    ESP_LOGI("MAIN", "========== CALLING SERVO LOOP ==========");
    servo_loop();
}