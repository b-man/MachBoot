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

typedef struct _nvram_variable {
    char name[64];
    char setting[256];
    int overridden;
} nvram_variable_t;

extern nvram_variable_t gNvramVariables[];

#define xstr(s) #s
#define str(s) xstr(s)

nvram_variable_t gNvramVariables[] = {
    {"build-style",         str(BUILD_STYLE), 0},
    {"build-version",       str(BUILD_TAG), 0},
    {"config_board",        str(BUILD_PLATFORM), 0},
    {"boot-args",           "-v", 0},
    {"loadaddr",            str(DRAM_BASE), 0},
    {"bootdelay",           "0", 0},
    {"auto-boot",           "false", 0},
    {"idle-off",            "true", 0},
    {"boot-device",         "nand0a", 0},
    {"boot-partition",      "0", 0},
    {"boot-path",           "/System/Library/Caches/com.apple.kernelcaches/kernelcache", 0},
    {"display-color-space", "RGB888", 0},
    {"display-timing",      str(BUILD_PLATFORM), 0},
    {"framebuffer",         str(FRAMEBUFFER_ADDRESS), 0},
    {"secure-boot",         "0x0", 0},
    {"filesize", "0x0", 0},
    {"", "", 0xff},
};

int nvram_init(void)
{
    return 0;
}

int nvram_set_variable(char* env, char* string) {
    int i = 0;
    char* p;

    /* xxx */
    while((p = strchr(string, '"')) != NULL)
        *p = ' ';
    
    /* Set if in NvramVars. */
    while(gNvramVariables[i].overridden != 0xff) {
        if(strlen(env) > 64)
            env[63] = '\0';
        if(strncmp(env, gNvramVariables[i].name, 64) == 0) {
            bzero(gNvramVariables[i].setting, 256);
            strncpy(gNvramVariables[i].setting, string, 255);
            gNvramVariables[i].overridden = 1;
            return 0;
        }
        i++;
    }
    
    /* Todo, add the thing to a list if it's user specified, IE: platform-uuid */
    return 0;
}

char* nvram_get_variable(char* env) {
    int i = 0;
 
    /* Set if in gNvramVars */
    while(gNvramVariables[i].overridden != 0xff) {
        if(strlen(env) > 64)
            env[63] = '\0';
        if(strncmp(env, gNvramVariables[i].name, strlen(env)) == 0) {
            return gNvramVariables[i].setting;
        }
        i++;
    }
    
    /* Todo, add the thing to a list if it's user specified, IE: platform-uuid */
    return 0;
}

int command_setenv(int argc, char* argv[]) {
    if(argc != 2) {
        printf("usage: setenv <var> <string>\n");
        return -1;
    }
    nvram_set_variable(argv[1], argv[2]);
    return 0;
}

int command_getenv(int argc, char* argv[]) {
    if(argc != 1) {
        printf("usage: getenv <var>\n");
        return -1;
    }
    printf("%s\n", nvram_get_variable(argv[1]));
    return 0;
}

int command_printenv(int argc, char* argv[]) {
    if(argv[1]) {
        if(nvram_get_variable(argv[1]))
            printf("%s = '%s'\n", argv[1], nvram_get_variable(argv[1]));
        return 0;
    } else {
        // xxx todo, fix for other settings
        int i = 0;
        while(gNvramVariables[i].overridden != 0xff) {
            printf("%s %s = '%s'\n", gNvramVariables[i].overridden ? "P" : "", gNvramVariables[i].name, gNvramVariables[i].setting);
            i++;
        }
    }
    
    return 0;
}

