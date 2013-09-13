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
 * Physical mapping memory manager (pmap).
 */

#include "genboot.h"
#include "pmap.h"

/**
 * pmap_map_section
 *
 * Bind ARM L1 section mappings for physical addresses 'PA' to virtual addresses 'VA'
 * in the specified 'tteb' translation-table-entry-base of size 'size'.
 */
void pmap_map_section(uint32_t pa, uint32_t va, uint32_t tteb, uint32_t size)
{
    uint32_t sect_iter = pa;
    uint32_t tte_pbase, tte_psize, tte;

    tte_pbase = addr_to_tte(tteb, va);
    tte_psize = ((size >> 0x14) << 2);

    /*
     * Create a section entry for every mapping.
     */
    for(tte = tte_pbase; tte < (tte_pbase + tte_psize); tte += sizeof(uint32_t)) {
        arm_l1_t *ttv = (arm_l1_t*)tte;

        /* Bits [31:20] */
        if(sect_iter & ~0xfff00000) {
            panic("mmu_map_section: misaligned L1 section mapping: 0x%08x\n", sect_iter);
        }

        ttv->ulong = sect_iter;                 /* address */
        ttv->l1.is_section = true;              /* its a section */
        ttv->l1.bufferable = true;              /* cacheability */
        ttv->l1.cacheable = true;
        ttv->l1.ap = 0x3;                       /* Full R+W */

        sect_iter += (1 * 1024 * 1024);     /* 1MB */
    }
    
    return;
}

/** 
 * mmu_init
 *
 * Map physical regions to virtual regions and enable everything.
 */
void mmu_init(uint32_t ram_start, uint32_t ram_size, uint32_t booter_start, uint32_t ttb)
{
    /* Map our booter. */
    pmap_map_section(0x0, 0x0, ttb, 0xFFFFFFFF);

    /* Map our booter to 0x0 also for exception vectors. */
    pmap_map_section(booter_start, 0x0, ttb, (1 * 1024 * 1024));

    /* Set TTBRs and do MMU start. */
    _arm_set_ttbr0(ttb);
    _arm_set_ttbr1(ttb);

    /* Flush TLB */
    _arm_flush_tlb();

    /* Enable MMU */
    printf("ttbr0: 0x%08x -- enabling MMU now\n", ttb);
    _arm_mmu_start();

    return;
}
