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
	fprintf(stderr, "  -w width    Source data width\n");
	fprintf(stderr, "  -h height   Source data height\n");
}

int
main(int argc, char *argv[])
{
	int c;

	char *end;
	long val;

	libdng_init();
	libdng_info info;
	libdng_new(&info);
	unsigned int width = 0;
	unsigned int height = 0;

	while ((c = getopt(argc, argv, "w:h:")) != -1) {
		switch (c) {
			case 'w':
				val = strtol(optarg, &end, 10);
				width = (unsigned int) val;
				break;
			case 'h':
				val = strtol(optarg, &end, 10);
				height = (unsigned int) val;
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

	printf("Reading %s\n", argv[optind]);
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

	libdng_write(&info, argv[optind + 1], width, height, data, src_size);
	free(data);
	libdng_free(&info);
	return 0;
}