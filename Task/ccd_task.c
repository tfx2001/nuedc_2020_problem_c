#include <stddef.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>

#include <ti/display/Display.h>

#include <ti_drivers_config.h>

#include "Hardware/CCD/ccd.h"

#include "User_Lib/OTSU/otsu.h"

/* Encoder counter */
uint16_t ccd_value[128];

extern Display_Handle display;

void ccdTask(void *arg0) {
    int threshold_auto;
    CCD_init();

    while (1) {
        CCD_read(ccd_value);
        threshold_auto = otsu(ccd_value, 1, 128, -1, -1, 128, 0);
        // for (int i = 0; i < 128; i++) {
        //     ccd_value[i] = ccd_value[i] < threshold_auto ? 0 : 1;
        // }
        Display_printf(display, 0, 0, "Threshold: %3d CCD: %5d%5d%5d%5d%5d%5d%5d%5d%5d%5d", threshold_auto,
                       ccd_value[0], ccd_value[12], ccd_value[24], ccd_value[36], ccd_value[48], ccd_value[60],
                       ccd_value[72], ccd_value[84], ccd_value[96], ccd_value[108]);
        /* Wait for interating */
        vTaskDelay(20);
    }
}