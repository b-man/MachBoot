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

extern int uart_getchar(void);
extern void uart_putchar(int c);

static void putc_wrapper(void *p, char c)
{
    uart_putchar(c);
}

/**
 * init_debug
 *
 * Start debugging subsystems.
 */
void init_debug(void)
{
    init_printf(NULL, putc_wrapper);
    printf("debug_init()\n");
}

/**
 * __assert_func
 *
 * Debug function for failed assertions.
 */
void __assert_func(const char *file, int line, const char *method,
                   const char *expression)
{
    panic("Assertion '%s' failed in file %s, line %d.\n",
          expression, file, line);

    /* Halt */
    _locore_halt_system();

    while (1);
}

/**
 * panic
 *
 * Halt the system and explain why.
 */
#undef panic
extern int cpu_number(void);
void panic(const char *panicStr, ...)
{
    void *caller = __builtin_return_address(0);

    /* Prologue */
    printf("panic(cpu %d caller 0x%08x): ", cpu_number(), caller);

    /* Epilogue */
    va_list valist;
    va_start(valist, panicStr);
    vprintf((char *)panicStr, valist);
    va_end(valist);

    /* We are hanging here. */
    printf("\npanic: we are hanging here...\n");

    /* Halt */
    _locore_halt_system();
}
