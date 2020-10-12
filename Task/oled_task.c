#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>

#include <ti/display/Display.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/apps/Button.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#include <ti_drivers_config.h>

#include <Hardware/OLED/oled.h>

extern SemaphoreP_Handle semaphore_run;

Button_Handle button_mode;
Button_Handle button_value;

uint8_t user_mode  = 0;
uint8_t user_value = 10;

void Button_Mode_Callback(Button_Handle buttonHandle, Button_EventMask buttonEvents) {
    char oled_str[16];

    switch (buttonEvents) {
        case Button_EV_CLICKED:
            user_mode = user_mode ? 0 : 1;
            OLED_ShowStr(0, 1, "      ", 1);
            OLED_ShowStr(0, 2, "         ", 1);
            OLED_ShowStr(0, 3, "                ", 1);
            switch (user_mode) {
                case 0:
                    OLED_ShowStr(0, 1, "Mode 1", 1);
                    break;
                case 1:
                    OLED_ShowStr(0, 1, "Mode 2", 1);
                    sprintf(oled_str, "Time: %ds", user_value);
                    OLED_ShowStr(0, 2, oled_str, 1);
                    break;
            }
            break;
    }
}

void Button_Value_Callback(Button_Handle buttonHandle, Button_EventMask buttonEvents) {
    char oled_str[16];

    /* Mode 1 */
    if (!user_mode) {
        switch (buttonEvents) {
            case Button_EV_CLICKED:
                /* 开始巡线 */
                SemaphoreP_post(semaphore_run);
                break;
        }
    }
    /* Mode 2 */
    else {
        switch (buttonEvents) {
            case Button_EV_CLICKED:
                user_value++;
                user_value = user_value >= 20 ? 20 : user_value;
                break;
            case Button_EV_DOUBLECLICKED:
                user_value--;
                user_value = user_value <= 10 ? 10 : user_value;
                break;
            case Button_EV_LONGPRESSED:
                /* 开始巡线 */
                SemaphoreP_post(semaphore_run);
                break;
        }
        sprintf(oled_str, "Time: %ds", user_value);
        OLED_ShowStr(0, 2, oled_str, 1);
    }
}

void oledTask(void *arg0) {
    Button_Params button_params;

    Button_init();
    Button_Params_init(&button_params);
    button_params.longPressDuration           = 1000;
    button_params.doublePressDetectiontimeout = 100;
    button_params.buttonEventMask = Button_EV_CLICKED | Button_EV_DOUBLECLICKED | Button_EV_LONGPRESSED;
    button_mode                   = Button_open(CONFIG_BUTTON_MODE, Button_Mode_Callback, &button_params);
    button_value                  = Button_open(CONFIG_BUTTON_VALUE, Button_Value_Callback, &button_params);

    OLED_init();
    OLED_ON();
    OLED_CLS();

    OLED_ShowStr(0, 0, "Self-Driving Car", 1);
    OLED_ShowStr(0, 1, "Mode 1", 1);
    while (1) {
        vTaskDelay(1000);
    }
}
