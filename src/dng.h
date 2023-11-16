#pragma once

/*
 * Tags copied from libtiff and renamed
 */

/* Adobe Digital Negative (DNG) format tags */
#include <tiffio.h>

#define DNGTAG_DNGVERSION 50706         /* &DNG version number */
#define DNGTAG_DNGBACKWARDVERSION 50707 /* &DNG compatibility version */
#define DNGTAG_UNIQUECAMERAMODEL 50708  /* &name for the camera model */
#define DNGTAG_LOCALIZEDCAMERAMODEL                                           \
    50709 /* &localized camera model                                           \
             name */
#define DNGTAG_CFAPLANECOLOR                                                  \
    50710                                /* &CFAPattern->LinearRaw space       \
                                            mapping */
#define DNGTAG_CFALAYOUT 50711          /* &spatial layout of the CFA */
#define DNGTAG_LINEARIZATIONTABLE 50712 /* &lookup table description */
#define DNGTAG_BLACKLEVELREPEATDIM                                            \
    50713                        /* &repeat pattern size for                   \
                                    the BlackLevel tag */
#define DNGTAG_BLACKLEVEL 50714 /* &zero light encoding level */
#define DNGTAG_BLACKLEVELDELTAH                                               \
    50715 /* &zero light encoding level                                        \
             differences (columns) */
#define DNGTAG_BLACKLEVELDELTAV                                               \
    50716 /* &zero light encoding level                                        \
             differences (rows) */
#define DNGTAG_WHITELEVEL                                                     \
    50717                          /* &fully saturated encoding                \
                                      level */
#define DNGTAG_DEFAULTSCALE 50718 /* &default scale factors */
#define DNGTAG_DEFAULTCROPORIGIN                                              \
    50719 /* &origin of the final image                                        \
             area */
#define DNGTAG_DEFAULTCROPSIZE                                                \
    50720 /* &size of the final image                                          \
             area */
#define DNGTAG_COLORMATRIX1                                                   \
    50721 /* &XYZ->reference color space                                       \
             transformation matrix 1 */
#define DNGTAG_COLORMATRIX2                                                   \
    50722                                /* &XYZ->reference color space        \
                                            transformation matrix 2 */
#define DNGTAG_CAMERACALIBRATION1 50723 /* &calibration matrix 1 */
#define DNGTAG_CAMERACALIBRATION2 50724 /* &calibration matrix 2 */
#define DNGTAG_REDUCTIONMATRIX1                                               \
    50725 /* &dimensionality reduction                                         \
             matrix 1 */
#define DNGTAG_REDUCTIONMATRIX2                                               \
    50726 /* &dimensionality reduction                                         \
             matrix 2 */
#define DNGTAG_ANALOGBALANCE                                                  \
    50727 /* &gain applied the stored raw                                      \
             values*/
#define DNGTAG_ASSHOTNEUTRAL                                                  \
    50728 /* &selected white balance in                                        \
             linear reference space */
#define DNGTAG_ASSHOTWHITEXY                                                  \
    50729 /* &selected white balance in                                        \
             x-y chromaticity                                                  \
             coordinates */
#define DNGTAG_BASELINEEXPOSURE                                               \
    50730                           /* &how much to move the zero              \
                                       point */
#define DNGTAG_BASELINENOISE 50731 /* &relative noise level */
#define DNGTAG_BASELINESHARPNESS                                              \
    50732 /* &relative amount of                                               \
             sharpening */
#define DNGTAG_BAYERGREENSPLIT                                                \
    50733                                 /* &how closely the values of        \
                                             the green pixels in the           \
                                             blue/green rows track the         \
                                             values of the green pixels        \
                                             in the red/green rows */
#define DNGTAG_LINEARRESPONSELIMIT 50734 /* &non-linear encoding range */
#define DNGTAG_CAMERASERIALNUMBER 50735  /* &camera's serial number */
#define DNGTAG_LENSINFO 50736            /* info about the lens */
#define DNGTAG_CHROMABLURRADIUS 50737    /* &chroma blur radius */
#define DNGTAG_ANTIALIASSTRENGTH                                              \
    50738                            /* &relative strength of the              \
                                        camera's anti-alias filter */
#define DNGTAG_SHADOWSCALE 50739    /* &used by Adobe Camera Raw */
#define DNGTAG_DNGPRIVATEDATA 50740 /* &manufacturer's private data */
#define DNGTAG_MAKERNOTESAFETY                                                \
    50741                                    /* &whether the EXIF MakerNote    \
                                                tag is safe to preserve        \
                                                along with the rest of the     \
                                                EXIF data */
