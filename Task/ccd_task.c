#include <stddef.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>

#include <ti/drivers/UART.h>

#include <ti_drivers_config.h>

#include "Hardware/CCD/ccd.h"

#include "User_Lib/OTSU/otsu.h"

/* Encoder counter */
uint16_t ccd_value[128];

void ccdTask(void *arg0) {
    UART_Params uartParams;
    UART_Handle uart;
    int         threshold_auto;
    uint8_t     data_trans[20];

    UART_Params_init(&uartParams);
    uartParams.readDataMode  = UART_DATA_BINARY;
    uartParams.writeDataMode = UART_DATA_TEXT;
    uart                     = UART_open(CONFIG_UART_NO_NEWLINE, &uartParams);

    UART_write(uart, "UART open successful.\n", 22);
    CCD_init();

    while (1) {
        CCD_read(ccd_value);
        // threshold_auto = otsu(ccd_value, 1, 128, -1, -1, 128, 0);
        UART_write(uart, "T", 1);
        for (int i = 0; i < 128; i++) {
            data_trans[0] = ccd_value[i];
            // data_trans[0] = data_trans[0] > 230 ? 255 : 0;
            data_trans[0] = data_trans[0] < 80 ? 0 : 255;
            UART_write(uart, data_trans, 1);
        }
        /* Wait for interating */
        vTaskDelay(200);
    }
}
