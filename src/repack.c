#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "repack.h"

void
dng_repack_10bit(const uint8_t *src_buf, uint8_t *dst_buf, uint32_t width, uint32_t height, unsigned int so) {
	uint16_t pixels[4];
	uint64_t bits_per_width = (uint64_t) width * 10;
	uint64_t remainder = bits_per_width % 8;
	uint32_t stride;
	uint32_t padding_bytes = 0;
	if (remainder == 0) {
		stride = bits_per_width / 8;
	} else {
		stride = (bits_per_width + 8 - remainder) / 8;
		padding_bytes = 8 - remainder;
	}
	if (so > 0 ){
		padding_bytes = so - stride;
	}

	size_t si = 0;

	/*
	 * Repack 40 bits stored in sensor format into sequential format
	 *
	 * src_buf: 11111111 22222222 33333333 44444444 11223344 ...
	 * dst_buf: 11111111 11222222 22223333 33333344 44444444 ...
	 */
	for (size_t i = 0; i < stride * height; i += 5) {
		// Skip padding bytes in source buffer
		if (i && i % stride == 0)
			si += padding_bytes;

		/* Extract pixels from packed sensor format */
		pixels[0] = (src_buf[si + 0] << 2) | (src_buf[si + 4] >> 6);
		pixels[1] = (src_buf[si + 1] << 2) | (src_buf[si + 4] >> 4 & 0x03);
		pixels[2] = (src_buf[si + 2] << 2) | (src_buf[si + 4] >> 2 & 0x03);
		pixels[3] = (src_buf[si + 3] << 2) | (src_buf[si + 4] & 0x03);

		/* Pack pixels into sequential format */
		dst_buf[i + 0] = (pixels[0] >> 2 & 0xff);
		dst_buf[i + 1] = (pixels[0] << 6 & 0xff) | (pixels[1] >> 4 & 0x3f);
		dst_buf[i + 2] = (pixels[1] << 4 & 0xff) | (pixels[2] >> 6 & 0x0f);
		dst_buf[i + 3] = (pixels[2] << 2 & 0xff) | (pixels[3] >> 8 & 0x03);
		dst_buf[i + 4] = (pixels[3] & 0xff);

		si += 5;
	}
}

void
dng_repack_12bit(const uint8_t *src_buf, uint8_t *dst_buf, uint32_t width, uint32_t height, unsigned int so) {
	uint16_t pixels[4];
	uint64_t bits_per_width = (uint64_t) width * 12;
	uint64_t remainder = bits_per_width % 8;
	uint32_t stride;
	uint32_t padding_bytes = 0;
	if (remainder == 0) {
		stride = bits_per_width / 8;
	} else {
		stride = (bits_per_width + 8 - remainder) / 8;
		padding_bytes = 8 - remainder;
	}
	if (so > 0 ){
		padding_bytes = so - stride;
	}

	size_t si = 0;

	/*
	 * Repack 48 bits stored in sensor format into sequential format
	 *
	 * src_buf: 11111111 22222222 11112222 33333333 44444444 33334444 ...
	 * dst_buf: 11111111 11112222 22222222 33333333 33334444 44444444 ...
	 */
	for (size_t i = 0; i < stride * height; i += 6) {
		// Skip padding bytes in source buffer
		if (i && i % stride == 0)
			si += padding_bytes;

		/* Extract pixels from packed sensor format */
		pixels[0] = (src_buf[si + 0] << 4) | (src_buf[si + 2] >> 4);
		pixels[1] = (src_buf[si + 1] << 4) | (src_buf[si + 2] & 0xF);
		pixels[2] = (src_buf[si + 3] << 4) | (src_buf[si + 5] >> 4);
		pixels[3] = (src_buf[si + 4] << 4) | (src_buf[si + 5] & 0xF);

		/* Pack pixels into sequential format */
		// TODO: This is wrong, fix when there's actual testing data
		dst_buf[i + 0] = (pixels[0] >> 4 & 0xff);
		dst_buf[i + 1] = (pixels[0] << 4 & 0xff) | (pixels[1] >> 4 & 0xF);
		dst_buf[i + 2] = (pixels[1] >> 4 & 0xff) | (pixels[2] >> 6 & 0x0f);
		dst_buf[i + 3] = (pixels[2] << 2 & 0xff) | (pixels[3] >> 8 & 0x03);
		dst_buf[i + 4] = (pixels[3] & 0xff);
		dst_buf[i + 5] = (pixels[3] & 0xff);

		si += 6;
	}
}


void
dng_repack(const uint8_t *src_buf, uint8_t *dst_buf, uint32_t width, uint32_t height, int bitdepth, unsigned int stride) {
	switch (bitdepth) {
		case 10:
			return dng_repack_10bit(src_buf, dst_buf, width, height, stride);
		case 12:
			return dng_repack_12bit(src_buf, dst_buf, width, height, stride);
		default:
			fprintf(stderr, "dng_repack: invalid bitdepth %d\n", bitdepth);
			break;
	}
}