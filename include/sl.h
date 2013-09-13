/*
 * Copyright (c) 2003 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 2.0 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

#ifndef _SL_H_
#define _SL_H_

#include <stdint.h>
#include <sys/types.h>
#include "linux_atags.h"

/* HFS. */
#define SWAP_BE16(x)  OSSwapBigToHostInt16(x)
#define SWAP_LE16(x)  OSSwapLittleToHostInt16(x)
#define SWAP_BE32(x)  OSSwapBigToHostInt32(x)
#define SWAP_LE32(x)  OSSwapLittleToHostInt32(x)
#define SWAP_BE64(x)  OSSwapBigToHostInt64(x)
#define SWAP_LE64(x)  OSSwapLittleToHostInt64(x)

extern void utf_encodestr( const u_int16_t * ucsp, int ucslen, u_int8_t * utf8p, u_int32_t bufsize, int byte_order );
extern void utf_decodestr(const u_int8_t * utf8p, u_int16_t * ucsp, u_int16_t * ucslen, u_int32_t bufsize, int byte_order);

typedef uint32_t UInt32;
typedef uint16_t UInt16;
typedef uint8_t UInt8;
extern void* gFSLoadAddress;
extern UInt32 kLoadSize;

typedef void* CICell;

extern int Read(CICell ih, long addr, long length);
extern void Seek(CICell ih, long long position);

typedef struct FinderInfo {
    unsigned char data[16];
} FinderInfo;

// File Permissions and Types
enum {
    kPermOtherExecute  = 1 << 0,
    kPermOtherWrite    = 1 << 1,
    kPermOtherRead     = 1 << 2,
    kPermGroupExecute  = 1 << 3,
    kPermGroupWrite    = 1 << 4,
    kPermGroupRead     = 1 << 5,
    kPermOwnerExecute  = 1 << 6,
    kPermOwnerWrite    = 1 << 7,
    kPermOwnerRead     = 1 << 8,
    kPermMask          = 0x1FF,
    kOwnerNotRoot      = 1 << 9,
    kFileTypeUnknown   = 0x0 << 16,
    kFileTypeFlat      = 0x1 << 16,
    kFileTypeDirectory = 0x2 << 16,
    kFileTypeLink      = 0x3 << 16,
    kFileTypeMask      = 0x3 << 16
};


extern void CacheInit( CICell ih, long blockSize );
extern long CacheRead( CICell ih, char * buffer, long long offset,
                long length, long cache );

#endif