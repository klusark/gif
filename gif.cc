#include "gif.h"
#include "io.h"

Gif::Gif()
{
}

Gif *Gif::LoadFromPixels(const uint8_t *data, uint16_t width, uint16_t height)
{
    Gif *gif = new Gif();
    gif->width = width;
    gif->height = height;
    return gif;
}

void Gif::write(BinaryWriter *file)
{
    file->writeCharacters("GIF89a");
    file->writeUInt16(width);
    file->writeUInt16(height);

    uint8_t global_colour_table = 1;
    uint8_t colour_resolution = 7;
    uint8_t sort = 0;
    uint8_t colour_table_size = 7;

    uint8_t flags = 0;
    flags |= global_colour_table << 7;
    flags |= colour_resolution << 4;
    flags |= sort << 3;
    flags |= colour_table_size;
    file->writeUInt8(flags);


    uint8_t background_index = 0;
    file->writeUInt8(background_index);

    uint8_t aspect_ratio = 0;
    file->writeUInt8(aspect_ratio);

    writeColourTable(file);

    writeImage(file);

    file->writeUInt8(0x3b);

}

void Gif::writeColourTable(BinaryWriter *file)
{
    for (int i = 0; i < 256; ++i) {
        file->writeUInt8(0);
        file->writeUInt8(0);
        file->writeUInt8(0);
    }
}

void Gif::writeImage(BinaryWriter *file)
{
    //Image Seperator
    file->writeUInt8(0x2c);

    uint16_t left_pos = 0;
    uint16_t top_pos = 0;
    uint16_t img_width = width;
    uint16_t img_height = height;
    file->writeUInt16(left_pos);
    file->writeUInt16(top_pos);
    file->writeUInt16(img_width);
    file->writeUInt16(img_height);

    uint8_t flags = 0;
    uint8_t local_table = 0;
    uint8_t interlaced = 0;
    uint8_t sort = 0;
    uint8_t local_table_size = 0;
    flags |= local_table << 7;
    flags |= interlaced << 6;
    flags |= sort << 5;
    flags |= local_table_size;

    file->writeUInt8(flags);

    uint32_t data[] = {0x100, 0x28, 0xff, 0xff, 0xff, 0xff, 0x101};
    file->writeStream(data, 7, 9);
}
