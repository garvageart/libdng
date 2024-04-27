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

	dng->neutral[0] = 1.0f;
	dng->neutral[1] = 1.0f;
	dng->neutral[2] = 1.0f;

	dng->analogbalance[0] = 1.0f;
	dng->analogbalance[1] = 1.0f;
	dng->analogbalance[2] = 1.0f;

	dng->exposure_time = 0.0f;
	dng->iso = 0;
	dng->fnumber = 0.0f;
	dng->crop_factor = 1.0f;
	dng->focal_length = 0.0f;
	dng->frame_rate = 0.0f;
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
	dng->whitelevel = (1 << dng->bit_depth) - 1;
	return 1;
}

int
libdng_set_mode_from_pixfmt(libdng_info *dng, uint32_t pixfmt)
{
	int index = dng_mode_from_pixfmt(pixfmt);
	if (index == 0) {
		fprintf(stderr, "libdng: Invalid pixfmt '%c%c%c%c'\n", pixfmt & 0xFF, pixfmt >> 8 & 0xFF, pixfmt >> 16 & 0xFF,
			pixfmt >> 24 & 0xFF);
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
libdng_set_neutral(libdng_info *dng, float red, float green, float blue)
{
	if (dng == NULL)
		return 0;

	dng->neutral[0] = red;
	dng->neutral[1] = green;
	dng->neutral[2] = blue;
	return 1;
}

int
libdng_set_analog_balance(libdng_info *dng, float red, float green, float blue)
{
	if (dng == NULL)
		return 0;

	dng->analogbalance[0] = red;
	dng->analogbalance[1] = green;
	dng->analogbalance[2] = blue;
	return 1;
}

int
libdng_set_exposure_program(libdng_info *dng, uint16_t mode)
{
	if (dng == NULL)
		return 0;

	if (mode > 8)
		return 0;

	dng->exposure_program = mode;
	return 1;
}

int
libdng_set_exposure_time(libdng_info *dng, float seconds)
{
	if (dng == NULL)
		return 0;

	if (seconds < 0.0f)
		return 0;

	dng->exposure_time = seconds;
	return 1;
}

int
libdng_set_iso(libdng_info *dng, uint32_t isospeed)
{
	if (dng == NULL)
		return 0;

	dng->iso = isospeed;
	return 1;
}

int
libdng_set_fnumber(libdng_info *dng, float fnumber)
{
	if (dng == NULL)
		return 0;

	if (fnumber < 0.0f)
		return 0;

	dng->fnumber = fnumber;
	return 1;
}

int
libdng_set_focal_length(libdng_info *dng, float focal_length, float crop_factor)
{
	if (dng == NULL)
		return 0;

	if (focal_length < 0.0f)
		return 0;

	if (crop_factor < 0.0f)
		return 0;

	dng->focal_length = focal_length;
	dng->crop_factor = crop_factor;
	return 1;
}

int
libdng_set_frame_rate(libdng_info *dng, float framerate)
{
	if (dng == NULL)
		return 0;

	dng->frame_rate = framerate;
	return 1;
}

int
libdng_write(libdng_info *dng, const char *path, unsigned int width, unsigned int height, const uint8_t *data,
	size_t length)
{
	return libdng_write_with_thumbnail(dng, path, width, height, data, length, 0, 0, NULL, 0);
}

int
libdng_write_with_thumbnail(libdng_info *dng, const char *path, unsigned int width, unsigned int height,
	const uint8_t *data, size_t length, unsigned int thumb_width, unsigned int thumb_height, const uint8_t *thumb,
	size_t thumb_length)
{

	uint8_t *raw_frame = (uint8_t *) data;
	if (dng->needs_repack) {
		raw_frame = malloc(length);
		dng_repack(data, raw_frame, width, height, dng->bit_depth);
	}

	TIFF *tif = TIFFOpen(path, "w");
	if (!tif) {
		return 0;
	}
	libdng_set_datetime_now(dng);

	char datetime[20] = {0};
	if (dng->datetime.tm_year) {
		strftime(datetime, 20, "%Y:%m:%d %H:%M:%S", &dng->datetime);
	}

	uint64_t ifd0_offsets[] = {0L};

	// When not supplying a thumbnail generate a black thumb with 1/16th of the
	// resolution of the full picture
	if (thumb_length == 0) {
		thumb_width = width >> 4;
		thumb_height = height >> 4;
	}

	// First IFD describes the thumbnail and contains most of the metadata
	// Tags are in numerical order
	TIFFSetField(tif, TIFFTAG_SUBFILETYPE, DNG_SUBFILETYPE_THUMBNAIL);
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, thumb_width);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, thumb_height);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	TIFFSetField(tif, TIFFTAG_ORIENTATION, dng->orientation);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tif, DNGTAG_COLOR_MATRIX_1, 9, dng->color_matrix_1);
	TIFFSetField(tif, DNGTAG_ASSHOTNEUTRAL, 3, dng->neutral);
	TIFFSetField(tif, DNGTAG_ANALOGBALANCE, 3, dng->analogbalance);

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
	if (dng->frame_rate != 0.0f) {
		TIFFSetField(tif, DNGTAG_FRAMERATE, 1, &dng->frame_rate);
	}
	TIFFSetField(tif, TIFFTAG_SUBIFD, 1, &ifd0_offsets);

	if (thumb_length == 0) {
		// Generate a single black scanline and write it
		unsigned char *buf = (unsigned char *) calloc(1, thumb_width * 3);
		for (int row = 0; row < thumb_height; row++) {
			TIFFWriteScanline(tif, buf, row, 0);
		}
		free(buf);
	} else {
		// Write the supplied thumbnail
		unsigned int t_stride = thumb_width;
		for (int row = 0; row < thumb_height; row++) {
			TIFFWriteScanline(tif, (void *) thumb + (row * t_stride), row, 0);
		}
	}

	if (!TIFFWriteDirectory(tif)) {
		return 0;
	}

	// Define the raw data IFD
	TIFFSetField(tif, TIFFTAG_SUBFILETYPE, DNG_SUBFILETYPE_ORIGINAL);
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_CFA);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
#if (TIFFLIB_VERSION > 20230000)
	TIFFSetField(tif, DNGTAG_CFAREPEATPATTERNDIM, 2, dng->bayer_pattern_dimensions);
