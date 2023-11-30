API
===

.. contents:: Table of Contents
   :depth: 3

Initialisation
--------------

The library mostly works on the libdng_info objects. There's only a single
initialisation function

int libdng_init()
^^^^^^^^^^^^^^^^^

This function should be called exactly once before using any of the other
functionality of the library. The purpose of this call is to register new
tags into libtiff which is required before any other interaction.

DNG objects
-----------

All the state for writing is stored in the :code:`libdng_info` struct. These
functions work by manipulating this instance.

void libdng_new(libdng_info \*dng)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Initialize the value of a libdng_info struct for further operation. This needs
to be cleaned up with libdng_free.

.. code-block:: c

   libdng_info dng = {0};
   libdng_new(&dng);


void libdng_free(libdng_info \*dng)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Clean up the allocations of :code:`libdng_new`

.. code-block:: c

   libdng_info dng = {0};
   libdng_new(&dng);

   // Do important stuff here

   libdng_free(&dng);

.. _set_mode_from_name:

int libdng_set_mode_from_name(libdng_info \*dng, const char \*name)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Set the pixel format for the raw data that will be written into the DNG. This
configures the CFA pattern, bit depth and whitelevel metadata. The valid names
are the V4L2 fourcc codes and the constant names.

.. code-block:: c

   libdng_info dng = {0};
   libdng_new(&dng);

   libdng_set_mode_from_name(&dng, "SRGGB10P");

   libdng_free(&dng);

Affected tags:

- TIFFTAG_CFAPATTERN
- TIFFTAG_WHITELEVEL
- TIFFTAG_BITSPERSAMPLE

Supported values:

+--------+---------------+-------+
| fourcc | constant name | Depth |
+========+===============+=======+
| RGGB   | SRGGB8        | 8     |
+--------+---------------+-------+
| GRBG   | SGRBG8        | 8     |
+--------+---------------+-------+
| GBRG   | SGBRG8        | 8     |
+--------+---------------+-------+
| BGGR   | SBGGR8        | 8     |
+--------+---------------+-------+
| RG10   | SRGGB10       | 10    |
+--------+---------------+-------+
| BA10   | SGRBG10       | 10    |
+--------+---------------+-------+
| GB10   | SGBRG10       | 10    |
+--------+---------------+-------+
| BG10   | SBGGR10       | 10    |
+--------+---------------+-------+
| pRAA   | SRGGB10P      | 10    |
+--------+---------------+-------+
| pgAA   | SGRBG10P      | 10    |
+--------+---------------+-------+
| pGAA   | SGBRG10P      | 10    |
+--------+---------------+-------+
| pBAA   | SBGGR10P      | 10    |
+--------+---------------+-------+
| RG12   | SRGGB12       | 12    |
+--------+---------------+-------+
| BA12   | SGRBG12       | 12    |
+--------+---------------+-------+
| GB12   | SGBRG12       | 12    |
+--------+---------------+-------+
| BG12   | SBGGR12       | 12    |
+--------+---------------+-------+
| pRCC   | SRGGB12P      | 12    |
+--------+---------------+-------+
| pgCC   | SGRBG12P      | 12    |
+--------+---------------+-------+
| pGCC   | SGBRG12P      | 12    |
+--------+---------------+-------+
| pBCC   | SBGGR12P      | 12    |
+--------+---------------+-------+
| RG16   | SRGGB16       | 16    |
+--------+---------------+-------+
| GR16   | SGRBG16       | 16    |
+--------+---------------+-------+
| GB16   | SGBRG16       | 16    |
+--------+---------------+-------+
| BYR2   | SBGGR16       | 16    |
+--------+---------------+-------+

int libdng_set_mode_from_pixfmt(libdng_info \*dng, uint32_t pixfmt)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This function performs the same tasks as :ref:`libdng_set_mode_from_name <set_mode_from_name>` but
uses the numeric fourcc value instead of a string. This is for passing through
the format from the V4L2 subsystem directly.

.. code-block:: c

   libdng_info dng = {0};
   libdng_new(&dng);

   libdng_set_mode_from_pixfmt(&dng, V4L2_PIX_FMT_SGBRG12P);

   libdng_free(&dng);

Affected tags:

