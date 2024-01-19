#include <stdbool.h>
#include <tiffio.h>
#include "greatest.h"
#include "libdng.h"

static enum greatest_test_res
check_str_tag(TIFF *im, uint32_t tag, const char *name, const char *expected)
{
	char *temp;
	if (TIFFGetField(im, tag, &temp) != 1) {
			FAILm(name);
	}
		ASSERT_STR_EQm(name, expected, temp);
		PASS();
}

static enum greatest_test_res
check_int_tag(TIFF *im, uint32_t tag, const char *name, int expected)
{
	uint32_t temp = 0;
	if (TIFFGetField(im, tag, &temp) != 1) {
			FAILm(name);
	}
		ASSERT_EQ_FMTm(name, expected, temp, "%d");
		PASS();
}

static enum greatest_test_res
check_float_tag(TIFF *im, uint32_t tag, const char *name, float expected)
{
	float temp;
	if (TIFFGetField(im, tag, &temp) != 1) {
			FAILm(name);
	}
		ASSERT_EQ_FMTm(name, expected, temp, "%f");
		PASS();
}


TEST generate_simple_dng(void)
{
	libdng_info info = {0};
	libdng_new(&info);
		ASSERT_EQm("Set mode", 1, libdng_set_mode_from_name(&info, "RGGB"));
		ASSERT_EQm("Set make", 1, libdng_set_make_model(&info, "Make", "Model"));
		ASSERT_EQm("Set software", 1, libdng_set_software(&info, "Software"));
		ASSERT_EQm("Set orientation", 1, libdng_set_orientation(&info, 4));
		ASSERT_EQm("Set exposuretime", 1, libdng_set_exposure_time(&info, 0.04f));
		ASSERT_EQm("Set fnumber", 1, libdng_set_fnumber(&info, 2.8f));
		ASSERT_EQm("Set fnumber", 1, libdng_set_focal_length(&info, 50.0f, 1.5f));
	uint8_t *data = malloc(1280 * 720);
		ASSERT_EQm("Write DNG", 1, libdng_write(&info, "test.dng", 1280, 720, data, 1280 * 720));
	free(data);
	libdng_free(&info);

	// Use LibTIFF directly to verify results
	TIFF *im = TIFFOpen("test.dng", "r");
	if (im == NULL) {
			FAILm("Could not open result");
	}

	toff_t exif_offset;
	if (TIFFGetField(im, TIFFTAG_EXIFIFD, &exif_offset) != 1) {
			FAILm("Could not find EXIF data");
	}

	// Check IFD0 with most metadata and the thumbnail image
		CHECK_CALL(check_int_tag(im, TIFFTAG_ORIENTATION, "ORIENTATION", 4));
		CHECK_CALL(check_int_tag(im, TIFFTAG_BITSPERSAMPLE, "THUMB_BPS", 8));
		CHECK_CALL(check_int_tag(im, TIFFTAG_SAMPLESPERPIXEL, "THUMB_CHANNELS", 3));
		CHECK_CALL(check_int_tag(im, TIFFTAG_PHOTOMETRIC, "THUMB_PHOTOMETRIC", PHOTOMETRIC_RGB));

		CHECK_CALL(check_str_tag(im, TIFFTAG_MAKE, "MAKE", "Make"));
		CHECK_CALL(check_str_tag(im, TIFFTAG_MODEL, "MODEL", "Model"));
		CHECK_CALL(check_str_tag(im, TIFFTAG_UNIQUECAMERAMODEL, "UCM", "Make Model"));
		CHECK_CALL(check_str_tag(im, TIFFTAG_SOFTWARE, "SOFTWARE", "Software"));
		CHECK_CALL(check_int_tag(im, TIFFTAG_ORIENTATION, "ORIENTATION", 4));

	// Switch to IFD1 which has the raw data
	int subifd_count = 0;
	void *ptr;
	toff_t subifd_offsets[2];
	TIFFGetField(im, TIFFTAG_SUBIFD, &subifd_count, &ptr);
	memcpy(subifd_offsets, ptr, subifd_count * sizeof(subifd_offsets[0]));
	TIFFSetSubDirectory(im, subifd_offsets[0]);

	// Check IFD1 metadata
		CHECK_CALL(check_int_tag(im, TIFFTAG_IMAGEWIDTH, "RAW_WIDTH", 1280));
		CHECK_CALL(check_int_tag(im, TIFFTAG_IMAGELENGTH, "RAW_HEIGHT", 720));
		CHECK_CALL(check_int_tag(im, TIFFTAG_BITSPERSAMPLE, "RAW_BPS", 8));
		CHECK_CALL(check_int_tag(im, TIFFTAG_SAMPLESPERPIXEL, "RAW_CHANNELS", 1));
		CHECK_CALL(check_int_tag(im, TIFFTAG_PHOTOMETRIC, "RAW_PHOTOMETRIC", PHOTOMETRIC_CFA));

	// Switch to the EXIF block with the generic picture metadata
	TIFFReadEXIFDirectory(im, exif_offset);
		CHECK_CALL(check_float_tag(im, EXIFTAG_EXPOSURETIME, "EXPOSURETIME", 0.04f));
		CHECK_CALL(check_float_tag(im, EXIFTAG_FNUMBER, "FNUMBER", 2.8f));
		CHECK_CALL(check_float_tag(im, EXIFTAG_FOCALLENGTH, "FOCALLENGTH", 50.0f));
		CHECK_CALL(check_int_tag(im, EXIFTAG_FOCALLENGTHIN35MMFILM, "FOCALLENGTHIN35MMFILM", 75));
		PASS();
}

SUITE (test_suite)
{
		RUN_TEST(generate_simple_dng);
}

GREATEST_MAIN_DEFS();

int
main(int argc, char **argv)
{
	GREATEST_MAIN_BEGIN();
	libdng_init();
	RUN_SUITE(test_suite);
	GREATEST_MAIN_END();
}