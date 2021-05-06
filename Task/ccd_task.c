#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include <ti/display/Display.h>
#include <ti/drivers/UART.h>

#include <ti_drivers_config.h>

#include "Hardware/CCD/ccd.h"

#include "User_Lib/Func/func.h"
// #include "User_Lib/OTSU/otsu.h"

extern int16_t expeted_speed;

uint16_t ccd_value[128];
uint16_t ccd_value_filted[128];
uint8_t  ccd_binary_black[128];
uint8_t  ccd_binary_white[128];
uint8_t  current_pos = 0;

extern Display_Handle display;

void ccdTask(void *arg0) {
    UART_Params uartParams;
    UART_Handle uart;

    int     mid_point;
    char    data_trans[16];
    uint8_t data_len;
    uint8_t flag = 0;

    UART_Params_init(&uartParams);
    uartParams.readDataMode  = UART_DATA_BINARY;
    uartParams.writeDataMode = UART_DATA_TEXT;
    uart                     = UART_open(CONFIG_UART_NO_NEWLINE, &uartParams);

    CCD_init();

    UART_write(uart, "UART open successful.\n", 22);
    Display_print0(display, 0, 0, "Display open successful!");

    while (1) {
        CCD_read(ccd_value);
        Block_Filter(ccd_value + 5, ccd_value_filted + 5, 118);
        /* 二值化 */
        // UART_write(uart, "T", 1);
        flag = 0;
        for (int i = 0; i < 128; i++) {
            if (ccd_value_filted[i] < 80) {
                flag++;
            }
            if (ccd_value_filted[i] < 80) {
                ccd_binary_black[i] = 0;
            } else {
                ccd_binary_black[i] = 255;
            }
            // UART_write(uart, ccd_value_filted + i, 1);
        }
        /* 找到线 */
        if (flag >= 60) {
            expeted_speed = 0;
        } else if (Block_Judge(ccd_binary_black + 5, 118) == BLOCK_OK) {
            mid_point = Block_MidPoint(ccd_value_filted + 5, 118) + 5;
            if (mid_point != 5) {
                // data_len = sprintf(data_trans, "Black: %d\n", (int)mid_point);
                // UART_write(uart, data_trans, data_len);
                current_pos = mid_point;
            }
        } else {
            current_pos = 0;
        }
        vTaskDelay(20);
    }
}
