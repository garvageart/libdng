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