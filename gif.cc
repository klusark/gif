#include "gif.h"
#include "io.h"

#include <vector>
#include <cstdio>
#include <cstring>

Gif::Gif()
{
}

Gif *Gif::LoadFromPixels(const uint8_t **data, uint16_t width, uint16_t height)
{
    uint8_t *d = (uint8_t *)malloc(width*height*3);
    for (int i = 0; i < height; ++i) {
        memcpy(d+(height*i*3), data[i], width*3);
    }
    Gif *gif = new Gif();
    gif->width = width;
    gif->height = height;
    gif->data = d;
    return gif;
}

void Gif::calculateData()
{
    colours.clear();
    int size = width * height;
    for (int i = 0; i < size; ++i) {
        uint32_t pixel = 0;
        pixel |= data[i * 3 + 0] << 0;
        pixel |= data[i * 3 + 1] << 8;
        pixel |= data[i * 3 + 2] << 16;
        int s = colours.size();
        bool found = false;
        for (int y = 0; y < s; ++y) {
            if (colours[y] == pixel) {
                found = true;
                pixels.push_back(y);
                break;
            }
        }
        if (found == false) {
            pixels.push_back(colours.size());
            colours.push_back(pixel);
        }
    }
    int tot = 0;
    int num = colours.size();
    if (num > 2)
        ++tot;
    if (num > 4)
        ++tot;
    if (num > 8)
        ++tot;
    if (num > 16)
        ++tot;
    if (num > 32)
        ++tot;
    if (num > 64)
        ++tot;
    if (num > 128)
        ++tot;

    colour_table_size = tot;
}

void Gif::write(BinaryWriter *file)
{
    calculateData();

    file->writeCharacters("GIF89a");
    file->writeUInt16(width);
    file->writeUInt16(height);

    uint8_t global_colour_table = 1;
    uint8_t colour_resolution = colour_table_size;
    uint8_t sort = 0;
    //colour_table_size = 1;

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

    /*file->writeUInt8(0x21);
    file->writeUInt8(0xf9);
    file->writeUInt8(0x04);
    file->writeUInt8(0x00);
    file->writeUInt8(0x00);
    file->writeUInt8(0x00);
    file->writeUInt8(0x00);
    file->writeUInt8(0x00);*/


    writeImage(file);

    file->writeUInt8(0x3b);

}

void Gif::writeColourTable(BinaryWriter *file)
{
    int size = 2 << colour_table_size;
    int i = 0;
    int num_colours = colours.size();
    for (i = 0; i < num_colours; ++i) {
        uint32_t colour = colours[i];
        uint8_t val = 0;
        val |= colour;
        file->writeUInt8(val);
        val = 0;

        val |= colour >> 8;
        file->writeUInt8(val);
        val = 0;

        val |= colour >> 16;
        file->writeUInt8(val);
    }
    for (; i < size; ++i) {
        file->writeUInt8(0);
        file->writeUInt8(0);
        file->writeUInt8(0);
    }

}

int search_table(std::vector<std::vector<uint32_t>> &table,
                 std::vector<uint32_t> &ck)
{
    int size = table.size();
    for (int i = 0; i < size; ++i) {
        int s = table[i].size();
        if (s != ck.size()) {
            continue;
        }
        bool good = true;
        for (int y = 0; y < s; ++y) {
            uint32_t val1 = table[i][y];
            uint32_t val2 = ck[y];
            if (val1 != val2) {
                good = false;
                break;
            }
        }
        if (good) {
            return i;
        }
    }
    return -1;
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



    int num_colours = 2 << (colour_table_size);
    std::vector<std::vector<uint32_t>> table;
    for (int i = 0; i < num_colours; ++i) {
       std::vector<uint32_t> val;
       val.push_back(i);
       table.push_back(val);
    }
    std::vector<uint32_t> a;
    table.push_back(a);
    table.push_back(a);

    std::vector<uint32_t> c;

    std::vector<uint32_t> out;
    std::vector<uint32_t> bits;

    uint32_t bit = 0;
    uint32_t total = 0;

    while (table.size()  > (1 << bit)) {
        ++bit;
    }

    file->writeUInt8(bit - 1);

    out.push_back(num_colours);
    bits.push_back(bit);
    total += bit;
    int num_pixel = pixels.size();
    for (int i = 0; i < num_pixel; ++i) {
        uint32_t k = pixels[i];
        std::vector<uint32_t> ck = c;
        ck.push_back(k);
        int pos = search_table(table, ck);
        if (pos != -1) {
            c.push_back(k);
        } else {
            table.push_back(ck);
            pos = search_table(table, c);
            if (table.size() > ((1<<bit) + 1)) {
                ++bit;
            }
            out.push_back(pos);
            bits.push_back(bit);
            /*if ((out.size() - 1) % 16 == 0) {
                printf("\n%05x: ", out.size() - 1);
            }*/
            //printf("%03x ", pos);
            //printf("#%03d %03d %03d\n", pos, bit, table.size());
            total += bit;
            c.clear();
            c.push_back(k);
        }
    }
    //if (c.size() != 1) {
        int pos = search_table(table, c);
        out.push_back(pos);
        bits.push_back(bit);
        total += bit;
    //}
    out.push_back(num_colours + 1);
    bits.push_back(bit);
    total += bit;

    /*int outnum = out.size();
    for (int i = 0; i < outnum; ++i) {
        printf("#%d %d\n", out[i], bits[i]);
    }*/

    int block = total/8;
    if ((total % 8) != 0) {
        block += 1;
    }
    file->writeStream(out, bits, block);
    file->writeUInt8(0);
}
