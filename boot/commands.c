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

int command_help(int argc, char* argv[]) {
    command_dispatch_t *current = &gDispatch[0];
    while(current->name != NULL) {
        printf("        %-16.16s %s\n", current->name, current->description);
        current++;
    }
    return 0;
}

int command_halt(int argc, char* argv[])
{
    _locore_halt_system();
    return 0;   /* Not reached. */
}

int command_go(int argc, char* argv[])
{
    uint32_t addr = 0;

    if(argc != 1) {
        printf("usage: %s <address>\n", argv[0]);
        return -1;
    }

    addr = strtoul(argv[1], NULL, 16);

    if(!permissions_range_check(addr)) {
        printf("Permission Denied\n");
        return -1;
    }

    printf("jumping into image at 0x%08x\n", addr);

    _locore_jump_to((void (**)(void *, uint32_t))addr, 0);

    return 0;
}

/* Memory read/write. */
int command_mws(int argc, char* argv[])
{
    uint32_t addr = 0;
    if(argc != 2) {
        printf("usage: %s [address] [data]\n", argv[0]);
        return -1;
    }
    addr = strtoul(argv[1], NULL, 16);

    if(!permissions_range_check(addr)) {
        printf("Permission Denied\n");
        return -1;
    }

    strcpy((char*)addr, argv[2]);
    return 0;
}

int command_mwb(int argc, char* argv[])
{
    uint32_t addr = 0;
    uint8_t data = 0;
    if(argc != 2) {
        printf("usage: %s [address] [data]\n", argv[0]);
        return -1;
    }
    addr = strtoul(argv[1], NULL, 16);

    if(!permissions_range_check(addr)) {
        printf("Permission Denied\n");
        return -1;
    }

    data = (uint8_t)strtoul(argv[2], NULL, 16);
    *((uint32_t*)addr) = data;
    return 0;
}

int command_mwh(int argc, char* argv[])
{
    uint32_t addr = 0;
    uint16_t data = 0;
    if(argc != 2) {
        printf("usage: %s [address] [data]\n", argv[0]);
        return -1;
    }
    addr = strtoul(argv[1], NULL, 16);

    if(!permissions_range_check(addr)) {
        printf("Permission Denied\n");
        return -1;
    }

    data = (uint16_t)strtoul(argv[2], NULL, 16);
    *((uint32_t*)addr) = data;
    return 0;
}

int command_mw(int argc, char* argv[])
{
    uint32_t addr = 0;
    uint32_t data = 0;
    if(argc != 2) {
        printf("usage: %s [address] [data]\n", argv[0]);
        return -1;
    }
    addr = strtoul(argv[1], NULL, 16);

    if(!permissions_range_check(addr)) {
        printf("Permission Denied\n");
        return -1;
    }

    data = (uint32_t)strtoul(argv[2], NULL, 16);
    *((uint32_t*)addr) = data;
    return 0;
}

int command_mdb(int argc, char* argv[])
{
    uint32_t addr = 0;
    if(argc != 1) {
        printf("usage: %s [address]\n", argv[0]);
        return -1;
    }
    addr = strtoul(argv[1], NULL, 16);

    if(!permissions_range_check(addr)) {
        printf("Permission Denied\n");
        return -1;
    }

    printf("*0x%08x = 0x%02x\n", addr, *((uint8_t*)addr));
    return 0;
}

int command_mdh(int argc, char* argv[])
{
    uint32_t addr = 0;
    if(argc != 1) {
        printf("usage: %s [address]\n", argv[0]);
        return -1;
    }
    addr = strtoul(argv[1], NULL, 16);

    if(!permissions_range_check(addr)) {
        printf("Permission Denied\n");
        return -1;
    }

    printf("*0x%08x = 0x%04x\n", addr, *((uint16_t*)addr));
    return 0;
}

int command_md(int argc, char* argv[])
{
    uint32_t addr = 0;
    if(argc != 1) {
        printf("usage: %s [address]\n", argv[0]);
        return -1;
    }
    addr = strtoul(argv[1], NULL, 16);

    if(!permissions_range_check(addr)) {
        printf("Permission Denied\n");
        return -1;
    }

    printf("*0x%08x = 0x%08x\n", addr, *((uint32_t*)addr));
    return 0;
}

/* Hashing Memory Functions. */
int command_crc(int argc, char* argv[])
{
    uint32_t addr = 0, len = 0;
    if(argc != 2) {
        printf("not enough arguments\n"
               "usage: crc <address> <len>\n");
        return -1;
    }
    addr = strtoul(argv[1], NULL, 16);
    len = strtoul(argv[2], NULL, 10);

    if(!permissions_range_check(addr)) {
        printf("Permission Denied\n");
        return -1;
    }

    printf("CRC32(0x%08x, %d) = 0x%08x\n", addr, len, crc32(0, (void*)addr, len));
    return 0;
}

