#include <stdbool.h>
#include "greatest.h"
#include "libdng.h"

#define R 0
#define G 1
#define B 2

static enum greatest_test_res
check_mode(const char *name, int depth, int cfa1, int cfa2, int cfa3, int cfa4, bool needs_repack)
{
	libdng_info info = {0};
	libdng_new(&info);
	libdng_set_mode_from_name(&info, name);
		ASSERT_EQ_FMTm("Bit depth", depth, info.bit_depth, "%d");
		ASSERT_EQ_FMTm("CFA1", cfa1, info.cfapattern[0], "%d");
		ASSERT_EQ_FMTm("CFA2", cfa2, info.cfapattern[1], "%d");
		ASSERT_EQ_FMTm("CFA3", cfa3, info.cfapattern[2], "%d");
		ASSERT_EQ_FMTm("CFA4", cfa4, info.cfapattern[3], "%d");
		ASSERT_EQ_FMTm("Repack", needs_repack, info.needs_repack, "%d");
		PASS();
}

TEST mode_from_name_8bit(void)
{
		CHECK_CALL(check_mode("RGGB", 8, R, G, G, B, false));
		CHECK_CALL(check_mode("BGGR", 8, B, G, G, R, false));
		CHECK_CALL(check_mode("GRBG", 8, G, R, B, G, false));
		CHECK_CALL(check_mode("GBRG", 8, G, B, R, G, false));
		PASS();
}

TEST mode_from_name_10bit(void)
{
		CHECK_CALL(check_mode("SRGGB10", 10, R, G, G, B, false));
		CHECK_CALL(check_mode("SBGGR10", 10, B, G, G, R, false));
		CHECK_CALL(check_mode("SGRBG10", 10, G, R, B, G, false));
		CHECK_CALL(check_mode("SGBRG10", 10, G, B, R, G, false));
		PASS();
}

TEST mode_from_name_10bit_packed(void)
{
		CHECK_CALL(check_mode("SRGGB10P", 10, R, G, G, B, true));
		CHECK_CALL(check_mode("SBGGR10P", 10, B, G, G, R, true));
		CHECK_CALL(check_mode("SGRBG10P", 10, G, R, B, G, true));
		CHECK_CALL(check_mode("SGBRG10P", 10, G, B, R, G, true));
		PASS();
}


SUITE (test_suite)
{
		RUN_TEST(mode_from_name_8bit);
		RUN_TEST(mode_from_name_10bit);
		RUN_TEST(mode_from_name_10bit_packed);
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