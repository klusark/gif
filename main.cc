#include "gif.h"
#include "io.h"

#include <png.h>
#include <iostream>

Gif *read_png(const char *filename)
{
    char header[8];

    FILE *file = fopen(filename, "rb");

    fread(header, 1, 8, file);
    png_sig_cmp((png_const_bytep)header, 0, 8);


    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);



    png_infop info = png_create_info_struct(png);

    if (setjmp(png_jmpbuf(png))) {
        std::cerr << "error" << std::endl;
        exit(-1);
    }


    png_init_io(png, file);
    png_set_sig_bytes(png, 8);

    png_read_info(png, info);

    int width = png_get_image_width(png, info);
    int height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png);
    }

    png_set_interlace_handling(png);
    png_read_update_info(png, info);

    if (setjmp(png_jmpbuf(png))) {
        std::cerr << "error" << std::endl;
        exit(-1);
    }

    png_bytep *row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
    for (int i = 0; i < height; ++i) {
        row_pointers[i] = (png_byte*) malloc(png_get_rowbytes(png, info));
    }

    png_read_image(png, row_pointers);

    fclose(file);

    Gif *gif = Gif::LoadFromPixels((const uint8_t **)row_pointers, width, height);
    return gif;
}


int main(int argc, const char **argv)
{
    const char *out = "out.gif";
    const char *in = "in.png";
    if (argc > 1) {
        in = argv[1];
    }
    if (argc > 2) {
        out = argv[2];
    }
    Gif *gif = read_png(in);
    //Gif *gif = Gif::LoadFromPixels(data, 10, 10);
    if (!gif) {
        std::cerr << "Could not load gif" << std::endl;
        return -1;
    }

    {
        BinaryWriter f(out);
        gif->write(&f);
    }

    delete gif;
}
