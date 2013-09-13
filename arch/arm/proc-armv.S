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

#include "asm_help.h"

/**
 * _arm_get_fiq_status
 *
 * Get current FIQ status.
 */
EnterARM(arm_get_fiq_status)
    mov    r1, #0x40
    b      L_interrupt_test_join

/**
 * _arm_get_irq_status
 *
 * Get current IRQ status.
 */
EnterARM(arm_get_irq_status)
    mov    r1, #0x80
    b      L_interrupt_test_join

/**
 * _arm_get_interrupts_status
 *
 * Get current FIQ/IRQ statuses.
 */
EnterARM(arm_get_interrupts_status)
    mov    r1, #0xC0
    b      L_interrupt_test_join

L_interrupt_test_join:
    mrs    r0, cpsr
    tst    r0, r1
    moveq  r0, #1
    movne  r0, #0
    bx     lr

/**
 * _splhigh
 *
 * Set high priority level (interrupts disabled).
 */
EnterARM(splhigh)
    cpsid    if
    bx       lr

/**
 * _spllo
 *
 * Set low priority level (interrupts enabled).
 */
EnterARM(spllo)
    cpsie    if
    bx       lr

/*
 * Processor maintenance
 */

/**
 * _arm_flush_tlb
 *
 * Clear all TLB entries.
 */
EnterARM(arm_flush_tlb)
    mov      r0, #0
    mcr      p15, 0, r0, c8, c7, 0
    dsb      ish
    isb      sy
    bx       lr

/**
 * _arm_set_ttbr0
 *
 * Set current translation-table base register.
 */
EnterARM(arm_set_ttbr0)
    orr      r0, r0, #0x18
    mcr      p15, 0, r0, c2, c0, 0
    bx       lr

/**
 * _arm_set_ttbr1
 *
 * Set second translation-table base register.
 */
EnterARM(arm_set_ttbr1)
    orr      r0, r0, #0x18
    mcr      p15, 0, r0, c2, c0, 1
    bx       lr

/**
 * _arm_wfi_loop
 *
 * Deadloop.
 */
EnterARM(arm_wfi_loop)
    wfi
    b        _arm_wfi_loop

/**
 * _arm_mmu_start
 *
 * Start the MMU.
 */
EnterARM(arm_mmu_start)
    /* set DACR */
    mov      r0, #0xffffffff
    mcr      p15, 0, r0, c3, c0, 0
    isb      sy

    /* Enable MMU */
    mrc      p15, 0, r0, c1, c0, 0
    orr      r0, r0, #1
    mcr      p15, 0, r0, c1, c0, 0
    bx       lr
