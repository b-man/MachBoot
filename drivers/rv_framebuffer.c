/*
 * Copyright 2013, winocm. <winocm@icloud.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 *   If you are going to use this software in any form that does not involve
 *   releasing the source to this project or improving it, let me know beforehand.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "genboot.h"
#include <stdarg.h>

#define HARDWARE_REGISTER(x)    *((unsigned int*)(x)) 

#define LCDTIMING0_PPL(x)           ((((x) / 16 - 1) & 0x3f) << 2)
#define LCDTIMING1_LPP(x)           (((x) & 0x3ff) - 1)
#define LCDCONTROL_LCDPWR           (1 << 11)
#define LCDCONTROL_LCDEN            (1)
#define LCDCONTROL_LCDBPP(x)        (((x) & 7) << 1)
#define LCDCONTROL_LCDTFT           (1 << 5)

#define PL111_TIMINGS_0         0x0
#define PL111_TIMINGS_1         0x4
#define PL111_TIMINGS_2         0x8
#define PL111_TIMINGS_3         0xC

#define PL111_UPPER_FB          0x10
#define PL111_LOWER_FB          0x14
#define PL111_CONTROL           0x18

uint32_t gRealviewPl111Base;
uint32_t gFbLength, gFbAddress;
uint32_t gFbWidth, gFbHeight;
int isRgb888;

void framebuffer_init(void)
{
    gRealviewPl111Base = 0x10020000;
    void* framebuffer = (void*)FRAMEBUFFER_ADDRESS;
    gFbAddress = (uint32_t)FRAMEBUFFER_ADDRESS;

    uint32_t depth = 2;
    uint32_t width = 1024;
    gFbWidth = width;

    uint32_t height = 768;
    gFbHeight = height;

    uint32_t pitch = (width * depth);
    gFbLength = (width * height * depth);
    
    /* Set framebuffer address */
    HARDWARE_REGISTER(gRealviewPl111Base + PL111_UPPER_FB) = (uint32_t)framebuffer;
    HARDWARE_REGISTER(gRealviewPl111Base + PL111_LOWER_FB) = (uint32_t)framebuffer;
    
    /* Initialize timings to 1024x768x16 */
	HARDWARE_REGISTER(gRealviewPl111Base + PL111_TIMINGS_0) = LCDTIMING0_PPL(width);
    HARDWARE_REGISTER(gRealviewPl111Base + PL111_TIMINGS_1) = LCDTIMING1_LPP(height);
    
    /* Enable the TFT/LCD Display */
	HARDWARE_REGISTER(gRealviewPl111Base + PL111_CONTROL) =
                         LCDCONTROL_LCDEN |
                         LCDCONTROL_LCDTFT |
                         LCDCONTROL_LCDPWR |
                         LCDCONTROL_LCDBPP(4);
    
    gBootArgs.Video.v_baseAddr = (unsigned long)framebuffer;
    gBootArgs.Video.v_rowBytes = width * 2;
    gBootArgs.Video.v_width = width;
    gBootArgs.Video.v_height = height;
    gBootArgs.Video.v_depth = 2 * (8);   // 16bpp

    isRgb888 = 0;
    
    printf("framebuffer_init(): framebuffer initialized\n");
    bzero(framebuffer, (pitch * height));
}

uint32_t framebuffer_color(uint32_t red, uint32_t green, uint32_t blue)
{
    uint32_t low = (((blue >> 3) << 11)| ((green >> 2) << 5)| ((red >> 3) << 0));
    return low | (low << 16);
}

int command_bgcolor(int argc, char* argv[]) {
    if(argc != 3) {
        printf("usage: bgcolor <red> <green> <blue>\n");
        return -1;
    }

    uint32_t r, g, b;
    r = strtoul(argv[1], NULL, 10);
    g = strtoul(argv[2], NULL, 10);
    b = strtoul(argv[3], NULL, 10);

    /* Set it all. */
    uint32_t fb_color = framebuffer_color(r, g, b);
    memset_pattern4((void*)FRAMEBUFFER_ADDRESS, (uint32_t*)&fb_color, gFbLength);

    return 0;
}