#include "dng.h"
#include "libdng.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

unsigned int
get_int32(const unsigned char *buffer, size_t offset)
{
	return (buffer[offset + 3] << 24) | (buffer[offset + 2] << 16) |
		(buffer[offset + 1] << 8) | (buffer[offset]);
}

unsigned int
get_int16(const unsigned char *buffer, size_t offset)
{
	return (buffer[offset + 1] << 8) | (buffer[offset]);
}

float
get_float(unsigned char *buffer, size_t offset)
{
	float f;
	unsigned char b[] = {buffer[offset + 0],
		buffer[offset + 1],
		buffer[offset + 2],
		buffer[offset + 3]};
	memcpy(&f, &b, sizeof(f));
	return f;
}

float
get_srational(unsigned char *buffer, size_t offset)
{
	int a = (int) get_int32(buffer, offset);
	int b = (int) get_int32(buffer, offset + 4);
	return (float) a / (float) b;
}

int
libdng_load_calibration_file(libdng_info *dng, const char *path)
{
	FILE *fp;
	size_t size;
	unsigned char *buffer;

	fp = fopen(path, "rb");
	if (fp == NULL) {
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buffer = malloc(sizeof(char) * size);
	size_t ret = fread(buffer, 1, size, fp);
	if (ret != size) {
		return 0;
	}
	fclose(fp);

	if (buffer[0] != 'I' || buffer[1] != 'I') {
		fprintf(stderr, "Magic for DCP file incorrect\n");
		return 0;
	}
	if (buffer[2] != 0x52 || buffer[3] != 0x43) {
		fprintf(stderr, "Invalid DCP version\n");
		return 0;
	}

	unsigned int ifd0 = get_int32(buffer, 4);
	unsigned int tag_count = get_int16(buffer, ifd0);

	for (int i = 0; i < tag_count; i++) {
		int tag_offset = ifd0 + 2 + (i * 12);
		unsigned int tag = get_int16(buffer, tag_offset + 0);
		// unsigned int type = get_int16(buffer, tag_offset + 2);
		unsigned int count = get_int32(buffer, tag_offset + 4);
		unsigned int offset = get_int32(buffer, tag_offset + 8);

		switch (tag) {
			case DNGTAG_COLOR_MATRIX_1:
				for (int j = 0; j < 9; j++) {
					float point =
						get_srational(buffer, offset + (j * 8));
					dng->color_matrix_1[j] = point;
				}
				break;
			case DNGTAG_COLOR_MATRIX_2:
				for (int j = 0; j < 9; j++) {
					float point =
						get_srational(buffer, offset + (j * 8));
					dng->color_matrix_2[j] = point;
				}
				break;
			case DNGTAG_FORWARD_MATRIX_1:
				for (int j = 0; j < 9; j++) {
					float point =
						get_srational(buffer, offset + (j * 8));
					dng->forward_matrix_1[j] = point;
				}
				break;
			case DNGTAG_FORWARD_MATRIX_2:
				for (int j = 0; j < 9; j++) {
					float point =
						get_srational(buffer, offset + (j * 8));
					dng->forward_matrix_2[j] = point;
				}
				break;
			case DNGTAG_CALIBRATION_ILLUMINANT_1:
				dng->illuminant_1 = offset;
				break;
			case DNGTAG_CALIBRATION_ILLUMINANT_2:
				dng->illuminant_2 = offset;
				break;
			case DNGTAG_PROFILE_TONE_CURVE:
				dng->tone_curve = malloc(count * sizeof(float));
				dng->tone_curve_length = count;
				for (int j = 0; j < count; j++) {
					dng->tone_curve[j] =
						get_float(buffer, offset + (j * 4));
				}
				break;
			case DNGTAG_LINEARIZATIONTABLE:
				dng->tone_curve = malloc(count * sizeof(float));
				dng->tone_curve_length = count;
				for (int j = 0; j < count; j++) {
					dng->tone_curve[j] =
						get_float(buffer, offset + (j * 4));
				}
				break;
			case DNGTAG_PROFILE_HUE_SAT_MAP_DIMS:
				dng->hue_sat_map_dims[0] = get_int32(buffer, offset);
				dng->hue_sat_map_dims[1] = get_int32(buffer, offset + 4);
				dng->hue_sat_map_dims[2] = get_int32(buffer, offset + 8);
				break;
			case DNGTAG_PROFILE_HUE_SAT_MAP_DATA_1:
				dng->hue_sat_map_data_1 = malloc(count * sizeof(float));
				for (int j = 0; j < count; j++) {
					dng->hue_sat_map_data_1[j] =
						get_float(buffer, offset + (j * 4));
				}
				break;
			case DNGTAG_PROFILE_HUE_SAT_MAP_DATA_2:
				dng->hue_sat_map_data_2 = malloc(count * sizeof(float));
				for (int j = 0; j < count; j++) {
					dng->hue_sat_map_data_2[j] =
						get_float(buffer, offset + (j * 4));
				}
				break;
		}
	}

	return 1;
}

bool
find_calibration_by_model(char *conffile, char *model, const char *sensor)
{
	// Check config/%model,%sensor.dcp in the current working directory
	sprintf(conffile, "config/%s,%s.dcp", model, sensor);
	if (access(conffile, F_OK) != -1) {
		printf("Found calibration file at %s\n", conffile);
		return true;
	}

	// Check user overridden /etc/megapixels/config/%model,%sensor.dcp
	sprintf(conffile,
		"%s/megapixels/config/%s,%s.dcp",
		"/etc",
		model,
		sensor);
	if (access(conffile, F_OK) != -1) {
		printf("Found calibration file at %s\n", conffile);
		return true;
	}

	// Check packaged /usr/share/megapixels/config/%model,%sensor.ini
	sprintf(conffile, "%s/megapixels/config/%s,%s.dcp", "/usr/share", model, sensor);
	if (access(conffile, F_OK) != -1) {
		printf("Found calibration file at %s\n", conffile);
		return true;
	}
	printf("No calibration found for %s,%s\n", model, sensor);
	return false;
}

bool
find_calibration(char *conffile, const char *sensor)
{
	char model[512];
	FILE *fp;

	if (access("/proc/device-tree/compatible", F_OK) == -1) {
		return false;
	}
	fp = fopen("/proc/device-tree/compatible", "r");
	char *modelptr = model;
	while (1) {
		int c = fgetc(fp);
		if (c == EOF) {
			*(modelptr) = '\0';
			return find_calibration_by_model(conffile, model, sensor);
		}
		*(modelptr++) = (char) c;
		if (c == 0) {
			bool res =
				find_calibration_by_model(conffile, model, sensor);
			if (res) {
				return true;
			}
			modelptr = model;
		}
	}
}