- TIFFTAG_CFAPATTERN
- TIFFTAG_WHITELEVEL
- TIFFTAG_BITSPERSAMPLE


int libdng_set_make_model(libdng_info \*dng, const char \*make, const char \*model)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Set the make and model name for the camera or other digitizing equipment that
produced the raw data. The make and model are also concatinated together to
produce the "unique camera model" field that is usually shown to end-users as
the camera model.

.. code-block:: c

   libdng_info dng = {0};
   libdng_new(&dng);

   libdng_set_make_model(&dng, "Nikon", "D3300");

   libdng_free(&dng);

Affected tags:

- TIFFTAG_MAKE
- TIFFTAG_MODEL
- TIFFTAG_UNIQUECAMERAMODEL


int libdng_set_software(libdng_info \*dng, const char \*software)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Set the name and version number of the software used to produce the image. On
phones this value is usually set to the camera application.

.. code-block:: c

   libdng_info dng = {0};
   libdng_new(&dng);

   libdng_set_software(&dng, "Megapixels 2.0");

   libdng_free(&dng);

Affected tags:

- TIFFTAG_SOFTWARE


.. _set_datetime:

int libdng_set_datetime(libdng_info \*dng, struct tm time)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Set the date and time the picture was taken or digitized. This is stored as both
TIFF and EXIF metadata. Instead of specifying a :code:`tm` struct the
:ref:`libdng_set_datetime_now <set_datetime_now>` function can be used to
directly use the current time.

.. code-block:: c

   libdng_info dng = {0};
   libdng_new(&dng);

   time_t rawtime;
   time(&rawtime);
   struct tm now = *(localtime(&rawtime));

   libdng_set_datetime(&dng, now);

   libdng_free(&dng);

Affected tags:

- TIFFTAG_DATETIME
- EXIFTAG_DATETIMEORIGINAL
- EXIFTAG_DATETIMEDIGITIZED


.. _set_datetime_now:

int libdng_set_datetime_now(libdng_info \*dng)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Set the date and time the picture was taken or digitized to the current date and
time. This is a shortcut method for :ref:`libdng_set_datetime <set_datetime>`.

.. code-block:: c

   libdng_info dng = {0};
   libdng_new(&dng);

   libdng_set_datetime_now(&dng);

   libdng_free(&dng);

Affected tags:

- TIFFTAG_DATETIME
- EXIFTAG_DATETIMEORIGINAL
- EXIFTAG_DATETIMEDIGITIZED


.. _set_orientation:

int libdng_set_orientation(libdng_info \*dng, uint16_t orientation)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Set the date and time the picture was taken or digitized to the current date and
time. This is a shortcut method for :ref:`libdng_set_datetime <set_datetime>`.

.. code-block:: c

   libdng_info dng = {0};
   libdng_new(&dng);

   libdng_set_datetime_now(&dng);

   libdng_free(&dng);

Affected tags:

- TIFFTAG_DATETIME
- EXIFTAG_DATETIMEORIGINAL
- EXIFTAG_DATETIMEDIGITIZED


.. _set_neutral:

int libdng_set_neutral(libdng_info \*dng, float r, float g, float b)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Set the whitebalance of the picture. The neutral whitepoint gains are the RGB
gains that will be applied on top of the raw debayered data to get a properly
whitebalanced picture. If the camera already uses whitebalance gain in the
hardware then those should be stored in the analogbalance metadata instead and
set using the :ref:`libdng_set_analog_balance <set_analog_balance>` function.

In most cases the gain for the green channel will be 1.0f and the red and blue
gains will be adjusted around that. It is important to have the right
analogbalance and neutral whitebalance in the metadata to make the DNG processing
software create the right color matrix from the two provided whitepoints in the
metadata.

The neutral whitepoint defaults to 1.0, 1.0, 1.0.

.. code-block:: c

   libdng_info dng = {0};
   libdng_new(&dng);

   libdng_set_neutral(&dng, 1.2f, 1.0f, 1.6f);

   libdng_free(&dng);

Affected tags:

- TIFFTAG_ASSHOTNEUTRAL


.. _set_analog_balance:

