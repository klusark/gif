#include "gif.h"
#include "io.h"

#include <vector>
#include <cstdio>

Gif::Gif()
{
}

Gif *Gif::LoadFromPixels(const uint8_t *data, uint16_t width, uint16_t height)
{
    Gif *gif = new Gif();
    gif->width = 10;
    gif->height = 10;
    return gif;
}

void Gif::write(BinaryWriter *file)
{
    file->writeCharacters("GIF89a");
    file->writeUInt16(width);
    file->writeUInt16(height);

    uint8_t global_colour_table = 1;
    uint8_t colour_resolution = 1;
    uint8_t sort = 0;
    colour_table_size = 1;

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

    file->writeUInt8(0x21);
    file->writeUInt8(0xf9);
    file->writeUInt8(0x04);
    file->writeUInt8(0x00);
    file->writeUInt8(0x00);
    file->writeUInt8(0x00);
    file->writeUInt8(0x00);
    file->writeUInt8(0x00);

    writeImage(file);

    file->writeUInt8(0x3b);

}

void Gif::writeColourTable(BinaryWriter *file)
{
    int size = 2 << colour_table_size;
    file->writeUInt8(255);
    file->writeUInt8(255);
    file->writeUInt8(255);

    file->writeUInt8(0);
    file->writeUInt8(0);
    file->writeUInt8(255);

    file->writeUInt8(255);
    file->writeUInt8(0);
    file->writeUInt8(0);

    file->writeUInt8(0);
    file->writeUInt8(0);
    file->writeUInt8(0);
/*    for (int i = 0; i < size; ++i) {
        file->writeUInt8(255);
        file->writeUInt8(255);
        file->writeUInt8(255);
    }*/
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

    uint32_t data[] = {
1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
1, 1, 1, 0, 0, 0, 0, 2, 2, 2,
1, 1, 1, 0, 0, 0, 0, 2, 2, 2,
2, 2, 2, 0, 0, 0, 0, 1, 1, 1,
2, 2, 2, 0, 0, 0, 0, 1, 1, 1,
2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
};

    file->writeUInt8(2);

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
    int i = 0;

    std::vector<uint32_t> out;
    out.push_back(num_colours);
    while (i < 100) {
        uint32_t k = data[i];
        std::vector<uint32_t> ck = c;
        ck.push_back(k);
        int pos = search_table(table, ck);
        if (pos != -1) {
            c = ck;
        } else {
            table.push_back(ck);
            pos = search_table(table, c);
            //printf("#%d\n", pos);
            out.push_back(pos);
            c.clear();
            c.push_back(k);
        }
        ++i;
    }
    if (c.size() != 1) {
        int pos = search_table(table, c);
        out.push_back(pos);
    }
    out.push_back(num_colours + 1);

    int outsize = out.size();
    for (int i = 0; i < outsize; ++i) {
        //printf("#%d\n", out[i]);
    }

    file->writeUInt8(13);
    file->writeStream(out, 3);
    file->writeUInt8(0);
}
