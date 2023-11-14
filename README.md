## Inside a DNG file

* SubIFDType 0 is the original raw data
* SubIFDType 1 is the thumbnail data
* The recommendation is to store the thumbnail as the first IFD
* TIFF metdata goes in the first IFD
* EXIF tags are preferred
* Camera profiles are stored in the first IFD

## Required tags

* DNGVersion
* UniqueCameraModel

## Neat tags

* AnalogBalance stores the gains applied to the RGB channels by the sensor