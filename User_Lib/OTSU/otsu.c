/*
 * =====================================================================================
 *        COPYRIGHT NOTICE
 *        Copyright (c) 2012  HUST-Renesas Lab
 *        ALL rights reserved.
 *//**        
 *        @file     otsu.c
 *
 *        @brief    大津法——自动阈值
 *
 *        @version  0.1
 *        @date     2012/7/24 22:15
 *
 *        @author   Hu Chunxu , hcx196@gmail.com
 *//* ==================================================================================
 *  @0.1    Hu Chunxu   2012/7/24   create orignal file
 * =====================================================================================
 */
#include "otsu.h"

#include <stdio.h>
#include <string.h>

/**
 *    @brief   算法对输入的灰度图像的直方图进行分析，将直方图分成两个部分，使得两部分之间的距离最大。
 *             划分点就是求得的阈值
 *
 *    @param   *image     buffer for image
 *    @param   rows       size of image
 *    @param   cols       size of image
 *    @param   x0, y0     region of vector used for computing threshold
 *    @param   dx, dy     region of vector used for computing threshold
 *
 *    @return  thresholdValue  threshold
 */
int otsu(unsigned char *image, int rows, int cols, int x0, int y0, int dx, int dy) {
    unsigned char *np;                 /* 图像指针 */
    int            thresholdValue = 1; /* 阈值 */
    int            ihist[256];         /* 图像直方图，256个点 */

    int    i, j, k; /* various counters */
    int    n, n1, n2, gmin, gmax;
    double m1, m2, sum, csum, fmax, sb;

    /* 对直方图置零... */
    memset(ihist, 0, sizeof(ihist));

    gmin = 255;
    gmax = 0;
    /* 生成直方图 */
    for (i = 0; i < cols; i++) {
        np = &image[i];
        ihist[*np]++;
    }

    /* set up everything */
    sum = csum = 0.0;
    n          = 0;

    for (k = 0; k <= 255; k++) {
        sum += (double)k * (double)ihist[k]; /* x*f(x) 质量矩*/
        n += ihist[k];                       /* f(x) 质量 */
    }

    if (!n) {
        /* if n has no value, there is problems... */
        // printf("NOT NORMAL thresholdValue = 160\n");
        return (160);
    }

    /* do the otsu global thresholding method */
    fmax = -1.0;
    n1   = 0;
    for (k = 0; k < 255; k++) {
        n1 += ihist[k];
        if (!n1) {
            continue;
        }
        n2 = n - n1;
        if (n2 == 0) {
            break;
        }
        csum += (double)k * ihist[k];
        m1 = csum / n1;
        m2 = (sum - csum) / n2;
        sb = (double)n1 * (double)n2 * (m1 - m2) * (m1 - m2);
        /* bbg: note: can be optimized. */
        if (sb > fmax) {
            fmax           = sb;
            thresholdValue = k;
        }
    }

    /* at this point we have our thresholding value */
    return (thresholdValue);
}