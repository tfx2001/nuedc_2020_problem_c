#include <Hardware/Motor/motor.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/PWM.h>

#include <ti_drivers_config.h>

#define AIN1_GPIO_CONFIG  CONFIG_GPIO_0
#define AIN2_GPIO_CONFIG  CONFIG_GPIO_1
#define MOTOR1_PWM_CONFIG CONFIG_PWM_MOTOR

static PWM_Handle pwm_motor;

void Motor_init() {
    PWM_Params pwmParams;

    /* Config PWM */
    PWM_Params_init(&pwmParams);
    pwmParams.periodUnits = PWM_PERIOD_HZ;
    pwmParams.periodValue = 1e4;
    pwmParams.dutyUnits = PWM_DUTY_FRACTION;
    pwmParams.dutyValue = 0;

    pwm_motor = PWM_open(MOTOR1_PWM_CONFIG, &pwmParams);
    PWM_start(pwm_motor);

    if (pwm_motor == 0) {
        while (1);
    }
}

void Motor_SetSpeed(int32_t Speed) {
    if (Speed >= 0) {
        GPIO_write(AIN1_GPIO_CONFIG, 0);
        GPIO_write(AIN2_GPIO_CONFIG, 1);
    } else {
        Speed = -Speed;
        GPIO_write(AIN1_GPIO_CONFIG, 1);
        GPIO_write(AIN2_GPIO_CONFIG, 0);
    }

    PWM_setDuty(pwm_motor, (uint32_t)(((uint64_t)PWM_DUTY_FRACTION_MAX * (Speed / 10.0F)) / 100));
}
