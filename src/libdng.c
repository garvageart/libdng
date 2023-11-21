#include "libdng.h"
#include "dng.h"
#include "mode.h"
#include "repack.h"

#include <stdio.h>
#include <tiffio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

#define DNG_SUBFILETYPE_ORIGINAL 0
#define DNG_SUBFILETYPE_THUMBNAIL 1
#define DNG_SUBFILETYPE_ALPHAMASK 4
#define DNG_SUBFILETYPE_THUMBNAIL_ALPHAMASK 5
#define DNG_SUBFILETYPE_THUMBNAIL_EXTRA 0x10001

static void
register_dng_tags(TIFF *tif)
{
	TIFFMergeFieldInfo(tif,
		custom_dng_fields,
		sizeof(custom_dng_fields) / sizeof(custom_dng_fields[0]));
}

int
libdng_init()
{
	TIFFSetTagExtender(register_dng_tags);
	return 0;
}

void
libdng_new(libdng_info *dng)
{
	dng->orientation = 1;
	dng->bayer_pattern_dimensions[0] = 2;
	dng->bayer_pattern_dimensions[1] = 2;

	for (size_t i = 0; i < 9; i++) {
		dng->color_matrix_1[i] = 0.0f;
		dng->color_matrix_2[i] = 0.0f;
	}
	dng->color_matrix_1[0] = 1.0f;
	dng->color_matrix_1[4] = 1.0f;
	dng->color_matrix_1[8] = 1.0f;

	dng->cfapattern[0] = 0;
	dng->cfapattern[1] = 1;
	dng->cfapattern[2] = 1;
	dng->cfapattern[3] = 2;
}

int
libdng_set_mode_from_index(libdng_info *dng, int index)
{
	uint32_t cfa = dng_cfa_from_mode(index);
	dng->cfapattern[0] = (cfa >> 24) & 0xFF;
	dng->cfapattern[1] = (cfa >> 16) & 0xFF;
	dng->cfapattern[2] = (cfa >> 8) & 0xFF;
	dng->cfapattern[3] = (cfa >> 0) & 0xFF;
	dng->needs_repack = dng_mode_needs_repack(index);
	dng->bit_depth = dng_bitdepth_from_mode(index);
	return 1;
}

int
libdng_set_mode_from_pixfmt(libdng_info *dng, uint32_t pixfmt)
{
	int index = dng_mode_from_pixfmt(pixfmt);
	if (index == 0) {
		fprintf(stderr, "Invalid pixfmt '%d'\n", pixfmt);
		return 0;
	}
	return libdng_set_mode_from_index(dng, index);
}

int
libdng_set_mode_from_name(libdng_info *dng, const char *name)
{
	int index = dng_mode_from_name(name);
	if (index == 0) {
		fprintf(stderr, "Invalid mode '%s'\n", name);
		return 0;
	}
	return libdng_set_mode_from_index(dng, index);
}

int
libdng_set_make_model(libdng_info *dng, const char *make, const char *model)
{
	if (dng == NULL)
		return 0;

	dng->camera_make = strdup(make);
	dng->camera_model = strdup(model);
	return 1;
}

int
libdng_set_software(libdng_info *dng, const char *software)
{
	if (dng == NULL)
		return 0;

	dng->software = strdup(software);
	return 1;
}

void
libdng_free(libdng_info *dng)
{
	if (dng->camera_make != NULL)
		free(dng->camera_make);
	if (dng->camera_model != NULL)
		free(dng->camera_model);
	if (dng->software != NULL)
		free(dng->software);
}

int
libdng_set_datetime(libdng_info *dng, struct tm time)
{
	if (dng == NULL)
		return 0;

	dng->datetime = time;
	return 1;
}

int
libdng_set_datetime_now(libdng_info *dng)
{
	if (dng == NULL)
		return 0;

	time_t rawtime;
	time(&rawtime);
	dng->datetime = *(localtime(&rawtime));
	return 1;
}

int
libdng_set_orientation(libdng_info *dng, uint16_t orientation)
{
	if (dng == NULL)
		return 0;

	dng->orientation = orientation;
	return 1;
}

