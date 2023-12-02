#include <getopt.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "libdng.h"

void
usage(char *name)
{
	fprintf(stderr, "Usage: %s src-file dcp-file dst-file\n", name);
	fprintf(stderr, "Merge color metadata from one TIFF file into the image data of another TIFF\n\n");
}

int
main(int argc, char *argv[])
{
	libdng_init();
	libdng_info info = {0};
	libdng_new(&info);

	if (argc < 3) {
		fprintf(stderr, "Missing required argument\n");
		usage(argv[0]);
		return 1;
	}

	uint32_t width, height, thumb_width, thumb_height;
	uint8_t *thumb;
	uint8_t *image;
	size_t thumb_length;
	size_t image_length;

	if (!libdng_read(&info, argv[1])) {
		fprintf(stderr, "Could not load the metadata from the original file\n");
		exit(1);
	}

	if (!libdng_read_image(&info, argv[1], 0, &thumb, &thumb_length, &thumb_width, &thumb_height)) {
		fprintf(stderr, "Could not load thumbnail from the original file\n");
		exit(1);
	}
	printf("Got %dx%d thumbnail of %zu bytes\n", thumb_width, thumb_height, thumb_length);

	if (!libdng_read_image(&info, argv[1], 1, &image, &image_length, &width, &height)) {
		fprintf(stderr, "Could not load image data from the original file\n");
		exit(1);
	}
	printf("Got %dx%d image of %zu bytes\n", width, height, image_length);

	if (!libdng_load_calibration_file(&info, argv[2])) {
		fprintf(stderr, "Could not load calibration files: %s\n", argv[2]);
		exit(1);
	}

	libdng_write_with_thumbnail(&info, argv[3], width, height, image, image_length, thumb_width, thumb_height, thumb,
		thumb_length);

	free(thumb);
	free(image);
	libdng_free(&info);
	return 0;
}