#!/bin/bash
set -e

if [ $# -ne 1 ]; then
  echo "Missing tool argument"
  exit 1
fi
makedng="$1"
echo "Running tests with '$makedng'"

# This testsuite runs raw data through the makedng utility and validates the
# result using the dng_validate tool from the Adobe DNG SDK. This tool needs
# to be manually installed for these tests to run.

# Create test raw data
mkdir -p scratch
magick -size 1280x720 gradient: -colorspace RGB scratch/data.rgb

# Generate DNG

$makedng -w 1280 -h 720 -p RGGB -m Test,RGGB scratch/data.rgb scratch/RGGB.dng
$makedng -w 1280 -h 720 -p GRBG -m Test,GRBG scratch/data.rgb scratch/GRBG.dng
$makedng -w 1280 -h 720 -p GBRG -m Test,GBRG scratch/data.rgb scratch/GBRG.dng
$makedng -w 1280 -h 720 -p BGGR -m Test,BGGR scratch/data.rgb scratch/BGGR.dng

$makedng -w 1280 -h 720 -p RG10 scratch/data.rgb scratch/RG10.dng
$makedng -w 1280 -h 720 -p SRGGB10P scratch/data.rgb scratch/SRGGB10P.dng

$makedng --width 1280 --height 720 --pixfmt RGGB \
  --software "Testsuite" \
  --orientation 2 \
  --neutral 0.1,0.2,0.3 \
  --balance 0.4,0.5,0.6 \
  --program 2 \
  --exposure 0.01 \
  --iso 1600 \
  --fnumber 2.8 \
  --focal-length 50,1.5 \
  --frame-rate 30 \
  scratch/data.rgb scratch/fields.dng

# Validate DNG
dng_validate scratch/RGGB.dng
dng_validate scratch/GRBG.dng
dng_validate scratch/GBRG.dng
dng_validate scratch/BGGR.dng

dng_validate scratch/RG10.dng
dng_validate scratch/SRGGB10P.dng

dng_validate scratch/fields.dng