int
libdng_write(libdng_info *dng, const char *path, unsigned int width, unsigned int height, const uint8_t *data,
	size_t length)
{

	uint8_t *raw_frame = (uint8_t *) data;
	if (dng->needs_repack) {
		raw_frame = malloc(length);
		dng_repack(data, raw_frame, width, height, dng->bit_depth);
	}

	TIFF *tif = TIFFOpen(path, "w");
	if (!tif) {
		return -1;
	}
	libdng_set_datetime_now(dng);

	char datetime[20] = {0};
	if (dng->datetime.tm_year) {
		strftime(datetime, 20, "%Y:%m:%d %H:%M:%S", &dng->datetime);
	}

	uint64_t ifd0_offsets[] = {0L};


	// First IFD describes the thumbnail and contains most of the metadata
	// Tags are in numerical order
	TIFFSetField(tif, TIFFTAG_SUBFILETYPE, DNG_SUBFILETYPE_THUMBNAIL);
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width >> 4);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height >> 4);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	TIFFSetField(tif, TIFFTAG_ORIENTATION, dng->orientation);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tif, DNGTAG_COLOR_MATRIX_1, 9, dng->color_matrix_1);
	TIFFSetField(tif, DNGTAG_ASSHOTNEUTRAL, 3, dng->neutral);

	if (dng->camera_make != NULL)
		TIFFSetField(tif, TIFFTAG_MAKE, dng->camera_make);
	if (dng->camera_model != NULL)
		TIFFSetField(tif, TIFFTAG_MODEL, dng->camera_model);
	if (dng->software != NULL)
		TIFFSetField(tif, TIFFTAG_SOFTWARE, dng->software);

	if (dng->datetime.tm_year) {
		TIFFSetField(tif, TIFFTAG_DATETIME, datetime);
	}

	TIFFSetField(tif, DNGTAG_DNGVERSION, "\001\004\0\0");

	char ucm[255];
	if (dng->unique_camera_model != NULL) {
		snprintf(ucm, sizeof(ucm), "%s", dng->unique_camera_model);
	} else if (dng->camera_make == NULL && dng->camera_model == NULL) {
		snprintf(ucm, sizeof(ucm), "%s", "LibDNG");
	} else {
		snprintf(ucm, sizeof(ucm), "%s %s", dng->camera_make, dng->camera_model);
	}
	TIFFSetField(tif, DNGTAG_UNIQUECAMERAMODEL, ucm);
	TIFFSetField(tif, TIFFTAG_SUBIFD, 1, &ifd0_offsets);

	// Write black thumbnail, only windows uses this
	{
		unsigned char *buf = (unsigned char *) calloc(1, (width >> 4) * 3);
		for (int row = 0; row < (height >> 4); row++) {
			TIFFWriteScanline(tif, buf, row, 0);
		}
		free(buf);
	}

	if (!TIFFWriteDirectory(tif)) {
		return -1;
	}

	// Define the raw data IFD
	TIFFSetField(tif, TIFFTAG_SUBFILETYPE, DNG_SUBFILETYPE_ORIGINAL);
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_CFA);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
	TIFFSetField(tif, TIFFTAG_CFAREPEATPATTERNDIM, 2, dng->bayer_pattern_dimensions);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(tif, DNGTAG_CFAPATTERN, 4, dng->cfapattern);

	unsigned int stride = width;
	for (int row = 0; row < height; row++) {
		TIFFWriteScanline(tif, (void *) raw_frame + (row * stride), row, 0);
	}
	if (!TIFFWriteDirectory(tif)) {
		return -1;
	}

	if (TIFFCreateEXIFDirectory(tif) != 0) {
		fprintf(stderr, "Could not create EXIF\n");
		return -1;
	}

	if (dng->datetime.tm_year) {
		if (!TIFFSetField(tif, EXIFTAG_DATETIMEORIGINAL, datetime)) {
			fprintf(stderr, "Could not write datetimeoriginal\n");
		}
		TIFFSetField(tif, EXIFTAG_DATETIMEDIGITIZED, datetime);
	}

	uint64_t exif_offset = 0;
	if (!TIFFWriteCustomDirectory(tif, &exif_offset)) {
		fprintf(stderr, "Can't write EXIF\n");
		return -1;
	}
	TIFFFreeDirectory(tif);

	// Update exif pointer
	TIFFSetDirectory(tif, 0);
	TIFFSetField(tif, TIFFTAG_EXIFIFD, exif_offset);
	TIFFRewriteDirectory(tif);


	TIFFClose(tif);

	if (dng->needs_repack) {
		free(raw_frame);
	}
	
	return 0;
}
