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

#define xstr(s) #s
#define str(s) xstr(s)

nvram_variable_t gNvramDefaultVariables[] = {
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

struct nvram_variable_node {
    char *name;
    char *setting;
    int overridden;
    struct nvram_variable_node *nextvar;
};

typedef struct nvram_variable_node nvram_variable_node_t;

nvram_variable_node_t *gNvramVariables;

int nvram_init(nvram_variable_t *vars, size_t size) {
    int step = 0;
    nvram_variable_node_t *head = NULL;

    while (step < size) {
        nvram_variable_t *var = &vars[step];
        gNvramVariables = (nvram_variable_node_t *)malloc(sizeof(nvram_variable_node_t));
        gNvramVariables->name = var->name;
        gNvramVariables->setting = var->setting;
        gNvramVariables->overridden = var->overridden;
        gNvramVariables->nextvar = head;
        head = gNvramVariables;
        step++;
    }

    gNvramVariables = head;

    return 0;
}

size_t nvram_get_variable_list_size(void)
{
	size_t size = 0;
	nvram_variable_node_t *var = gNvramVariables;

	while(var != NULL) {
		size++;
		var = var->nextvar;
	}

	return size;
}

int nvram_set_variable(const char *env, const char *setting) {
    int step = 0;
    nvram_variable_node_t *var = gNvramVariables;

    while (step < nvram_get_variable_list_size()) {
        if (strncmp(env, var->name, strlen(env)) == 0) {
            bzero(var->setting, 255);
            strncpy(var->setting, setting, 255);
            var->overridden = 1;
            return 0;
        }

        var = var->nextvar;
        step++;
    }

    nvram_variable_node_t *newvar = (nvram_variable_node_t *)malloc(sizeof(nvram_variable_node_t));
    newvar->name = (char *)env;
    newvar->setting = (char *)setting;
    newvar->overridden = 0;
    newvar->nextvar = gNvramVariables;
    gNvramVariables = newvar;

    return 0;
}

const char *nvram_get_variable(const char *env) {
    int step = 0;
    nvram_variable_node_t *var = gNvramVariables;

    while (step < nvram_get_variable_list_size()) {
        if (strncmp(env, var->name, strlen(env)) == 0)
            return var->setting;

        var = var->nextvar;
        step++;
    }

    return NULL;
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
    nvram_variable_node_t *vars = gNvramVariables;

    if(argv[1]) {
        if(nvram_get_variable(argv[1]))
            printf("%s = '%s'\n", argv[1], nvram_get_variable(argv[1]));
        return 0;
    } else {
        while(vars != NULL) {
            printf("%s %s = '%s'\n", vars->overridden ? "P" : "", vars->name, vars->setting);
            vars = vars->nextvar;
        }
    }

    return 0;
}

