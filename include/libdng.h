#ifndef LIBDNG_LIBRARY_H
#define LIBDNG_LIBRARY_H

#include <limits.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

#define EXPORT __attribute__((__visibility__("default")))

typedef struct {
		char *camera_make;
		char *camera_model;
		char *unique_camera_model;
		char *software;
		uint16_t orientation;
		struct tm datetime;
		uint16_t exposure_program;
		float exposure_time;
		uint32_t iso;
		float fnumber;
		float focal_length;
		float crop_factor;
		float frame_rate;

		// Raw image data
		uint16_t bayer_pattern_dimensions[2];
		float neutral[3];
		float analogbalance[3];
		uint8_t cfapattern[4];
		uint32_t whitelevel;

		// Calibration data
		float color_matrix_1[9];
		float color_matrix_2[9];
		float forward_matrix_1[9];
		float forward_matrix_2[9];
		unsigned short illuminant_1;
		unsigned short illuminant_2;
		unsigned int hue_sat_map_dims[3];
		size_t tone_curve_length;
		float *tone_curve;
		float *hue_sat_map_data_1;
		float *hue_sat_map_data_2;

		uint16_t bit_depth;
		bool needs_repack;
		uint32_t width;
		uint32_t height;

		float distortion_a;
		float distortion_b;
		float distortion_c;
		float vignette_k1;
		float vignette_k2;
		float vignette_k3;

		// v4l2 bytesperline in case it's weird
		unsigned int stride;
} libdng_info;

#define LIBDNG_ORIENTATION_TOPLEFT 1
#define LIBDNG_ORIENTATION_TOPRIGHT 2
#define LIBDNG_ORIENTATION_BOTRIGHT 3
#define LIBDNG_ORIENTATION_BOTLEFT 4
#define LIBDNG_ORIENTATION_LEFTTOP 5
#define LIBDNG_ORIENTATION_RIGHTTOP 6
#define LIBDNG_ORIENTATION_RIGHTBOT 7
#define LIBDNG_ORIENTATION_LEFTBOT 8

#define LIBDNG_EXPOSUREPROGRAM_UNDEFINED 0
#define LIBDNG_EXPOSUREPROGRAM_MANUAL 1
#define LIBDNG_EXPOSUREPROGRAM_NORMAL 2
#define LIBDNG_EXPOSUREPROGRAM_APERTURE_PRIORITY 3
#define LIBDNG_EXPOSUREPROGRAM_SHUTTER_PRIORITY 4
#define LIBDNG_EXPOSUREPROGRAM_CREATIVE 5
#define LIBDNG_EXPOSUREPROGRAM_ACTION 6
#define LIBDNG_EXPOSUREPROGRAM_PORTRAIT 7
#define LIBDNG_EXPOSUREPROGRAM_LANDSCAPE 8

#define LIBDNG_ILLUMINANT_UNKNOWN 0
#define LIBDNG_ILLUMINANT_DAYLIGHT 1
#define LIBDNG_ILLUMINANT_FLUORESCENT 2
#define LIBDNG_ILLUMINANT_TUNGSTEN 3
#define LIBDNG_ILLUMINANT_FLASH 4
#define LIBDNG_ILLUMINANT_FINE_WEATHER 9
#define LIBDNG_ILLUMINANT_CLOUDY_WEATHER 10
#define LIBDNG_ILLUMINANT_SHADE 11
#define LIBDNG_ILLUMINANT_DAYLIGHT_FLUORESCENT 12
#define LIBDNG_ILLUMINANT_DAY_WHITE_FLUORESCENT 13
#define LIBDNG_ILLUMINANT_COOL_WHITE_FLUORESCENT 14
#define LIBDNG_ILLUMINANT_WHITE_FLUORESCENT 15
#define LIBDNG_ILLUMINANT_STANDARD_A 17
#define LIBDNG_ILLUMINANT_STANDARD_B 18
#define LIBDNG_ILLUMINANT_STANDARD_C 19
#define LIBDNG_ILLUMINANT_D55 20
#define LIBDNG_ILLUMINANT_D65 21
#define LIBDNG_ILLUMINANT_D75 22
#define LIBDNG_ILLUMINANT_D50 23
#define LIBDNG_ILLUMINANT_ISO_TUNGSTEN 24
#define LIBDNG_ILLUMINANT_OTHER 255

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
libdng_set_neutral(libdng_info *dng, float red, float green, float blue);

EXPORT int
libdng_set_analog_balance(libdng_info *dng, float red, float green, float blue);

EXPORT int
libdng_load_calibration_file(libdng_info *dng, const char *path);

EXPORT int
libdng_set_exposure_program(libdng_info *dng, uint16_t mode);

EXPORT int
libdng_set_exposure_time(libdng_info *dng, float seconds);

EXPORT int
libdng_set_iso(libdng_info *dng, uint32_t isospeed);

EXPORT int
libdng_set_fnumber(libdng_info *dng, float fnumber);

EXPORT int
libdng_set_focal_length(libdng_info *dng, float focal_length, float crop_factor);

EXPORT int
libdng_set_frame_rate(libdng_info *dng, float framerate);

EXPORT int
libdng_set_distortion(libdng_info *dng, float a, float b, float c);

EXPORT int
libdng_set_vignette(libdng_info *dng, float k1, float k2, float k3);

EXPORT int
libdng_set_color_matrix_1(libdng_info *dng, float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8, float v9);

EXPORT int
libdng_set_color_matrix_2(libdng_info *dng, float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8, float v9);

EXPORT int
libdng_set_forward_matrix_1(libdng_info *dng, float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8, float v9);

EXPORT int
libdng_set_forward_matrix_2(libdng_info *dng, float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8, float v9);

EXPORT int
libdng_set_stride(libdng_info *dng, unsigned int stride);

EXPORT int
libdng_write(libdng_info *dng, const char *path, unsigned int width, unsigned int height, const uint8_t *data,
	size_t length);

EXPORT int
libdng_write_with_thumbnail(libdng_info *dng, const char *path, unsigned int width, unsigned int height,
	const uint8_t *data,
	size_t length, unsigned int thumb_width, unsigned int thumb_height, const uint8_t *thumb, size_t thumb_length);

EXPORT int
libdng_read(libdng_info *dng, const char *path);

EXPORT int
libdng_read_image(libdng_info *dng, const char *path, uint8_t index, uint8_t **data, size_t *length, uint32_t *width,
	uint32_t *height);

#endif //LIBDNG_LIBRARY_H
