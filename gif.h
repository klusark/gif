#ifndef __GIF_H__
#define __GIF_H__

#include <stdint.h>
#include <vector>

class BinaryWriter;

class Gif
{
public:
    static Gif *LoadFromPixels(const uint8_t **data, uint16_t width,
                               uint16_t height);
    void write(BinaryWriter *file);
    void addFrame(const uint8_t **data);
private:
    Gif();

    void writeColourTable(BinaryWriter *file);
    void writeImage(BinaryWriter *file, int img);

    void calculateData();

    std::vector<const uint8_t *> data;

    std::vector<uint32_t> colours;
    std::vector<std::vector<uint32_t>> pixels;
    uint16_t width;
    uint16_t height;
    uint8_t colour_table_size;
};

#endif