int command_sha1(int argc, char* argv[])
{
    SHA1_CTX ctx;
    unsigned char hash[20];
    int i;
    uint32_t addr = 0, len = 0;

    if(argc != 2) {
        printf("not enough arguments\n"
               "usage: sha1 <address> <len>\n");
        return -1;
    }
    addr = strtoul(argv[1], NULL, 16);
    len = strtoul(argv[2], NULL, 10);

    if(!permissions_range_check(addr)) {
        printf("Permission Denied\n");
        return -1;
    }

    SHA1Init(&ctx);
    SHA1Update(&ctx, (uint8_t*)(addr), len);
    SHA1Final(hash, &ctx);

    printf("SHA1(0x%08x, %d) = ", addr, len);
    for(i=0;i<20;i++)
        printf("%02x", hash[i]);
    printf("\n");
    return 0;
}

/* Setpicture */
int command_setpicture(int argc, char* argv[])
{
    uint32_t addr = LOADADDR;
    if(argc > 2) {
        printf("usage: %s [address]\n", argv[0]);
        return -1;
    }
    if(argv[1])
        addr = strtoul(argv[1], NULL, 16);

    if(!permissions_range_check(addr)) {
        printf("Permission Denied\n");
        return -1;
    }

    load_iboot_image((uint32_t*)addr, 'recm');
    return 0;
}

/* Bootx. */
void *gKernelImage = NULL, *gDeviceTreeImage = NULL;
uint32_t gKernelSize;

int command_bootx(int argc, char* argv[]) {
    uint32_t decomp_addr = KERNELCACHE_LOADADDR, comp_addr = LOADADDR;
    uint32_t image_tag, out_size, kernel_image;

    if(argv[1])
        comp_addr = strtoul(argv[1], NULL, 16);

    if(!permissions_range_check(comp_addr)) {
        printf("Permission Denied\n");
        return -1;
    }
    
    printf("loading kernelcache from %p to address %p\n", comp_addr, decomp_addr);

    if(!image3_fast_get_type((void*)comp_addr, &image_tag)) {
        return -1;
    }

    if(image_tag != kImage3TypeKernel)
        return -1;

    assert(image3_validate_image((void*)comp_addr));
    assert(image3_get_tag_data((void*)comp_addr, kImage3TagData, (void*)&kernel_image, &out_size));
    
    if(decompress_kernel((void*)kernel_image, (void*)decomp_addr) != 0) {
        printf("Failed to decompress kernelcache\n");
        return -1;
    }

    if(!gDeviceTreeImage) {
        printf("Invalid device tree\n");
        return -1;
    }

    start_darwin();

    return 0;
}

/* DeviceTree/Ramdisk */
int command_devicetree(int argc, char* argv[]) {
    uint32_t devicetree_addr = LOADADDR, image_tag;
    uint32_t devicetree_image, out_size;

    if(argv[1])
        devicetree_addr = strtoul(argv[1], NULL, 16);

    if(!permissions_range_check(devicetree_addr)) {
        printf("Permission Denied\n");
        return -1;
    }
    
    if(!image3_fast_get_type((void*)devicetree_addr, &image_tag)) {
        return -1;
    }

    if(image_tag != kImage3TypeXmlDeviceTree)
        return -1;

    assert(image3_validate_image((void*)devicetree_addr));
    assert(image3_get_tag_data((void*)devicetree_addr, kImage3TagData, (void*)&devicetree_image, &out_size));
    
    /* Copy it. */
    printf("creating device tree at 0x%x of size 0x%x, from image at 0x%x\n",
           DEVICETREE_LOADADDR, out_size, devicetree_addr);
    bcopy((void*)devicetree_addr, (void*)DEVICETREE_LOADADDR, out_size);

    gDeviceTreeImage = (void*)DEVICETREE_LOADADDR;
    
    return 0;
}


/* Command dispatch. */
command_dispatch_t gDispatch[] = {
    {"help", command_help, "this list"},
    {"go", command_go, "jump directly to address"},
    {"devicetree", command_devicetree, "create a device tree from the specified address"},
    {"bootx", command_bootx, "boot a kernel cache at a specified address"},
    {"halt", command_halt, "halt the system (good for JTAG)"},
    {"bgcolor", command_bgcolor, "set the display background color"},
    {"setpicture", command_setpicture, "set the image on the display"},
    {"memorytester", command_memorytest, "test a memory region"},
    {"mws", command_mws, "memory write - string"},
    {"mwb", command_mwb, "memory write - 8bit"},
    {"mwh", command_mwh, "memory write - 16bit"},
    {"mw", command_mw, "memory write - 32bit"},
    {"mdb", command_mdb, "memory display - 8bit"},
    {"mdh", command_mdh, "memory display - 16bit"},
    {"md", command_md, "memory display - 32bit"},
    {"sha1", command_sha1, "SHA-1 hash of memory"},
    {"crc", command_crc, "POSIX 1003.2 checksum of memory"},
    {"setenv", command_setenv, "set an environment variable"},
    {"getenv", command_getenv, "get an environment variable over console"},
    {"printenv", command_printenv, "print one or all environment variables"},    
    {NULL, NULL, NULL},
};

