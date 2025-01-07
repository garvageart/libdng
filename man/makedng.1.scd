makedng(1) "makedng"

# NAME

makedng - pack raw sensor data into a dng file with metadata

# SYNOPSIS

makedng [OPTIONS] input-file output-file

OPTIONS := { -w width | -h height | -p pixfmt | -m make,model | -s software | -o orientation | -n r,g,b | -b r,g,b | -e program }

# DESCRIPTION

Read raw sensor data from a file and generate a DNG 1.4 compliant file with the
metadata to correctly render the file.

# OPTIONS

*-w width*, *-h height*, *--width width*, *--height height*
	Set the width and height of the source data. This is a required option.

*-p pixelfmt*, *--pixfmt pixelfmt*
	Set the pixelformat for the source data. This accepts a V4L2 fourcc code or
	the V4L constant name like "RGGB" and "SRGGB8". This is a required option.

*-m make,model*, *--model make,model*
	Set the camera Make and Model as a comma separated string.

*-s software*, *--software software*
	Set the name of the camera software used to take the picture.

*-o orientation*, *--orientation orientation*
	Set the orientation of the image data as a number from 1-8 from the DNG
	specification.

*-c dcp-file*, *--calibration dcp-file*
	Load a DCP calibration file and append the color profile to the final DNG.

*-n red,green,blue*, *--neutral red,green,blue*
	Set the neutral whitepoint for the picture as the 3 floating point gain values.

*-b red,green,blue*, *--balance red,green,blue*
	Set the analog sensor gain for the three color channels as three comma separated floats.

*-e program*, *--program program*
	Set the exposure program in the EXIF data using one of the program numbers from the DNG spec.

*-t seconds*, *--exposure seconds*
	Set the exposure time for the picture in seconds (eg. 0.01 for 1/100)

*-i iso-speed*, *--iso iso-speed*
	Set the ISO speed rating for the picture

*-f fnumber*, *--fnumber fnumber*
	Set the aperture size, or f-number of the picture (2.2 for f/2.2)

*-l focal-length,crop-factor* | *-l focal-length*, *--focal-length len,crop*, *--focal-length len*
	Set the focal length for the lens. If no crop factor for the system is supplied a 35mm full-frame sensor is assumed.

# PIXEL FORMATS

The pixel format argument accepts the following values:

8-bit bayer formatted data, this matches with MIPI RAW8:
	RGGB, SRGGB8: RG,GB bayer pixel data++
GRBG, SGRBG8: GR,BG bayer pixel data++
GBRG, SGBRG8: GB,RG bayer pixel data++
BGGR, SBGGR8: BG,GR bayer pixel data

10-bit bayer formatted data. transmitted as 16-bit format in 8 bytes:
	RG10, SRGGB10: RG,GB bayer pixel data++
BA10, SGRBG10: GR,BG bayer pixel data++
GB10, SGBRG10: GB,RG bayer pixel data++
BG10, SBGGR10: BG,GR bayer pixel data

10-bit bayer formatted data packed into 5 bytes, this matches with MIPI RAW10:
	pRAA, SRGGB10P: RG,GB bayer pixel data++
pgAA, SGRBG10P: GR,BG bayer pixel data++
pGAA, SGBRG10P: GB,RG bayer pixel data++
pBAA, SBGGR10P: BG,GR bayer pixel data

12-bit bayer formatted data, transmitted as 16-bit format in 8 bytes:
	RG12, SRGGB12: RG,GB bayer pixel data++
BA12, SGRBG12: GR,BG bayer pixel data++
GB12, SGBRG12: GB,RG bayer pixel data++
BG12, SBGGR12: BG,GR bayer pixel data

12-bit bayer formatted data packed into 6 bytes, this matches with MIPI RAW12:
	pRCC, SRGGB12P: RG,GB bayer pixel data++
pgCC, SGRBG12P: GR,BG bayer pixel data++
pGCC, SGBRG12P: GB,RG bayer pixel data++
pBCC, SBGGR12P: BG,GR bayer pixel data

16-bit bayer formatted data:
	RG16, SRGGB16: RG,GB bayer pixel data++
GR16, SGRBG16: GR,BG bayer pixel data++
GB16, SGBRG16: GB,RG bayer pixel data++
BYR2, SBGGR16: BG,GR bayer pixel data

# EXPOSURE PROGRAMS

The defined exposure programs in the DNG specification are:

- 0: Undefined exposure program (default)
- 1: Full manual program
- 2: Full auto program
- 3: Aperture priority
- 4: Shutter priority
- 5: Creative program
- 6: Action program (fast shutter)
- 7: Portrait program (Large aperture)
- 8: Landscape program (Small aperture)