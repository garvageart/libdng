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

#define LIBDNG_ORIENTATION_TOPLEFT 1
#define LIBDNG_ORIENTATION_TOPRIGHT 2
#define LIBDNG_ORIENTATION_BOTRIGHT 3
#define LIBDNG_ORIENTATION_BOTLEFT 4
#define LIBDNG_ORIENTATION_LEFTTOP 5
#define LIBDNG_ORIENTATION_RIGHTTOP 6
#define LIBDNG_ORIENTATION_RIGHTBOT 7
#define LIBDNG_ORIENTATION_LEFTBOT 8

EXPORT int
libdng_init();

EXPORT void
libdng_new(libdng_info *dng);

EXPORT void
libdng_free(libdng_info *dng);

EXPORT int
libdng_set_mode_from_name(libdng_info *dng, const char *name);

EXPORT int
libdng_set_mode_from_pixfmt(libdng_info *dng, uint32_t pixfmt);

EXPORT int
libdng_set_make_model(libdng_info *dng, const char *make, const char *model);

EXPORT int
libdng_set_software(libdng_info *dng, const char *software);

EXPORT int
libdng_set_datetime(libdng_info *dng, struct tm time);

EXPORT int
libdng_set_datetime_now(libdng_info *dng);

EXPORT int
libdng_set_orientation(libdng_info *dng, uint16_t orientation);

EXPORT int
libdng_write(libdng_info *dng, const char *path, unsigned int width, unsigned int height, const uint8_t *data,
	size_t length);

#endif //LIBDNG_LIBRARY_H
