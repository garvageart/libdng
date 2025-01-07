#include <stdbool.h>
#include <tiffio.h>
#include "greatest.h"
#include "libdng.h"


TEST load_pinephone_dcp(void)
{
	libdng_info info = {0};
	libdng_new(&info);

	if (!libdng_load_calibration_file(&info, "fixture/pinephone.dcp")) {
			FAILm("Could not load calibration file");
	}

	float colormatrix1[] = {
		1.8464f, -0.9617f, 0.096f,
		-0.4260f, 1.3753f, 0.3348f,
		-0.1147f, 0.4933f, 1.6042f,
	};
	char name[20] = {0};
	for (int i = 0; i < 9; i++) {
		snprintf(name, 20, "COLORMATRIX1[%d]", i);
			ASSERT_IN_RANGEm(strdup(name), colormatrix1[i], info.color_matrix_1[i], 0.0000001f);
	}

	float forwardmatrix1[] = {
		0.7063f, 0.1361f, 0.1219f,
		0.2691f, 0.6969f, 0.0340f,
		0.0003f, 0.0855f, 0.7393f,
	};
	for (int i = 0; i < 9; i++) {
		snprintf(name, 20, "FORWARDMATRIX1[%d]", i);
			ASSERT_IN_RANGEm(strdup(name), forwardmatrix1[i], info.forward_matrix_1[i], 0.0000001f);
	}

		ASSERT_EQ_FMTm("ILLUMINANT1", LIBDNG_ILLUMINANT_STANDARD_A, info.illuminant_1, "%d");
		ASSERT_EQ_FMTm("ILLUMINANT2", LIBDNG_ILLUMINANT_D65, info.illuminant_2, "%d");

		ASSERT_EQ_FMTm("TONE_CURVE_LEN", (size_t) 4, info.tone_curve_length, "%zu");
	double tonecurve[] = {0.0, 0.0, 1.0, 1.0};
	for (size_t i = 0; i < info.tone_curve_length; i++) {
		snprintf(name, 20, "TONECURVE[%zu]", i);
			ASSERT_IN_RANGEm(strdup(name), tonecurve[i], info.tone_curve[i], 0.0000001f);
	}

	// HueSat map is 90x30x1 in this DCP (ProfileHueSatMapDims: Hues = 90, Sats = 30, Vals = 1)
		ASSERT_EQ_FMTm("HUE_SAT_MAP_DIMS[0]", 90, info.hue_sat_map_dims[0], "%d");
		ASSERT_EQ_FMTm("HUE_SAT_MAP_DIMS[1]", 30, info.hue_sat_map_dims[1], "%d");
		ASSERT_EQ_FMTm("HUE_SAT_MAP_DIMS[2]", 1, info.hue_sat_map_dims[2], "%d");

	size_t entries = info.hue_sat_map_dims[0] * info.hue_sat_map_dims[1] * info.hue_sat_map_dims[2];
	for (size_t i = 0; i < entries; i = i + 3) {
		if (info.hue_sat_map_data_1[i] < -360.0f || info.hue_sat_map_data_1[i] > 360.0f) {
			snprintf(name, 20, "HUESATMAP1[%zu]", i);
				FAILm(strdup(name));
		}
		if (info.hue_sat_map_data_2[i] < -360.0f || info.hue_sat_map_data_2[i] > 360.0f) {
			snprintf(name, 20, "HUESATMAP2[%zu]", i);
				FAILm(strdup(name));
		}

	}
		PASS();
}

SUITE (test_suite)
{
		RUN_TEST(load_pinephone_dcp);
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