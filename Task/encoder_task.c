#include <stddef.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>

#include <ti/drivers/GPIO.h>
#include <ti/display/Display.h>

#include <ti_drivers_config.h>

/* Encoder counter */
uint32_t counter = 0;
uint32_t counter_per_100ms = 0;

extern Display_Handle display;

void encoderTask(void *arg0) {
    /* Enable interrupt */
    GPIO_enableInt(CONFIG_GPIO_ENCODER);

    while (1) {
        // Display_print1(display, 0, 0, "Counter: %d", counter);
        counter_per_100ms = counter;
        counter = 0;
        vTaskDelay(100);
    }
}

void Encoder_Callback(uint_least8_t index) {
    counter++;
}
