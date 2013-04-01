#ifndef __IO_H__
#define __IO_H__

#include <string>
#include <stdint.h>
#include <vector>

class BinaryWriter
{
public:
    BinaryWriter(const std::string &filename);
    ~BinaryWriter();
    void writeCharacters(const std::string &characters);
    void writeUInt16(uint16_t val);
    void writeUInt8(uint8_t val);
    void writeStream(const std::vector<uint32_t> &data, uint32_t bits);

    void write(const void *ptr, uint32_t size, uint32_t count = 1);
private:
    void *fh;
};

#endif
