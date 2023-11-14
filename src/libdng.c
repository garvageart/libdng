#include "libdng.h"
#include "dng.h"

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
}

int
libdng_set_make_model(libdng_info *dng, char *make, char *model)
{
	if (dng == NULL)
		return 0;

	dng->camera_make = strdup(make);
	dng->camera_model = strdup(model);
	return 1;
}

void
libdng_free(libdng_info *dng)
{
	if (dng->camera_make != NULL)
		free(dng->camera_make);
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
libdng_write(libdng_info *dng, const char *path, unsigned int width, unsigned int height, uint8_t *data, size_t length)
{
	TIFF *tif = TIFFOpen(path, "w");
	if (!tif) {
		return -1;
	}

	char datetime[20] = {0};
	if (dng->datetime.tm_year) {
		strftime(datetime, 20, "%Y:%m:%d %H:%M:%S", &dng->datetime);
	}

	// First IFD describes the thumbnail and contains most of the metadata
	// Tags are in numerical order
	TIFFSetField(tif, TIFFTAG_SUBFILETYPE, DNG_SUBFILETYPE_THUMBNAIL);
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width >> 4);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height >> 4);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	if (dng->camera_make != NULL)
		TIFFSetField(tif, TIFFTAG_MAKE, dng->camera_make);
	if (dng->camera_model != NULL)
		TIFFSetField(tif, TIFFTAG_MODEL, dng->camera_model);
	if (dng->orientation != 1)
		TIFFSetField(tif, TIFFTAG_ORIENTATION, dng->orientation);
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
	unsigned int stride = width;
	for (int row = 0; row < height; row++) {
		TIFFWriteScanline(tif, (void *) data + (row * stride), row, 0);
	}
	if (!TIFFWriteDirectory(tif)) {
		return -1;
	}

	TIFFCreateEXIFDirectory(tif);

	if (dng->datetime.tm_year) {
		TIFFSetField(tif, EXIFTAG_DATETIMEORIGINAL, datetime);
		TIFFSetField(tif, EXIFTAG_DATETIMEDIGITIZED, datetime);
	}

	uint64_t exif_offset = 0;
	TIFFWriteCustomDirectory(tif, &exif_offset);
	TIFFFreeDirectory(tif);

	// Update exif pointer
	TIFFSetDirectory(tif, 0);
	TIFFSetField(tif, TIFFTAG_EXIFIFD, exif_offset);
	TIFFRewriteDirectory(tif);

	TIFFClose(tif);

	return 0;
}
