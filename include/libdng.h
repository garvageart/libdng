#ifndef LIBDNG_LIBRARY_H
#define LIBDNG_LIBRARY_H

#include <limits.h>
#include <time.h>
#include <stdint.h>

#define EXPORT __attribute__((__visibility__("default")))

typedef struct {
		char *camera_make;
		char *camera_model;
		char *unique_camera_model;
		char *software;
		uint16_t orientation;
		struct tm datetime;

		// Raw image data
		uint16_t bayer_pattern_dimensions[2];
		float colormatrix1[9];
		float colormatrix2[9];
		float neutral[3];
		uint8_t cfapattern[4];
} libdng_info;

EXPORT int
libdng_init();

EXPORT void
libdng_new(libdng_info *dng);

EXPORT void
libdng_free(libdng_info *dng);

EXPORT int
libdng_write(libdng_info *dng, const char *path, unsigned int width, unsigned int height, uint8_t *data,
	size_t length);

#endif //LIBDNG_LIBRARY_H
