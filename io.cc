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
