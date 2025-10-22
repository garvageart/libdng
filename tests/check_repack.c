#include <stdbool.h>
#include "greatest.h"
#include "libdng.h"

void
libdng_repack(const uint8_t* src_buf, uint8_t* dst_buf, uint32_t width, uint32_t height, int bitdepth,
              unsigned int stride);

TEST repack_10bit(void)
{
    // Check 8 pixels of 10 bit without any padding byte afer each row giving it a stride of 5.
    // The 0xAA bytes are the 5th byte with the 2 extra bits per pixel
    const uint8_t test_data[] = {0x11, 0x22, 0x33, 0x44, 0xAA, 0x66, 0x77, 0x88, 0x99, 0xAA};
    uint8_t temp[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t correct[] = {0x11, 0x88, 0xa3, 0x39, 0x12, 0x66, 0x9d, 0xe8, 0x8a, 0x66, 0x00, 0x00};
    libdng_repack(test_data, temp, 4, 2, 10, 0);
    ASSERT_MEM_EQm("Repack 10bit", correct, temp, sizeof temp);
    PASS();
}

TEST repack_10bit_pad1(void)
{
    // Check 8 pixels of 10 bit with 1 padding byte afer each row giving it a stride of 6.
    // The 0xAA bytes are the 5th byte with the 2 extra bits per pixel and the 0xFF bytes are padding
    const uint8_t test_data[] = {0x11, 0x22, 0x33, 0x44, 0xAA, 0xFF, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xFF};
    uint8_t temp[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t correct[] = {0x11, 0x88, 0xa3, 0x39, 0x12, 0x66, 0x9d, 0xe8, 0x8a, 0x66, 0x00, 0x00};
    libdng_repack(test_data, temp, 4, 2, 10, 6);
    ASSERT_MEM_EQm("Repack 10bit pad1", correct, temp, sizeof temp);
    PASS();
}


SUITE(test_suite)
{
    RUN_TEST(repack_10bit);
    RUN_TEST(repack_10bit_pad1);
}

GREATEST_MAIN_DEFS();

int
main(int argc, char** argv)
{
    GREATEST_MAIN_BEGIN();
    libdng_init();
    RUN_SUITE(test_suite);
    GREATEST_MAIN_END();
}
