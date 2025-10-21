#include <stdio.h>

#include "libdng.h"

void
usage(char *name)
{
    fprintf(stderr, "Usage: %s src-file output-file\n", name);
    fprintf(stderr, "Read the main image from a DNG file and dump it as raw data\n\n");
}

int
main(int argc, char *argv[])
{
    libdng_init();
    libdng_info info = {0};
    libdng_new(&info);

    uint32_t width, height;
    uint8_t *image;
    size_t image_length;

    if (argc < 3) {
        fprintf(stderr, "Missing required argument\n");
        usage(argv[0]);
        return 1;
    }

    if (!libdng_read(&info, argv[1])) {
        fprintf(stderr, "Could not load the metadata from the original file\n");
        return 1;
    }

    if (!libdng_read_image(&info, argv[1], 1, &image, &image_length, &width, &height)) {
        fprintf(stderr, "Could not load image data from the original file\n");
        return 1;
    }
    printf("Got %dx%d image of %zu bytes\n", width, height, image_length);

    FILE *fp = fopen(argv[2] ,"wb");
    if (!fp) {
        fprintf(stderr, "Could not open output file\n");
        return 1;
    }
    fwrite(image, image_length, 1, fp);
    fclose(fp);

    return 0;
}
