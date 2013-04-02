#include <iostream>

#include "gif.h"
#include "io.h"


int main()
{
    uint8_t data[] = {
0,0,0, 255,255,255,0,0,0,
255,255,255,0,0,0,255,255,255,
0,0,0, 255,255,255,0,0,0
};

    Gif *gif = Gif::LoadFromPixels(data, 3, 3);
    if (!gif) {
        std::cerr << "Could not load gif" << std::endl;
        return -1;
    }

    {
        BinaryWriter f("out.gif");
        gif->write(&f);
    }

    delete gif;
}
