#include <stdint.h>
#include <stdbool.h>

#pragma once

#define CFA_NONE 0
#define CFA_BGGR '\002\001\001\000'
#define CFA_GBRG '\001\002\000\001'
#define CFA_GRBG '\001\000\002\001'
#define CFA_RGGB '\000\001\001\002'


struct pixelformat {
		char *fourcc;
		char *name;
		uint32_t pixfmt;
		uint32_t cfa;
		int bits_per_sample;
		bool repack;
};

int
dng_mode_from_name(const char *name);

int
dng_mode_from_pixfmt(uint32_t pixfmt);

uint32_t
dng_cfa_from_mode(int index);

int
dng_bitdepth_from_mode(int index);

bool
dng_mode_needs_repack(int index);