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
#include <ti/drivers/GPIO.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/dpl/SemaphoreP.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#include "Hardware/Motor/motor.h"
#include "Hardware/OLED/oled.h"
#include "Hardware/Servo/servo.h"
#include "pid.h"

extern Display_Handle    display;
extern SemaphoreP_Handle semaphore_run;

extern uint32_t counter_per_100ms;
extern uint8_t  current_pos;
extern uint32_t counter_total;
extern uint8_t  user_mode;
extern uint8_t  user_value;

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

int16_t expeted_speed;

/**
 * @brief main 任务入口函数
 *
 * @param arg0 参数指针
 */
void mainTask(void *arg0) {
    float      speed;
    float      angle;
    char       oled_str[16];
    TickType_t init_tick;

    /* User lib init */
    taskENTER_CRITICAL();
    Motor_init();
    Servo_init();
    taskEXIT_CRITICAL();

    while (1) {
        /* Pend for semaphore */
        SemaphoreP_pend(semaphore_run, SemaphoreP_WAIT_FOREVER);
        counter_total = 0;
        /* 开始运行 */
        OLED_ShowStr(0, 3, "                ", 1);
        OLED_ShowStr(0, 3, "Running..", 1);
        /* 获取起始 tick */
        init_tick     = xTaskGetTickCount();
        expeted_speed = 6700 / user_value;
        /* PID init */
        PID_Init(&pid_speed, PID_DELTA, pid_speed_param, 1000, 700);
        PID_Init(&pid_servo, PID_DELTA, pid_servo_param, 80, 0);
        while (1) {
            if (!expeted_speed) {
                break;
            }
            speed = PID_Calc(&pid_speed, counter_per_100ms, expeted_speed);
            Motor_SetSpeed(speed >= 0 ? speed : 0);
            if (current_pos) {
                angle = PID_Calc(&pid_servo, current_pos, 40);
                Servo_setAngle(-angle + 90);
            }
            vTaskDelay(50);
        }
        Motor_SetSpeed(0);
        Servo_setAngle(90);
        /* Clean screen */
        OLED_ShowStr(0, 3, "                ", 1);
        /* Show time */
        sprintf(oled_str, "Time: %dms", xTaskGetTickCount() - init_tick);
        OLED_ShowStr(0, 3, oled_str, 1);
        for (int i = 0; i < 2; i++) {
            GPIO_write(CONFIG_GPIO_BUZZER, 1);
            vTaskDelay(200);
            GPIO_write(CONFIG_GPIO_BUZZER, 0);
            vTaskDelay(200);
        }
    }
}
