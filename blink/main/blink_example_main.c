/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

#define L1_A_GPIO 32
#define L1_B_GPIO 13
#define L1_C_GPIO 25
#define L1_D_GPIO 33
#define STATUS_LED_GPIO 26

#define BLINK_GPIO CONFIG_BLINK_GPIO


// static uint8_t s_led_state = 0;

#ifdef CONFIG_BLINK_LED_RMT
static led_strip_t *pStrip_a;

static void blink_led(void)
{
    /* If the addressable LED is enabled */
    if (s_led_state) {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        pStrip_a->set_pixel(pStrip_a, 0, 16, 16, 16);
        /* Refresh the strip to send data */
        pStrip_a->refresh(pStrip_a, 100);
    } else {
        /* Set all LED off to clear all pixels */
        pStrip_a->clear(pStrip_a, 50);
    }
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    pStrip_a = led_strip_init(CONFIG_BLINK_LED_RMT_CHANNEL, BLINK_GPIO, 1);
    /* Set all LED off to clear all pixels */
    pStrip_a->clear(pStrip_a, 50);
}

#elif CONFIG_BLINK_LED_GPIO

// static void blink_led(void)
// {
//     /* Set the GPIO level according to the state (LOW or HIGH)*/
//     gpio_set_level(BLINK_GPIO, s_led_state);
// }

static void configure_led(gpio_num_t pin)
{
    ESP_LOGI(TAG, "Example configured to blink GPIO LED! on pin %d", pin);
    gpio_reset_pin(pin);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

static void show_value(unsigned short val)
{
    switch (val) {
    case 0:
        gpio_set_level(L1_D_GPIO, false);
        gpio_set_level(L1_C_GPIO, false);
        gpio_set_level(L1_B_GPIO, false);
        gpio_set_level(L1_A_GPIO, false);
        break;
    case 1:
        gpio_set_level(L1_D_GPIO, false);
        gpio_set_level(L1_C_GPIO, false);
        gpio_set_level(L1_B_GPIO, false);
        gpio_set_level(L1_A_GPIO, true);
        break;
    case 2:
        gpio_set_level(L1_D_GPIO, false);
        gpio_set_level(L1_C_GPIO, false);
        gpio_set_level(L1_B_GPIO, true);
        gpio_set_level(L1_A_GPIO, false);
        break;
    case 3:
        gpio_set_level(L1_D_GPIO, false);
        gpio_set_level(L1_C_GPIO, false);
        gpio_set_level(L1_B_GPIO, true);
        gpio_set_level(L1_A_GPIO, true);
        break;
    case 4:
        gpio_set_level(L1_D_GPIO, false);
        gpio_set_level(L1_C_GPIO, true);
        gpio_set_level(L1_B_GPIO, false);
        gpio_set_level(L1_A_GPIO, false);
        break;
    case 5:
        gpio_set_level(L1_D_GPIO, false);
        gpio_set_level(L1_C_GPIO, true);
        gpio_set_level(L1_B_GPIO, false);
        gpio_set_level(L1_A_GPIO, true);
        break;
    case 6:
        gpio_set_level(L1_D_GPIO, false);
        gpio_set_level(L1_C_GPIO, true);
        gpio_set_level(L1_B_GPIO, true);
        gpio_set_level(L1_A_GPIO, false);
        break;
    case 7:
        gpio_set_level(L1_D_GPIO, false);
        gpio_set_level(L1_C_GPIO, true);
        gpio_set_level(L1_B_GPIO, true);
        gpio_set_level(L1_A_GPIO, true);
        break;
    case 8:
        gpio_set_level(L1_D_GPIO, true);
        gpio_set_level(L1_C_GPIO, false);
        gpio_set_level(L1_B_GPIO, false);
        gpio_set_level(L1_A_GPIO, false);
        break;
    case 9:
        gpio_set_level(L1_D_GPIO, true);
        gpio_set_level(L1_C_GPIO, false);
        gpio_set_level(L1_B_GPIO, false);
        gpio_set_level(L1_A_GPIO, true);
        break;
    }
}

#endif

// void app_main(void)
// {

//     /* Configure the peripheral according to the LED type */
//     configure_led(BLINK_GPIO);

//     while (1) {
//         ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
//         blink_led();
//         /* Toggle the LED state */
//         s_led_state = !s_led_state;
//         vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
//     }
// }

void app_main(void)
{
    configure_led(L1_A_GPIO);
    configure_led(L1_B_GPIO);
    configure_led(L1_C_GPIO);
    configure_led(L1_D_GPIO);
    configure_led(STATUS_LED_GPIO);

    // 8 and 9 aren't used which acts as a blank
    show_value(8);

    while (1) {
        for (unsigned short i = 0; i <= 7; i++) {
            ESP_LOGI(TAG, "Turning on output %d!", i);
            gpio_set_level(STATUS_LED_GPIO, true);
            show_value(i);
            vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
            gpio_set_level(STATUS_LED_GPIO, false);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }

        show_value(8);

        ESP_LOGI(TAG, "Finished numbered loop");
        for (unsigned short i = 0; i < 3; i++) {
            gpio_set_level(STATUS_LED_GPIO, true);
            vTaskDelay(300 / portTICK_PERIOD_MS);
            gpio_set_level(STATUS_LED_GPIO, false);
            vTaskDelay(300 / portTICK_PERIOD_MS);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
