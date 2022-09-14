#include <stdio.h>
#include <png.h>
#include <stdlib.h>
#include <string.h>
#include <webp/encode.h>

int main()
{
    png_image image;
    memset(&image, 0, sizeof(png_image));

    int ret_val = EXIT_SUCCESS;

    image.version = PNG_IMAGE_VERSION;

    int result = png_image_begin_read_from_file(&image, "reona_unknown.png");
    if (!result)
    {
        fprintf(stderr, "Failed to read header from PNG file");
        abort();
    }

    printf("Width: %d, Height: %d\r\n", image.width, image.height);

    image.format = PNG_FORMAT_RGBA;

    int row_stride = PNG_IMAGE_ROW_STRIDE(image);
    int px_size = PNG_IMAGE_PIXEL_COMPONENT_SIZE(image.format);

    int stride_bytes = row_stride * px_size;
    printf("Row stride: %d, pixel size: %d, stride in bytes: %d\r\n", row_stride, px_size, stride_bytes);

    int size = PNG_IMAGE_SIZE(image);
    printf("Buffer size: %d\r\n", size);

    char *buffer = (char *)malloc(size);
    memset(buffer, 0, size);

    result = png_image_finish_read(&image, NULL, buffer, row_stride, NULL);
    if (!result)
    {
        fprintf(stderr, "Failed to read image data to memory.");
        ret_val = EXIT_FAILURE;
        goto finish;
    }

    uint8_t *webp_output;
    size_t webp_size = WebPEncodeLosslessRGBA((const uint8_t*)buffer, image.width, image.height, stride_bytes, &webp_output);

    printf("WebP size: %lu\r\n", webp_size);

    FILE *out = fopen("reona_unknown.webp", "wb");
    fwrite(webp_output, webp_size, 1, out);

    fclose(out);

    WebPFree(webp_output);

finish:
    png_image_free(&image);
    free(buffer);

    return ret_val;
}