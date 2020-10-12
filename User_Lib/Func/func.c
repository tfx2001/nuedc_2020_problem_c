#include "func.h"

#include <stdlib.h>

uint8_t Block_Judge(uint8_t Value[], uint8_t Size) {
    uint8_t tCountBlack = 0;
    uint8_t tCountWhite = 0;

    for (int i = 0; i < Size; i++) {
        if (!Value[i]) {
            tCountBlack++;
        } else {
            tCountWhite++;
        }
    }

    if (tCountBlack < tCountWhite && tCountBlack > 10) {
        return BLOCK_OK;
    } else if (tCountWhite < tCountBlack && tCountWhite > 10) {
        return BLOCK_OK;
    } else {
        return BLOCK_ERROR;
    }
}

uint8_t Block_MidPoint(uint16_t Value[], uint8_t Size) {
    uint16_t lPos = 0, rPos = 0;
    uint8_t  flag = 0;

    for (int i = 3; i < Size; i++) {
        /* 下降 */
        if (Value[i - 3] - Value[i - 2] >= 8 && Value[i - 2] - Value[i - 1] >= 8 &&
            Value[i - 1] - Value[i] >= 8) {
            lPos = i;
            flag = 0;
            /* 上升 */
            for (int j = i; j < Size; j++) {
                if (Value[j - 2] - Value[j - 3] >= 5 && Value[j - 1] - Value[j - 2] >= 5 &&
                    Value[j] - Value[j - 1] >= 5) {
                    rPos = j;
                    flag = 1;
                    break;
                }
            }
            if (flag) {
                break;
            }
        }
    }

    if (rPos - lPos >= 5) {
        return (rPos + lPos) / 2;
    } else {
        return 0;
    }
}

void Block_Filter(uint16_t Value[], uint16_t Filted[], uint8_t Szie) {
    uint16_t sum;

    for (int i = 0; i < Szie; i++) {
        sum = 0;

        for (int j = -1; j <= 1; j++) {
            sum += Value[i + j];
        }
        Filted[i] = sum / 3;
    }
}