#else
	TIFFSetField(tif, DNGTAG_CFAREPEATPATTERNDIM, dng->bayer_pattern_dimensions);
#endif
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, dng->bit_depth);
	TIFFSetField(tif, DNGTAG_CFAPATTERN, 4, dng->cfapattern);
	TIFFSetField(tif, DNGTAG_WHITELEVEL, 1, &dng->whitelevel);

	unsigned int stride = width;
	for (int row = 0; row < height; row++) {
		TIFFWriteScanline(tif, (void *) raw_frame + (row * stride), row, 0);
	}
	if (!TIFFWriteDirectory(tif)) {
		return 0;
	}

	if (TIFFCreateEXIFDirectory(tif) != 0) {
		fprintf(stderr, "Could not create EXIF\n");
		return 0;
	}

	if (dng->datetime.tm_year) {
		if (!TIFFSetField(tif, EXIFTAG_DATETIMEORIGINAL, datetime)) {
			fprintf(stderr, "Could not write datetimeoriginal\n");
		}
		TIFFSetField(tif, EXIFTAG_DATETIMEDIGITIZED, datetime);
	}

	TIFFSetField(tif, EXIFTAG_EXPOSUREPROGRAM, dng->exposure_program);

	if (dng->exposure_time > 0) {
		TIFFSetField(tif, EXIFTAG_EXPOSURETIME, dng->exposure_time);
	}
	if (dng->iso > 0) {
		TIFFSetField(tif, EXIFTAG_ISOSPEEDRATINGS, 1, &dng->iso);
	}
	if (dng->fnumber > 0) {
		TIFFSetField(tif, EXIFTAG_FNUMBER, dng->fnumber);
	}
	if (dng->focal_length > 0) {
		TIFFSetField(tif, EXIFTAG_FOCALLENGTH, dng->focal_length);
		if (dng->crop_factor != 1.0f) {
			TIFFSetField(tif, EXIFTAG_FOCALLENGTHIN35MMFILM, (uint16_t) (dng->focal_length * dng->crop_factor));
		}
	}

	uint64_t exif_offset = 0;
	if (!TIFFWriteCustomDirectory(tif, &exif_offset)) {
		fprintf(stderr, "Can't write EXIF\n");
		return 0;
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

	return 1;
}

