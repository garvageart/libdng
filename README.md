## libdng

[!NOTE] This is a fork of [libdng](https://gitlab.com/megapixels-org/libdng), intended to integrate
the official Adobe DNG SDK.

This is a library for making it easier to generate valid DNG files following
the Adobe Digital Negative specification. This library is written for writing
pictures created with [Megapixels](https://gitlab.com/megapixels-org/Megapixels)
but should be useful for any photography applications.

The documentation can be found on [https://libdng.me.gapixels.me/](https://libdng.me.gapixels.me/)

Main features are:
* Writing DNG 1.4.0 compliant TIFF files
* Parsing DCP files for extra color processing metadata
* Some DNG reading support for the `dngmerge` utility
* Isolating applications from the changing libtiff APIs

This library also builds the `makedng` utility which allows generating a .dng
file by hand from a raw sensor dump so it can be loaded in regular image
processing software and a `dngmerge` utlity for combinding an existing DNG file
with new color calibration tags from a DCP file.
