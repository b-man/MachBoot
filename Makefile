BUILD_TAG			= MachBoot-1.1 #$(shell git tag -l | tail -n 1)
BUILD_STYLE			= DEVELOPMENT
BUILD_PRODUCT		= MachBoot
BUILD_PLATFORM		= RealView
TEXT_BASE			= 0x8ff00000
FRAMEBUFFER_ADDRESS = 0x8f700000
DRAM_BASE			= 0x70000000
LOADADDR			= 0x70000000
RAMDISK_LOADADDR 		= 0x73C00000 # +60MB
KERNELCACHE_LOADADDR 	= 0x72200000 # +34MB
DEVICETREE_LOADADDR 	= 0x72100000 # +33MB
PICTURE_LOADADDR 		= 0x72000000 # +32MB
DRAM_SIZE			= 0x20000000 # 512MB DRAM

export SRCROOT = $(shell /bin/pwd)
export OBJROOT = $(SRCROOT)/Build/Objects
export DSTROOT = $(SRCROOT)/Build/Destination
export SYMROOT = $(SRCROOT)/Build/Symbols

OBJECTS		= \
	arch/arm/start.o arch/arm/excvect.o arch/arm/semihost.o \
	arch/arm/proc-armv.o arch/arm/traps.o arch/arm/cswitch.o arch/arm/crt/bcopy.o \
	arch/arm/crt/bzero.o arch/arm/crt/strchr.o arch/arm/crt/strcmp.o \
	arch/arm/crt/strlen.o arch/arm/crt/strncmp.o arch/arm/pmap.o \
	arch/arm/thread.o arch/arm/crt/memset_pattern.o boot/bootx.o boot/main.o \
	drivers/rv_uart.o lib/core/malloc.o lib/core/printf.o lib/core/tlsf.o \
	lib/crt/libc_stub.o lib/device_tree.o lib/image3.o lib/json_parser.o lib/macho_loader.o \
	lib/xml.o sys/debug.o sys/version.o sys/memory_region.o lib/lzss.o lib/adler32.o \
	boot/permissions.o boot/commands.o boot/nvram.o boot/parser.o drivers/rv_framebuffer.o \
	drivers/rv_init.o lib/memorytester/mt.o lib/memorytester/tests.o lib/crc32.o lib/crypto/sha1.o \
	lib/iboot_image.o lib/stb_image.o lib/core/stack_protector.o \
	lib/hfs/cache.o lib/hfs/hfs.o lib/hfs/hfs_compare.o lib/hfs/sys.o lib/hfssup.o \
	lib/kernelcache.o \
	mach.o xmdt.o

CFLAGS		= -mcpu=cortex-a8 -std=c99 -fno-builtin -Os -fPIC -Wall -Werror -Wno-error=multichar -Wno-multichar -Wno-error=unused-function -mapcs-frame \
		-fstack-protector-all -Wno-error=strict-aliasing
CPPFLAGS	= -Iinclude -D__LITTLE_ENDIAN__ -DTEXT_BASE=$(TEXT_BASE) -DBUILD_STYLE="$(BUILD_STYLE)" \
		  -DBUILD_TAG="$(BUILD_TAG)" -DBUILD_PRODUCT="$(BUILD_PRODUCT)" -DBUILD_PLATFORM="$(BUILD_PLATFORM)" \
		  -Iarch/arm/include -DDRAM_BASE=$(DRAM_BASE) -DDRAM_SIZE=$(DRAM_SIZE) -D__arm__ -DARM \
		  -DFRAMEBUFFER_ADDRESS=$(FRAMEBUFFER_ADDRESS) -DLOADADDR=$(LOADADDR) -DKERNELCACHE_LOADADDR=$(KERNELCACHE_LOADADDR) \
		  -DDEVICETREE_LOADADDR=$(DEVICETREE_LOADADDR) -DPICTURE_LOADADDR=$(PICTURE_LOADADDR) \
		  -DRAMDISK_LOADADDR=$(RAMDISK_LOADADDR)
ASFLAGS		= -mcpu=cortex-a8 -DTEXT_BASE=$(TEXT_BASE) -D__ASSEMBLY__
LDFLAGS		= -nostdlib -Wl,-Tldscript.ld
CROSS		= arm-none-eabi-
CC		= $(CROSS)gcc
AS		= $(CROSS)gcc
LD		= $(CROSS)ld
OBJCOPY		= $(CROSS)objcopy
TARGET		= SampleBooter.elf

SIZE		= 32768

all: dirs $(TARGET) $(OBJECTS)

mach.o: blobs/mach.img3
	$(LD) -r -b binary -o mach.o blobs/mach.img3
	$(OBJCOPY) --rename-section .data=.kernel mach.o mach.o

xmdt.o: blobs/xmdt.img3
	$(LD) -r -b binary -o xmdt.o blobs/xmdt.img3
	$(OBJCOPY) --rename-section .data=.devicetree xmdt.o xmdt.o

.PHONY: dirs
dirs:
#	mkdir -p $(OBJROOT) $(SYMROOT) $(DSTROOT)

$(TARGET): $(OBJECTS) mach.o xmdt.o
	scripts/version.sh lib/version.S $(BUILD_PRODUCT) $(BUILD_PLATFORM) $(BUILD_STYLE) $(BUILD_TAG)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o lib/version.o lib/version.S
	$(CC) $(LDFLAGS) $(OBJECTS) lib/version.o -o $(TARGET)  -lgcc 
	$(OBJCOPY) -g -S -O binary $(TARGET) $(TARGET).raw
	mkimage -A arm -O linux -T kernel -C none -a $(TEXT_BASE) -e $(TEXT_BASE) -n "$(BUILD_TAG)" -d $(TARGET).raw $(TARGET).uImage
#	rm -f $(TARGET) $(TARGET).raw

%.o: %.S
	$(CC) $(CPPFLAGS) $(CFLAGS) $(ASFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET)* $(OBJECTS) $(OBJROOT)/version.o mach.o xmdt.o
