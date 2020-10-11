#include "servo.h"

/* Ti Drivers Headfiles */
#include <ti/drivers/PWM.h>

#include <ti_drivers_config.h>

/* FreeRTOS Headfiles */
#include <FreeRTOS.h>
#include <task.h>

static PWM_Handle pwm_servo;

void Servo_init() {
    PWM_Params pwmParams;

    PWM_Params_init(&pwmParams);
    pwmParams.periodUnits = PWM_PERIOD_US;
    pwmParams.periodValue = 20000;
    pwmParams.dutyUnits   = PWM_DUTY_US;
    pwmParams.dutyValue   = 1500;
    pwm_servo             = PWM_open(CONFIG_PWM_SERVO, &pwmParams);
    if (pwm_servo == NULL) {
        while (1)
            ;
    }
    PWM_start(pwm_servo);
}

void Servo_setAngle(uint8_t Angle) {
    PWM_setDuty(pwm_servo, (Angle / 180.0F) * 2000 + 500);
}
