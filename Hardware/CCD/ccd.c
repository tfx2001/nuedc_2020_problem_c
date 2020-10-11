#include "ccd.h"

/* Ti Drivers Headfiles */
#include <ti/drivers/ADC.h>
#include <ti/drivers/GPIO.h>

#include <ti_drivers_config.h>

/* FreeRTOS Headfiles */
#include <FreeRTOS.h>
#include <task.h>

/* ADC Handle */
static ADC_Handle adc;

void CCD_init() {
    ADC_Params adcParams;

    /* ADC init */
    ADC_Params_init(&adcParams);
    adcParams.isProtected = true;
    adc                   = ADC_open(CONFIG_ADC_0, &adcParams);
}

void CCD_read(uint16_t Vaules[128]) {
    /* 进入临界段防止时序混乱 */
    taskENTER_CRITICAL();
    /* Tick 1 */
    GPIO_write(CONFIG_GPIO_SI, 1);
    GPIO_write(CONFIG_GPIO_CLK, 1);
    GPIO_write(CONFIG_GPIO_SI, 0);
    for (int i = 0; i < 128; i++) {
        /* Sampling at CLK falling edge */
        GPIO_write(CONFIG_GPIO_CLK, 0);
        ADC_convert(adc, Vaules + i);
        GPIO_write(CONFIG_GPIO_CLK, 1);
    }
    GPIO_write(CONFIG_GPIO_CLK, 0);
    taskEXIT_CRITICAL();
}
