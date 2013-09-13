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

#ifndef _IBOOTEMUL_H_
#define _IBOOTEMUL_H_

/* Other stuff. */
extern char gUsbString[256];
extern uint32_t gPermissions, gLowLimit, gHighLimit;

/* Cool stuff. */
typedef int (*command_function_t)(int argc, char* argv[]);

typedef struct _command_dispatch {
    char*                   name;
    command_function_t      function;
    char*                   description;
} command_dispatch_t;

extern command_dispatch_t gDispatch[];

typedef enum {
    PERM_CAN_LOAD_IMAGE = 0x1,
    PERM_CAN_LOAD_KERNEL = 0x2,
    PERM_CAN_DO_ANYTHING = 0x8000000
} kPermissions;

/* Permissions. */
extern void permissions_init(void);
extern bool permissions_check(uint32_t flag);
extern bool permissions_range_check(uint32_t address);

/* Commands. */
extern int command_setenv(int argc, char* argv[]);
extern int command_getenv(int argc, char* argv[]);
extern int command_printenv(int argc, char* argv[]);
extern int command_bgcolor(int argc, char* argv[]);
extern int command_halt(int argc, char* argv[]);
extern int command_memorytest(int argc, char **argv);
extern void command_prompt(void);

/* Nvram. */
extern int nvram_init(void);
extern int nvram_set_variable(char* env, char* string);
extern char* nvram_get_variable(char* env);

#endif