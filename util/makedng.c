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
	fprintf(stderr, "Usage: %s -w width -h height srcfile dstfile\n", name);
	fprintf(stderr, "Convert raw sensor data to DNG\n\n");
	fprintf(stderr, "Arguments:\n");
	fprintf(stderr, "  -w width       Source data width\n");
	fprintf(stderr, "  -h height      Source data height\n");
	fprintf(stderr, "  -p fmt         Source data pixelformat\n");
	fprintf(stderr, "  -m make,model  Make and model, comma seperated\n");
	fprintf(stderr, "  -s software    Software name\n");
	fprintf(stderr, "  -o orientation Orientation number [0-9]\n");
}

int
main(int argc, char *argv[])
{
	int c;

	char *end;
	long val;

	libdng_init();
	libdng_info info = {0};
	libdng_new(&info);
	unsigned int width = 0;
	unsigned int height = 0;
	char *pixelfmt = NULL;
	char *model = NULL;
	char *software = NULL;
	uint16_t orientation = 0;

	while ((c = getopt(argc, argv, "w:h:p:o:m:s:")) != -1) {
		switch (c) {
			case 'w':
				val = strtol(optarg, &end, 10);
				width = (unsigned int) val;
				break;
			case 'h':
				val = strtol(optarg, &end, 10);
				height = (unsigned int) val;
				break;
			case 'o':
				val = strtol(optarg, &end, 10);
				if (val < 1 || val > 8) {
					fprintf(stderr, "Orientation out of range\n");
					return 1;
				}
				orientation = (uint16_t) val;
				break;
			case 'p':
				pixelfmt = optarg;
				break;
			case 'm':
				model = optarg;
				break;
			case 's':
				software = optarg;
				break;
			case '?':
				if (optopt == 'd' || optopt == 'l') {
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				} else if (isprint(optopt)) {
					fprintf(stderr, "Unknown option '-%c'\n", optopt);
				} else {
					fprintf(stderr, "Unknown option character x%x\n", optopt);
				}
				return 1;
			default:
				return 1;
		}
	}

	if (argc - optind < 2) {
		fprintf(stderr, "Missing required argument\n");
		usage(argv[0]);
		return 1;
	}

	if (width == 0) {
		fprintf(stderr, "The width argument is required\n");
		usage(argv[0]);
		return 1;
	}

	if (pixelfmt == NULL) {
		fprintf(stderr, "The pixel format argument is required\n");
		usage(argv[0]);
		return 1;
	}
	if (!libdng_set_mode_from_name(&info, pixelfmt)) {
		fprintf(stderr, "Invalid pixel format supplied\n");
		usage(argv[0]);
		return 1;
	}

	if (model != NULL) {
		char *make = model;
		for (size_t i = 0; i < strlen(model); i++) {
			if (model[i] == ',') {
				model[i] = '\0';
				model = &model[i + 1];
			}
		}
		printf("Make: '%s'\n", make);
		printf("Model: '%s'\n", model);
		libdng_set_make_model(&info, make, model);
	}

	if (software != NULL) {
		libdng_set_software(&info, software);
	}

	if (orientation > 0) {
		libdng_set_orientation(&info, orientation);
	}

	printf("Reading %s...\n", argv[optind]);
	FILE *src = fopen(argv[optind], "r");
	if (src == NULL) {
		fprintf(stderr, "Can't open source file: %s\n", strerror(errno));
		return 1;
	}
	fseek(src, 0L, SEEK_END);
	long src_size = ftell(src);
	rewind(src);
	uint8_t *data = malloc(src_size);
	fread(data, src_size, 1, src);
	fclose(src);

	printf("Writing %s...\n", argv[optind + 1]);
	if (libdng_write(&info, argv[optind + 1], width, height, data, src_size) < 0) {
		fprintf(stderr, "Could not write DNG\n");
		return 1;
	}
	free(data);
	libdng_free(&info);
	return 0;
}