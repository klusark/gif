#include <cstdio>

#include "io.h"

BinaryWriter::BinaryWriter(const std::string &filename)
{
    FILE *f = fopen(filename.c_str(), "wb");
    fh = f;
}

BinaryWriter::~BinaryWriter()
{
    FILE *f = (FILE *)fh;
    fclose(f);
}

void BinaryWriter::write(const void *ptr, uint32_t size, uint32_t count)
{
    FILE *f = (FILE *)fh;
    fwrite(ptr, size, count, f);
}

void BinaryWriter::writeCharacters(const std::string &characters)
{
    write(characters.c_str(), 1, characters.length());
}

void BinaryWriter::writeUInt16(uint16_t val)
{
    write(&val, 2);
}

void BinaryWriter::writeUInt8(uint8_t val)
{
    write(&val, 1);
}

void BinaryWriter::writeStream(const std::vector<uint32_t> &data,
                               const std::vector<uint32_t> &bits)
{
    uint8_t out = 0;
    uint8_t out_bit = 0;
    uint8_t in_bit = 0;
    uint32_t count = data.size();
    for (int i = 0; i < count; ++i) {
        uint32_t d = data[i];
        for (int j = 0; j < bits[i]; ++j) {
            uint32_t val = (d & (1 << j)) >> j;
            out |= (val << out_bit);
            ++out_bit;
            if (out_bit == 8) {
                writeUInt8(out);
                out_bit = 0;
                out = 0;
            }
        }
    }
    if (out != 0) {
        writeUInt8(out);
    }
}