#define DNGTAG_CALIBRATIONILLUMINANT1 50778 /* &illuminant 1 */
#define DNGTAG_CALIBRATIONILLUMINANT2 50779 /* &illuminant 2 */
#define DNGTAG_BESTQUALITYSCALE 50780       /* &best quality multiplier */
#define DNGTAG_RAWDATAUNIQUEID                                                \
    50781 /* &unique identifier for                                            \
             the raw image data */
#define DNGTAG_ORIGINALRAWFILENAME                                            \
    50827 /* &file name of the original                                        \
             raw file */
#define DNGTAG_ORIGINALRAWFILEDATA                                            \
    50828 /* &contents of the original                                         \
             raw file */
#define DNGTAG_ACTIVEAREA                                                     \
    50829 /* &active (non-masked) pixels                                       \
             of the sensor */
#define DNGTAG_MASKEDAREAS                                                    \
    50830                              /* &list of coordinates                 \
                                          of fully masked pixels */
#define DNGTAG_ASSHOTICCPROFILE 50831 /* &these two tags used to */
#define DNGTAG_ASSHOTPREPROFILEMATRIX                                         \
    50832                                     /* map cameras's color space     \
                                                 into ICC profile space */
#define DNGTAG_CURRENTICCPROFILE 50833       /* & */
#define DNGTAG_CURRENTPREPROFILEMATRIX 50834 /* & */


// DNG tags not present in libtiff
#define DNGTAG_CFAREPEATPATTERNDIM 33421
#define DNGTAG_CFAPATTERN 33422
#define DNGTAG_FORWARDMATRIX1 50964
#define DNGTAG_FORWARDMATRIX2 50965
#define DNGTAG_COLOR_MATRIX_1 50721
#define DNGTAG_COLOR_MATRIX_2 50722
#define DNGTAG_PROFILE_HUE_SAT_MAP_DIMS 50937
#define DNGTAG_PROFILE_HUE_SAT_MAP_DATA_1 50938
#define DNGTAG_PROFILE_HUE_SAT_MAP_DATA_2 50939
#define DNGTAG_PROFILE_TONE_CURVE 50940
#define DNGTAG_CALIBRATION_ILLUMINANT_1 50778
#define DNGTAG_CALIBRATION_ILLUMINANT_2 50779
#define DNGTAG_FORWARD_MATRIX_1 50964
#define DNGTAG_FORWARD_MATRIX_2 50965


/*
 * Field definitions for the tags in the DNG spec
 */

static const TIFFFieldInfo custom_dng_fields[] = {
	{DNGTAG_DNGVERSION,                 -1, -1, TIFF_BYTE,      FIELD_CUSTOM, 1, 1, "DNGVersion"},
	{DNGTAG_FORWARDMATRIX1,             -1, -1, TIFF_SRATIONAL, FIELD_CUSTOM, 1, 1, "ForwardMatrix1"},
	{DNGTAG_FORWARDMATRIX2,             -1, -1, TIFF_SRATIONAL, FIELD_CUSTOM, 1, 1, "ForwardMatrix2"},
	{DNGTAG_PROFILE_TONE_CURVE,         -1, -1, TIFF_FLOAT,     FIELD_CUSTOM, 1, 1, "ProfileToneCurve"},
	{DNGTAG_PROFILE_HUE_SAT_MAP_DIMS,   -1, -1, TIFF_FLOAT,     FIELD_CUSTOM, 1, 1, "ProfileHueSatMapDims"},
	{DNGTAG_PROFILE_HUE_SAT_MAP_DATA_1, -1, -1, TIFF_FLOAT,     FIELD_CUSTOM, 1, 1, "ProfileHueSatMapData1"},
	{DNGTAG_PROFILE_HUE_SAT_MAP_DATA_2, -1, -1, TIFF_FLOAT,     FIELD_CUSTOM, 1, 1, "ProfileHueSatMapData2"},
	{DNGTAG_CFAREPEATPATTERNDIM,        -1, -1, TIFF_SHORT,     FIELD_CUSTOM, 1, 1, "CFARepeatPatternDim"},
	{DNGTAG_CFAPATTERN,                 -1, -1, TIFF_BYTE,      FIELD_CUSTOM, 1, 1, "CFAPattern"},
};
