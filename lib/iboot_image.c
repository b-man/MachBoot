/*
 * From OpeniBoot.
 */

#include "genboot.h"
#include "stb_image.h"
#define RGBA2RVFB(x) ((((((x) >> 16) & 0xFF) >> 3) << 11)| (((((x) >> 8) & 0xFF) >> 2) << 5)| (((((x) >> 0) & 0xFF) >> 3) << 0));
#define RGBA2BGR(x) ((((x) >> 16) & 0xFF) | ((((x) >> 8) & 0xFF) << 8) | (((x) & 0xFF) << 16))
#define BGR16(x) ((((((x) >> 16) & 0xFF) >> 3) << 11) | (((((x) >> 8) & 0xFF) >> 2) << 5) | (((x) & 0xFF) >> 3))
#define BGR32(x) ((((((x) >> 11) & 0x1F) << 3) << 16) | (((((x) >> 5) & 0x3F) << 2) << 8) | (((x) & 0x1F) << 3))
#define RGB16(x) (((((x) & 0xFF) >> 3) << 11) | (((((x) >> 8) & 0xFF) >> 2) << 5) | ((((x) >> 16) & 0xFF) >> 3))

extern int isRgb888;
extern uint32_t gFbWidth, gFbHeight, gFbAddress;

uint32_t* framebuffer_load_image(const char* data, int len, int* width, int* height, int alpha) {
	int components;
	uint32_t* stbiData = (uint32_t*) stbi_load_from_memory((stbi_uc const*)data, len, width, height, &components, 4);
	if(!stbiData) {
		return NULL;
	}
	return stbiData;
}

inline volatile uint32_t* PixelFromCoords(register uint32_t x, register uint32_t y) {
	return (uint32_t*)(gFbAddress + (y * gFbWidth) + x);
}

inline volatile uint16_t* PixelFromCoords565(register uint32_t x, register uint32_t y) {
	return ((uint16_t*)gFbAddress) + (y * gFbWidth) + x;
}

/* Rgb565 only. */
static void framebuffer_draw_image565(uint32_t* image, int x, int y, int width, int height) {
	register uint32_t sx;
	register uint32_t sy;
	for(sy = 0; sy < height; sy++) {
		for(sx = 0; sx < width; sx++) {
			*(PixelFromCoords565(sx + x, sy + y)) = RGB16(image[(sy * width) + sx]);
		}
	}
}

/* Rgb888 only. */
void framebuffer_draw_image888(uint32_t* image, int x, int y, int width, int height) {
	register uint32_t sx;
	register uint32_t sy;
	for(sy = 0; sy < height; sy++) {
		for(sx = 0; sx < width; sx++) {
			*(PixelFromCoords(sx + x, sy + y)) = RGBA2BGR(image[(sy * width) + sx]);
		}
	}
}

void framebuffer_draw_image(uint32_t* image, int x, int y, int width, int height)
{
	if(isRgb888)
		framebuffer_draw_image888(image, x, y, width, height);
	else
		framebuffer_draw_image565(image, x, y, width, height);
}

void load_iboot_image(uint32_t *address, uint32_t tag)
{
	void* out_image;
	uint32_t out_size, decomp_length, image_tag;
	iboot_im_header_t* header;

	void *loadaddr = (void*)(PICTURE_LOADADDR);	/* Kill me. malloc() causes a stack smash. */

	if(!image3_fast_get_type((void*)address, &image_tag)) {
		return;
	}

	assert(image3_validate_image((void*)address));
	assert(image3_get_tag_data((void*)address, kImage3TagData, &out_image, &out_size));

	/* Decompress the LZSS image. */
	assert(out_image && out_size);
	header = (iboot_im_header_t*)out_image;

	/* See if it's valid. */
	if(strncmp((char*)out_image, kiBootImageSignature, sizeof(kiBootImageSignature))) {
		printf("load_iboot_image(): invalid signature\n");
		return;
	}

	/* Compute the final length of the image. */
	if(header->compression_type != kiBootImageLzss) {
		printf("load_iboot_image(): invalid image compression type\n");
		return;
	}

	/* Get it now. */
	if(header->format != kiBootImageArgb) {
		printf("load_iboot_image(): only 'argb' images are supported\n");
		return;
	}

	/* k. */
	decomp_length = decompress_lzss((loadaddr), (uint8_t*)((uintptr_t)out_image + sizeof(iboot_im_header_t)), out_size);

	printf("load_iboot_image(): loaded image at 0x%08x with width: %d, height: %d, decomp length: %d\n", loadaddr, header->width, header->height, decomp_length);
	
	/* todo: center it. */
	framebuffer_draw_image((uint32_t*)loadaddr, 0, 0, header->width, header->height);

	return;
}