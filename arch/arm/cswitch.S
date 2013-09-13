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
/*
 * Thread Manager (context switching)
 */

#include "asm_help.h"

/**
 * Switch_context
 *
 * Switch the current execution context from one thread to another.
 * - r0 = new thread
 */
EnterARM(Switch_context)
    /* Get the old thread value. */
    mrc     p15, 0, r1, c13, c0, 4
    cmp     r1, r0
    beq     L_SwitchContextPanic

    /* Save old registers. */
    add     r2, r1, #0x10
    stmia   r2, {r4-lr}

    /* Load new registers. */
    add     r2, r0, #0x10
    ldmia   r2, {r4-lr}

    /* Set the new thread value. */
    mcr     p15, 0, r0, c13, c0, 4

    /* Set CurrentThread too. */
    ldr     r1, =CurrentThread
    str     r0, [r1]

    /* Continue. */
    bx      lr

L_SwitchContextPanic:
    mov     r2, r0
    adr     r0, _panicString
    blx     panic
_panicString:
    .asciz  "invalid thread in Switch_context, 0x%08x == 0x%08x."

/**
 * Processor_idle
 *
 * Idle loop.
 */
EnterARM(Processor_idle)
    wfi
    b       _Processor_idle

/**
 * arm_get_current_thread
 *
 * Return current thread.
 */
EnterARM(arm_get_current_thread)
    mrc     p15, 0, r0, c13, c0, 4
    bx      lr

/**
 * Call_continuation
 *
 * Call the continuation function.
 */
EnterARM(Call_continuation)
    mov     r0, r4
    mov     r1, r5
    blx     thread_continue
    b       . 