int
libdng_read(libdng_info *dng, const char *path)
{
	TIFF *tif = TIFFOpen(path, "r");
	if (!tif) {
		return 0;
	}
	unsigned short count;
	char *cvalues;
	float *fvalues;
	uint8_t *u8values;
	uint32_t *u32values;

	// Reading the "main" image which is the thumbnail
	TIFFGetField(tif, TIFFTAG_ORIENTATION, &dng->orientation);
	if (TIFFGetField(tif, TIFFTAG_MAKE, &cvalues) == 1) {
		dng->camera_make = strdup(cvalues);
	}
	if (TIFFGetField(tif, TIFFTAG_MODEL, &cvalues) == 1) {
		dng->camera_model = strdup(cvalues);
	}
	if (TIFFGetField(tif, TIFFTAG_UNIQUECAMERAMODEL, &dng->unique_camera_model) == 1) {
		dng->unique_camera_model = strdup(cvalues);
	}
	if (TIFFGetField(tif, TIFFTAG_SOFTWARE, &cvalues) == 1) {
		dng->software = strdup(cvalues);
	}

	if (TIFFGetField(tif, DNGTAG_ASSHOTNEUTRAL, &count, &fvalues) == 1) {
		for (int i = 0; i < count; i++) {
			dng->neutral[i] = fvalues[i];
		}
	}
	if (TIFFGetField(tif, DNGTAG_ANALOGBALANCE, &count, &fvalues) == 1) {
		for (int i = 0; i < count; i++) {
			dng->analogbalance[i] = fvalues[i];
		}
	}

	if (TIFFGetField(tif, DNGTAG_COLOR_MATRIX_1, &count, &fvalues) == 1) {
		for (int i = 0; i < count; i++) {
			dng->color_matrix_1[i] = fvalues[i];
		}
	}
	if (TIFFGetField(tif, DNGTAG_COLOR_MATRIX_2, &count, &fvalues) == 1) {
		for (int i = 0; i < count; i++) {
			dng->color_matrix_2[i] = fvalues[i];
		}
	}
	if (TIFFGetField(tif, DNGTAG_FORWARDMATRIX1, &count, &fvalues) == 1) {
		for (int i = 0; i < count; i++) {
			dng->forward_matrix_1[i] = fvalues[i];
		}
	}
	if (TIFFGetField(tif, DNGTAG_FORWARDMATRIX2, &count, &fvalues) == 1) {
		for (int i = 0; i < count; i++) {
			dng->forward_matrix_2[i] = fvalues[i];
		}
	}

	int subifd_count = 0;
	void *ptr;
	toff_t subifd_offsets[2];
	TIFFGetField(tif, TIFFTAG_SUBIFD, &subifd_count, &ptr);
	memcpy(subifd_offsets, ptr, subifd_count * sizeof(subifd_offsets[0]));
	TIFFSetSubDirectory(tif, subifd_offsets[0]);

	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &dng->bit_depth);

	if (TIFFGetField(tif, DNGTAG_WHITELEVEL, &count, &u32values) == 1) {
		dng->whitelevel = u32values[0];
	} else {
		dng->whitelevel = (1 << dng->bit_depth) - 1;
	}
	if (TIFFGetField(tif, DNGTAG_CFAPATTERN, &count, &u8values) == 1) {
		if (count > 4) {
			fprintf(stderr, "overflow in CFAPATTERN length %d > 4\n", count);
			return 0;
		}
		for (int i = 0; i < count; i++) {
			dng->cfapattern[i] = u8values[i];
		}
	}

	TIFFClose(tif);
	return 1;
}

int
libdng_read_image(libdng_info *dng, const char *path, uint8_t index, uint8_t **data, size_t *length, uint32_t *width,
	uint32_t *height)
{
	TIFF *tif = TIFFOpen(path, "r");
	if (!tif) {
		return 0;
	}

	if (index == 1) {
		int subifd_count = 0;
		void *ptr;
		toff_t subifd_offsets[2];
		TIFFGetField(tif, TIFFTAG_SUBIFD, &subifd_count, &ptr);
		memcpy(subifd_offsets, ptr, subifd_count * sizeof(subifd_offsets[0]));
		TIFFSetSubDirectory(tif, subifd_offsets[0]);
	}

	uint32_t samples_per_pixel;
	uint32_t bits_per_sample;

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, height);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bits_per_sample);

	tsize_t scanline_size = TIFFScanlineSize(tif);

	float bytes_per_pixel = (float) samples_per_pixel * (float) bits_per_sample / 8.0f;
	(*length) = (uint32_t) ((float) (*width) * (float) (*height) * bytes_per_pixel);
	(*data) = malloc(*length);
	if (*data == NULL) {
		fprintf(stderr, "Could not allocate memory in libdng_read_image\n");
		return 0;
	}

	for (uint32_t y = 0; y < *height; y++) {
		TIFFReadScanline(tif, (*data) + (y * scanline_size), y, 0);
	}

	TIFFClose(tif);
	return 1;
}