#ifndef __GIF_H__
#define __GIF_H__

#include <stdint.h>

class BinaryWriter;

class Gif
{
public:
    static Gif *LoadFromPixels(const uint8_t *data, uint16_t width,
                               uint16_t height);
    void write(BinaryWriter *file);
private:
    Gif();


    uint16_t width;
    uint16_t height;
};

#endif
