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

#ifndef _PMAP_H_
#define _PMAP_H_

#include "genboot.h"

/*
 * ARM physical/virtual address types.
 */
typedef uint32_t paddr_t, vaddr_t;

/*
 * Definition of ARM page entries (v6 compatible).
 */
typedef struct arm_l1_entry_t {
    uint32_t    is_coarse:1;        /* Is it a coarse page/section descriptor? */
    uint32_t    is_section:1;
    uint32_t    bufferable:1;       /* Zero on coarse. */
    uint32_t    cacheable:1;        /* Zero on coarse. */
    uint32_t    sbz:1;              /* Should be zero. */
    uint32_t    domain:4;           /* Domain entry */
    uint32_t    ecc:1;              /* P-bit */
    uint32_t    ap:2;
    uint32_t    tex:3;
    uint32_t    apx:1;
    uint32_t    shareable:1;
    uint32_t    non_global:1;
    uint32_t    sbz2:2;
    uint32_t    pfn:12;
} arm_l1_entry_t;

typedef struct arm_l2_entry_t {
    uint32_t    nx:1;               /* 1 on 64kB pages, not supported. */
    uint32_t    valid:1;            /* 0 on 64kB pages, not supported. */
    uint32_t    bufferable:1;
    uint32_t    cacheable:1;
    uint32_t    ap:2;
    uint32_t    tex:3;
    uint32_t    apx:1;
    uint32_t    shareable:1;
    uint32_t    non_global:1;
    uint32_t    pfn:20;
} arm_l2_entry_t;

typedef struct arm_l1_t {
    union {
        arm_l1_entry_t l1;
        uint32_t ulong;
    };
} arm_l1_t;

typedef struct arm_l2_t {
    union {
        arm_l2_entry_t l2;
        uint32_t ulong;
    };
} arm_l2_t;

#define tte_offset(addr)            (((addr >> 0x14) & 0xfff) << 2)
#define addr_to_tte(base, addr)     (base + tte_offset(addr))

extern void _arm_flush_tlb(void);
extern void _arm_mmu_start(void);
extern void _arm_set_ttbr0(uint32_t ttb);
extern void _arm_set_ttbr1(uint32_t ttb);

#endif
