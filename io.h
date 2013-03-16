#ifndef __IO_H__
#define __IO_H__

#include <string>
#include <stdint.h>

class BinaryWriter
{
public:
    BinaryWriter(const std::string &filename);
    void writeCharacters(const std::string &characters);
    void writeUInt16(uint16_t val);
};

#endif
