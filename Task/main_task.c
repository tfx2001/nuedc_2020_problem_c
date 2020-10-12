/* For usleep() */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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
extern uint8_t        current_pos;
extern uint32_t       counter_total;

const float pid_speed_param[3] = {
    0.1F,
    0.05F,
    0.0F,
};

const float pid_servo_param[3] = {
    1.0F,
    0.0F,
    0.5F,
};

static PidTypeDef pid_speed;
static PidTypeDef pid_servo;
static uint16_t   ccd_value[128];

int16_t expeted_speed = 800;

/**
 * @brief main 任务入口函数
 *
 * @param arg0 参数指针
 */
void mainTask(void *arg0) {
    float speed;
    float angle;
    char  oled_str[16];

    /* User lib init */
    taskENTER_CRITICAL();
    Motor_init();
    OLED_init();
    Servo_init();
    PID_Init(&pid_speed, PID_DELTA, pid_speed_param, 1000, 700);
    PID_Init(&pid_servo, PID_DELTA, pid_servo_param, 80, 0);

    /* Test OLED */
    OLED_ON();
    OLED_Fill(0x00);
    taskEXIT_CRITICAL();

    // Motor_SetSpeed(200);

    while (1) {
        speed = PID_Calc(&pid_speed, counter_per_100ms, expeted_speed);
        Motor_SetSpeed(speed >= 0 ? speed : 0);
        if (current_pos) {
            angle = PID_Calc(&pid_servo, current_pos, 40);
            Servo_setAngle(-angle + 90);
        } else {
            // Servo_setAngle(90);
        }
        // Display_print1(display, 0, 0, "Speed: %d", (uint32_t)speed);
        vTaskDelay(50);
    }
}
