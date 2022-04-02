#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#ifndef portTICK_PERIOD_MS
// Hack to remove compiler errors in vscode due to includePath issues
#define portTICK_PERIOD_MS 1
#endif

static const char *TAG = "nxsnf";

#define CLOCK_PIN 22
#define STORE_PIN 23
#define DATA_PIN 21

#define BLANK 15
#define NUM_SEGMENTS 8
static const char SEGMENT_VALUE[NUM_SEGMENTS] = {1,3,4,5,6,7,8,9};

static void configure_pin(gpio_num_t pin) {
    gpio_reset_pin(pin);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

static void sleep(int milis) {
    vTaskDelay(milis / portTICK_PERIOD_MS);
}

// Write the four least significant bits to the shift register
static void shift_out(char val) {
    for (int i = 3; i >= 0; i--) {
        gpio_set_level(CLOCK_PIN, 0);

        // if the value passed to myDataOut and a bitmask result
        //  true then... so if we are at i=6 and our value is
        //  %11010100 it would the code compares it to %01000000
        //  and proceeds to set pinState to 1.
        int out = 0;
        if (val & (1 << i)) {
            out = 1;
        }
        gpio_set_level(DATA_PIN, out);

        // register shifts bits on upstroke of clock pin
        gpio_set_level(CLOCK_PIN, 1);

        // zero the data pin after shift to prevent bleed through
        gpio_set_level(DATA_PIN, 0);
    }
}

static void write_vals(char vals[], int numVals, int delay) {
    gpio_set_level(STORE_PIN, 0);
    for (int i = 0; i < numVals; i++) {
        shift_out(vals[i]);
    }
    gpio_set_level(STORE_PIN, 1);

    if (delay > 0) {
        sleep(delay);
    }
}

static void number_chase(int times, int delay, bool trailToBlank) {
    int limit = NUM_SEGMENTS * times;
    for (int i = 0; i < limit; i++) {
        int valIdx = i % NUM_SEGMENTS;
        write_vals((char[]){SEGMENT_VALUE[valIdx]}, 1, delay);
    }

    if (trailToBlank) {
        write_vals((char[]){SEGMENT_VALUE[NUM_SEGMENTS - 3], SEGMENT_VALUE[NUM_SEGMENTS - 2], SEGMENT_VALUE[NUM_SEGMENTS - 1], BLANK}, 4, delay);
        write_vals((char[]){SEGMENT_VALUE[NUM_SEGMENTS - 2], SEGMENT_VALUE[NUM_SEGMENTS - 1], BLANK, BLANK}, 4, delay);
        write_vals((char[]){SEGMENT_VALUE[NUM_SEGMENTS - 1], BLANK, BLANK, BLANK}, 4, delay);
        write_vals((char[]){BLANK, BLANK, BLANK, BLANK}, 4, delay);
    }
}

static void count_all_up(int times, int delay) {
    int limit = NUM_SEGMENTS * times;
    for (int i = 0; i < limit; i++) {
        char val = SEGMENT_VALUE[i % NUM_SEGMENTS];
        write_vals((char[]){val, val, val, val}, 4, delay);
    }
}

static void count_all_down(int times, int delay) {
    int limit = NUM_SEGMENTS * times;
    for (int i = limit - 1; i >= 0; i--) {
        char val = SEGMENT_VALUE[i % NUM_SEGMENTS];
        write_vals((char[]){val, val, val, val}, 4, delay);
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Hello");

    configure_pin(CLOCK_PIN);
    configure_pin(STORE_PIN);
    configure_pin(DATA_PIN);

    int delay = 200;

    while (1) {
        for (int i = 0; i < 5; i++) {
            number_chase(1, delay, true);
        }
        write_vals((char[]){BLANK, BLANK, BLANK, BLANK}, 4, delay * 5);

        for (int i = 0; i < 5; i++) {
            count_all_up(1, delay);
            count_all_down(1, delay);
            sleep(delay * 5);
        }

        write_vals((char[]){BLANK, BLANK, BLANK, BLANK}, 4, delay * 15);
        ESP_LOGI(TAG, "Finished main loop");
    }
}