int libdng_set_analog_balance(libdng_info \*dng, float r, float g, float b)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Set the ADC gain used to whitebalance the sensor readings before they were
digitized in the RAW file. This will be used to normalize the raw data again
before using the rest of the color pipeline. Most sensors will not whitebalance
the raw bayer data but some sensors in 8-bit CFA output might do this to optimize
the use of the available bits a bit more.

For the whitebalance after the capture of the raw data the
:ref:`libdng_set_neutral <set_neutral>` function should be used to store the
measured whitebalance.

.. code-block:: c

   libdng_info dng = {0};
   libdng_new(&dng);

   libdng_set_analog_balance(&dng, 1.2f, 1.0f, 1.6f);

   libdng_free(&dng);

Affected tags:

- TIFFTAG_ANALOGBALANCE


.. _load_calibration_file:

int libdng_load_calibration_file(libdng_info \*dng, const char \*path)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Load the calibration metadata from another TIFF file and append it to the final
DNG output. This is used to load all the calibration data from a :code:`.dcp`
file which is a "DNG Color Profile". These files are generated using profiling
software and contain the colormatrices, calibration curves and LUTs to transform
the raw data into a calibrated picture. The exact list of TIFF tags that are
loaded with this command is the list below:

.. code-block:: c

   libdng_info dng = {0};
   libdng_new(&dng);

   libdng_load_calibration_file(&dng, "/tmp/d3300.dcp");

   libdng_free(&dng);

Affected tags:

- TIFFTAG_COLOR_MATRIX_1
- TIFFTAG_COLOR_MATRIX_2
- TIFFTAG_FORWARD_MATRIX_1
- TIFFTAG_FORWARD_MATRIX_2
- TIFFTAG_CALIBRATION_ILLUMINANT_1
- TIFFTAG_CALIBRATION_ILLUMINANT_2
- TIFFTAG_PROFILE_TONE_CURVE
- TIFFTAG_PROFILE_HUE_SAT_MAP_DIMS
- TIFFTAG_PROFILE_HUE_SAT_MAP_DATA_1
- TIFFTAG_PROFILE_HUE_SAT_MAP_DATA_2


.. _set_exposure_program:

int libdng_set_exposure_program(libdng_info \*dng, uint16_t mode)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sets the exposure program used to take the picture. The intended values here
are the :code:`LIBDNG_EXPOSUREPROGRAM_` constants which maps to the numbers in
the DNG specification for the exposure programs:

.. code-block:: c

   #define LIBDNG_EXPOSUREPROGRAM_UNDEFINED 0
   #define LIBDNG_EXPOSUREPROGRAM_MANUAL 1
   #define LIBDNG_EXPOSUREPROGRAM_NORMAL 2
   #define LIBDNG_EXPOSUREPROGRAM_APERTURE_PRIORITY 3
   #define LIBDNG_EXPOSUREPROGRAM_SHUTTER_PRIORITY 4
   #define LIBDNG_EXPOSUREPROGRAM_CREATIVE 5
   #define LIBDNG_EXPOSUREPROGRAM_ACTION 6
   #define LIBDNG_EXPOSUREPROGRAM_PORTRAIT 7
   #define LIBDNG_EXPOSUREPROGRAM_LANDSCAPE 8

example use:

.. code-block:: c

   libdng_info dng = {0};
   libdng_new(&dng);

   libdng_set_exposure_program(&dng, LIBDNG_EXPOSUREPROGRAM_APERTURE_PRIORITY);

   libdng_free(&dng);

Affected tags:

- EXIFTAG_EXPOSUREPROGRAM


.. _write:

int libdng_write(libdng_info \*dng, const char \*path, unsigned int width, unsigned int height, const uint8_t \*data, size_t length)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is the function that takes the raw data and stores it into a DNG file together with all the metadata that has
been defined before this.


.. code-block:: c

   // This size is only valid for 8-bit pictures.
   size_t src_size = 1280 * 720;
   uint8_t *data = malloc(src_size);
   // At this point the raw data should be _in_ data.

   libdng_info dng = {0};
   libdng_new(&dng);
   if(!libdng_write(&dng, "/tmp/out.dng", 1280, 720, data, src_size)) {
     fprintf(stderr, "Could not write DNG\n");
     // handle errors here
   }

   free(data);
   libdng_free(&dng);