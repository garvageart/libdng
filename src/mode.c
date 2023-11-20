#include <string.h>
#include <linux/videodev2.h>
#include "mode.h"

static struct pixelformat pixelformat_lut[] = {
	{
		.fourcc = "",
		.name = "unsupported",
		.pixfmt = 0,
		.cfa = CFA_NONE,
		.bits_per_sample = 0,
	},
	{
		.fourcc = "RGGB",
		.name = "SRGGB8",
		.pixfmt = V4L2_PIX_FMT_SRGGB8,
		.cfa = CFA_RGGB,
		.bits_per_sample = 8,
	},
	{
		.fourcc = "GRBG",
		.name = "SGRBG8",
		.pixfmt = V4L2_PIX_FMT_SGRBG8,
		.cfa = CFA_GRBG,
		.bits_per_sample = 8,
	},
	{
		.fourcc = "GBRG",
		.name = "SGBRG8",
		.pixfmt = V4L2_PIX_FMT_SGBRG8,
		.cfa = CFA_GBRG,
		.bits_per_sample = 8,
	},
	{
		.fourcc = "BGGR",
		.name = "SBGGR8",
		.pixfmt = V4L2_PIX_FMT_SBGGR8,
		.cfa = CFA_BGGR,
		.bits_per_sample = 8,
	},
	{
		.fourcc = "RG10",
		.name = "SRGGB10",
		.pixfmt = V4L2_PIX_FMT_SRGGB10,
		.cfa = CFA_RGGB,
		.bits_per_sample = 10,
	},
	{
		.fourcc = "BA10",
		.name = "SGRBG10",
		.pixfmt = V4L2_PIX_FMT_SGRBG10,
		.cfa = CFA_GRBG,
		.bits_per_sample = 10,
	},
	{
		.fourcc = "GB10",
		.name = "SGBRG10",
		.pixfmt = V4L2_PIX_FMT_SGBRG10,
		.cfa = CFA_GBRG,
		.bits_per_sample = 10,
	},
	{
		.fourcc = "BG10",
		.name = "SBGGR10",
		.pixfmt = V4L2_PIX_FMT_SBGGR10,
		.cfa = CFA_BGGR,
		.bits_per_sample = 10,
	},
	{
		.fourcc = "pRAA",
		.name = "SRGGB10P",
		.pixfmt = V4L2_PIX_FMT_SRGGB10P,
		.cfa = CFA_RGGB,
		.bits_per_sample = 10,
	},
	{
		.fourcc = "pgAA",
		.name = "SGRBG10P",
		.pixfmt = V4L2_PIX_FMT_SGRBG10P,
		.cfa = CFA_GRBG,
		.bits_per_sample = 10,
	},
	{
		.fourcc = "pGAA",
		.name = "SGBRG10P",
		.pixfmt = V4L2_PIX_FMT_SGBRG10P,
		.cfa = CFA_GBRG,
		.bits_per_sample = 10,
	},
	{
		.fourcc = "pBAA",
		.name = "SBGGR10P",
		.pixfmt = V4L2_PIX_FMT_SBGGR10P,
		.cfa = CFA_BGGR,
		.bits_per_sample = 10,
	},
	{
		.fourcc = "RG12",
		.name = "SRGGB12",
		.pixfmt = V4L2_PIX_FMT_SRGGB12,
		.cfa = CFA_RGGB,
		.bits_per_sample = 12,
	},
	{
		.fourcc = "BA12",
		.name = "SGRBG12",
		.pixfmt = V4L2_PIX_FMT_SGRBG12,
		.cfa = CFA_GRBG,
		.bits_per_sample = 12,
	},
	{
		.fourcc = "GB12",
		.name = "SGBRG12",
		.pixfmt = V4L2_PIX_FMT_SGBRG12,
		.cfa = CFA_GBRG,
		.bits_per_sample = 12,
	},
	{
		.fourcc = "BG12",
		.name = "SBGGR12",
		.pixfmt = V4L2_PIX_FMT_SBGGR12,
		.cfa = CFA_BGGR,
		.bits_per_sample = 12,
	},
	{
		.fourcc = "RG16",
		.name = "SRGGB16",
		.pixfmt = V4L2_PIX_FMT_SRGGB16,
		.cfa = CFA_RGGB,
		.bits_per_sample = 16,
	},
	{
		.fourcc = "GR16",
		.name = "SGRBG16",
		.pixfmt = V4L2_PIX_FMT_SGRBG16,
		.cfa = CFA_GRBG,
		.bits_per_sample = 16,
	},
	{
		.fourcc = "GB16",
		.name = "SGBRG16",
		.pixfmt = V4L2_PIX_FMT_SGBRG16,
		.cfa = CFA_GBRG,
		.bits_per_sample = 16,
	},
	{
		.fourcc = "BYR2",
		.name = "SBGGR16",
		.pixfmt = V4L2_PIX_FMT_SBGGR16,
		.cfa = CFA_BGGR,
		.bits_per_sample = 16,
	},
};

int
dng_mode_from_name(const char *name)
{
	int count = sizeof(pixelformat_lut) / sizeof(pixelformat_lut[0]);
	for (int i = 0; i < count; i++) {
		if (strcmp(pixelformat_lut[i].name, name) == 0) {
			return i;
		}
		if (strcmp(pixelformat_lut[i].fourcc, name) == 0) {
			return i;
		}
	}
	return 0;
}

int
dng_mode_from_pixfmt(uint32_t pixfmt)
{
	int count = sizeof(pixelformat_lut) / sizeof(pixelformat_lut[0]);
	for (int i = 0; i < count; i++) {
		if (pixelformat_lut[i].pixfmt == pixfmt) {
			return i;
		}
	}
	return 0;
}

uint32_t
dng_cfa_from_mode(int index)
{
	return pixelformat_lut[index].cfa;
}