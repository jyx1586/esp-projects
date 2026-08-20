#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_log.h"
#include "soc/soc.h"
#include "soc/gpio_reg.h"

#define A   GPIO_NUM_11
#define B   GPIO_NUM_7
#define C   GPIO_NUM_4
#define D   GPIO_NUM_2
#define E   GPIO_NUM_1
#define F   GPIO_NUM_13
#define G   GPIO_NUM_5
#define DP   GPIO_NUM_3
#define SEGMENTS_PIN_SEL ((1ULL << A) | (1ULL << B) | (1ULL << C) | \
                          (1ULL << D) | (1ULL << E) | (1ULL << F) | \
                          (1ULL << G) | (1ULL << DP))

#define DIG1 GPIO_NUM_12
#define DIG2 GPIO_NUM_19
#define DIG3 GPIO_NUM_18
#define DIG4 GPIO_NUM_6
#define DIGITS_PIN_SEL (((1ULL << DIG1) | (1ULL << DIG2) | (1ULL << DIG3) | (1ULL << DIG4)))

static const char *TAG = "LED_DISPLAY";
const uint32_t digit_gen[10] = {
    ((1ULL << A) | (1ULL << B) | (1ULL << C) | (1ULL << D) | (1ULL << E) | (1ULL << F)), //0
    ((1ULL << B) | (1ULL << C)), //1
    ((1ULL << A) | (1ULL << B) | (1ULL << G) | (1ULL << E) | (1ULL << D)), //2
    ((1ULL << A) | (1ULL << B) | (1ULL << C) | (1ULL << D) | (1ULL << G)), //3
    ((1ULL << F) | (1ULL << G) | (1ULL << B) | (1ULL << C)), //4
    ((1ULL << A) | (1ULL << F) | (1ULL << G) | (1ULL << C) | (1ULL << D)), //5
    ((1ULL << A) | (1ULL << F) | (1ULL << E) | (1ULL << D) | (1ULL << C) | (1ULL << G)), //6
    ((1ULL << A) | (1ULL << B) | (1ULL << C)), //7
    ((1ULL << A) | (1ULL << B) | (1ULL << C) | (1ULL << D) | (1ULL << E) | (1ULL << F) | (1ULL << G)), //8
    ((1ULL << A) | (1ULL << B) | (1ULL << C) | (1ULL << D) | (1ULL << F) | (1ULL << G))//9
};
uint16_t number = 3059; // number to display
uint8_t digit; // digit number currently displayed
const gpio_num_t segment_pins[8] = {
    A, B, C, D, E, F, G, DP
};
const gpio_num_t digit_pins[4] = {
    DIG1, DIG2, DIG3, DIG4
};

void rst_segments(){
    for (int i=0; i<8; i++){
        gpio_set_level(segment_pins[i], 1);
    }
}
void rst_digits(){
    for (int i=0; i<4; i++){
        gpio_set_level(digit_pins[i], 1);
    }
}

void display_num(){ // displays a number 
    rst_digits();
    
    uint64_t target_pattern = 0;
    int num = 0; 
    switch(digit){ // which digit is displaying 
        case 0:
            num = (number / 1000) % 10;
            break;
        case 1:
            num = (number / 100) % 10;
            break;
        case 2:
            num = (number / 10) % 10;
            break;
        case 3:
            num = number % 10;
            break;
    }
    target_pattern = digit_gen[num]; 
    for (int i=0; i<8; i++){
        bool bit_is_set = (target_pattern & (1ULL << segment_pins[i])) == 0; // if the bit IS set already (for CA, being set means 1 which means it is off), then the & produces a nonzero value and that means it is != 0, so the output is 1. if the bit IS NOT set, then the & operation produces a 0, and != 0 is FALSE: 0. 
        gpio_set_level(segment_pins[i], bit_is_set ? 0 : 1);
    }

    gpio_set_level(digit_pins[digit], 0);
    vTaskDelay(pdMS_TO_TICKS(1));
    digit = (digit+1) % 4;
}

void cycle_through(){ // cycles through all digits and all segments
    rst_digits();
    rst_segments();
    gpio_set_level(digit_pins[digit], 0);

    for (int i=0; i<8; i++){
        gpio_set_level(segment_pins[i], 0);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(segment_pins[i], 1);
    }
    digit = (digit+1) % 4;
}

void app_main() {
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_LOGE(TAG, "=========== Begin program ===========");
    ESP_LOGI(TAG, "Initializing...");

    gpio_config_t io_config = {};
    io_config.intr_type = GPIO_INTR_DISABLE;
    io_config.mode = GPIO_MODE_OUTPUT;
    io_config.pull_up_en = GPIO_PULLUP_DISABLE;
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.pin_bit_mask = SEGMENTS_PIN_SEL;
    gpio_config(&io_config);
    io_config.pin_bit_mask = DIGITS_PIN_SEL;
    gpio_config(&io_config);
    
    // reset segment pins by setting HIGH
    ESP_LOGI(TAG, "Resetting Segment Pins");
    rst_segments();

    while(1){
        cycle_through();
    }
}