#include "gif.h"
#include "io.h"

Gif::Gif()
{
}

Gif *Gif::LoadFromPixels(const uint8_t *data, uint16_t width, uint16_t height)
{
    Gif *gif = new Gif();
    return gif;
}

void Gif::write(BinaryWriter *file)
{
    file->writeCharacters("GIF89a");
    file->writeUInt16(width);
    file->writeUInt16(height);
}

