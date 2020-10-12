#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include <ti/display/Display.h>
#include <ti/drivers/GPIO.h>

#include <ti_drivers_config.h>

#include <Hardware/OLED/oled.h>

/* Encoder counter */
uint32_t counter           = 0;
uint32_t counter_per_100ms = 0;
uint32_t counter_total     = 0;

extern int16_t expeted_speed;

extern Display_Handle display;

void encoderTask(void *arg0) {
    char oled_str[16];

    /* Enable interrupt */
    GPIO_enableInt(CONFIG_GPIO_ENCODER);

    while (1) {
        counter_per_100ms = counter;
        counter_total += counter;
        if (counter_total >= 67000) {
            expeted_speed = 0;
        }
        counter = 0;
        vTaskDelay(100);
    }
}

void Encoder_Callback(uint_least8_t index) {
    counter++;
}
