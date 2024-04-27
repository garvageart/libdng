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
	fprintf(stderr, "Usage: %s -w width -h height -p fmt srcfile dstfile\n", name);
	fprintf(stderr, "Convert raw sensor data to DNG\n\n");
	fprintf(stderr, "Arguments:\n");
	fprintf(stderr, "  -w, --width width               Source data width\n");
	fprintf(stderr, "  -h, --height height             Source data height\n");
	fprintf(stderr, "  -p, --pixfmt fmt                Source data pixelformat\n");
	fprintf(stderr, "  -m, --model make,model          Make and model, comma seperated\n");
	fprintf(stderr, "  -s, --software software         Software name\n");
	fprintf(stderr, "  -o, --orientation orientation   Orientation number [0-9]\n");
	fprintf(stderr, "  -c, --calibration dcp           Append calibration data from .dcp file\n");
	fprintf(stderr, "  -n, --neutral r,g,b             Set the whitepoint as 3 comma seperated floats\n");
	fprintf(stderr, "  -b, --balance r,g,b             Set sensor analog gain as 3 comma seperated floats\n");
	fprintf(stderr, "  -e, --program program           Set the exposure program in EXIF, 0-8\n");
	fprintf(stderr, "  -t, --exposure seconds          Set the exposure time in seconds\n");
	fprintf(stderr, "  -i, --iso speed                 Set the ISO speed rating\n");
	fprintf(stderr, "  -f, --fnumber fnumber           Set the aperture as f/value\n");
	fprintf(stderr, "  -l, --focal-length length,crop  Set the aperture as f/value\n");
	fprintf(stderr, "  -F, --frame-rate framerate      Set the CinemaDNG framerate\n");
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
	char *calibration = NULL;
	uint16_t orientation = 0;
	float neutral[] = {1.0f, 1.0f, 1.0f};
	float balance[] = {1.0f, 1.0f, 1.0f};
	uint16_t exposure_program = 0;
	float exposure_time = 0;
	uint32_t iso = 0;
	float fnumber = 0.0f;
	float framerate = 0.0f;
	float focal_length = 0.0f;
	float crop_factor = 1.0f;

	static struct option long_options[] = {
		{"width", required_argument, NULL, 'w'},
		{"height", required_argument, NULL, 'h'},
		{"orientation", required_argument, NULL, 'o'},
		{"pixfmt", required_argument, NULL, 'p'},
		{"model", required_argument, NULL, 'm'},
		{"software", required_argument, NULL, 's'},
		{"calibration", required_argument, NULL, 'c'},
		{"neutral", required_argument, NULL, 'n'},
		{"balance", required_argument, NULL, 'b'},
		{"program", required_argument, NULL, 'e'},
		{"exposure", required_argument, NULL, 't'},
		{"iso", required_argument, NULL, 'i'},
		{"fnumber", required_argument, NULL, 'f'},
		{"focal-length", required_argument, NULL, 'l'},
		{"frame-rate", required_argument, NULL, 'F'},
		{"help", no_argument, NULL, 'H'},
	};
	int option_index = 0;
	while ((c = getopt_long(argc, argv, "w:h:p:o:m:s:c:n:b:e:t:i:f:l:F:", long_options, &option_index)) != -1) {
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
			case 'c':
				calibration = optarg;
				break;
			case 'n':
				val = sscanf(optarg, "%f,%f,%f", &neutral[0], &neutral[1], &neutral[2]);
				if (val != 3) {
					fprintf(stderr, "Invalid format for -n\n");
					return 1;
				}
				break;
			case 'b':
				val = sscanf(optarg, "%f,%f,%f", &balance[0], &balance[1], &balance[2]);
				if (val != 3) {
					fprintf(stderr, "Invalid format for -b\n");
					return 1;
				}
				break;
			case 'e':
				val = strtol(optarg, &end, 10);
				exposure_program = (uint16_t) val;
				break;
			case 't':
				exposure_time = strtof(optarg, &end);
				break;
			case 'i':
				val = strtol(optarg, &end, 10);
				iso = (uint32_t) val;
				break;
			case 'f':
				fnumber = strtof(optarg, &end);
				break;
			case 'F':
				framerate = strtof(optarg, &end);
				break;
			case 'l':
				val = sscanf(optarg, "%f,%f", &focal_length, &crop_factor);
				if (val != 2 && val != 1) {
					fprintf(stderr, "Invalid format for -l. Specify -l $length,$cropfactor or -l $length\n");
					return 1;
				}
				break;
			case 'H':
				usage(argv[0]);
				return 0;
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

	if (calibration != NULL) {
		libdng_load_calibration_file(&info, calibration);
	}

	libdng_set_neutral(&info, neutral[0], neutral[1], neutral[2]);
	libdng_set_analog_balance(&info, balance[0], balance[1], balance[2]);
	libdng_set_exposure_program(&info, exposure_program);
	if (exposure_time > 0) {
		libdng_set_exposure_time(&info, exposure_time);
	}
	if (iso > 0) {
		libdng_set_iso(&info, iso);
	}
	if (fnumber > 0) {
		libdng_set_fnumber(&info, fnumber);
	}
	if (focal_length > 0) {
		libdng_set_focal_length(&info, focal_length, crop_factor);
	}
	if (framerate > 0) {
		libdng_set_frame_rate(&info, framerate);
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
	if (!libdng_write(&info, argv[optind + 1], width, height, data, src_size)) {
		fprintf(stderr, "Could not write DNG\n");
		return 1;
	}
	free(data);
	libdng_free(&info);
	return 0;
}