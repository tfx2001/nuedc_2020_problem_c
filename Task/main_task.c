/* For usleep() */
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

/* FreeRTOS Header files */
#include <FreeRTOS.h>
#include <task.h>

/* Driver Header files */
#include <ti/display/Display.h>
#include <ti/drivers/ADC.h>
#include <ti/drivers/PWM.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#include "Hardware/Motor/motor.h"
#include "Hardware/OLED/oled.h"
#include "Hardware/Servo/servo.h"
#include "pid.h"

extern Display_Handle display;
extern uint32_t       counter_per_100ms;

const float pid_speed_param[3] = {
    0.1F,
    0.1F,
    0.0F,
};

static PidTypeDef pid_speed;
static uint16_t   ccd_value[128];

/**
 * @brief main 任务入口函数
 *
 * @param arg0 参数指针
 */
void mainTask(void *arg0) {
    float speed;

    /* User lib init */
    taskENTER_CRITICAL();
    Motor_init();
    OLED_init();
    Servo_init();
    PID_Init(&pid_speed, PID_DELTA, pid_speed_param, 1000, 700);

    /* Test OLED */
    OLED_ON();
    OLED_Fill(0x00);
    OLED_ShowStr(0, 0, "hello!", 1);
    taskEXIT_CRITICAL();

    // Motor_SetSpeed(200);

    while (1) {
        speed = PID_Calc(&pid_speed, counter_per_100ms, 500);
        // Display_print1(display, 0, 0, "Speed: %d", (uint32_t)speed);
        // OLED_ShowStr(0, 0, "   ", 1);
        // Motor_SetSpeed((int32_t)speed);
        vTaskDelay(100);
    }
}
