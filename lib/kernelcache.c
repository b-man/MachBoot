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

extern void *gKernelImage;
extern uint32_t gKernelSize;

int decompress_kernel(void* compressedKernel, void* decompressedKernel)
{
    compressed_kernel_header *kernel_header = (compressed_kernel_header*)compressedKernel;
    u_int32_t uncompressed_size, size;
    
    printf("decompress_kernel: compressed kernel at %p\n", compressedKernel);
    
    if(kernel_header->signature != __builtin_bswap32('comp')) {
        printf("decompress_kernel: bad file magic\n");
        return -1;
    }
    
    if (kernel_header->signature == __builtin_bswap32('comp')) {
        if (kernel_header->compress_type != __builtin_bswap32('lzss')) {
            printf("decompress_kernel: kernel compression is bad\n");
            return -1;
        }
    }
    
    assert(decompressedKernel != NULL);
    
    uncompressed_size = __builtin_bswap32(kernel_header->uncompressed_size);
    
    size = decompress_lzss((u_int8_t *)decompressedKernel, &kernel_header->data[0],
                               __builtin_bswap32(kernel_header->compressed_size));

    if (uncompressed_size != size) {
        printf("decompress_kernel: size mismatch from lzss: %x\n", size);
        return -1;
    }
    
    if (__builtin_bswap32(kernel_header->adler32) !=
        Adler32(decompressedKernel, uncompressed_size)) {
        printf("decompress_kernel: adler mismatch\n");
        return -1;
    }
    
    printf("decompress_kernel: kernelcache decompressed to %p\n", decompressedKernel);
    gKernelSize = size;
    gKernelImage = (void*)decompressedKernel;    
    return 0;
}