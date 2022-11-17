#include "DDGMemoryBuffer.h"
#include <fstream>

DDGMemoryBuffer::DDGMemoryBuffer()
{
    bufferData = std::shared_ptr<uint8_t>(new uint8_t[0], std::default_delete<uint8_t[]>());
    bufferEnd = 0;
    bufferOffset = 0;
}

DDGMemoryBuffer::DDGMemoryBuffer(size_t size)
{
    bufferData = std::shared_ptr<uint8_t>(new uint8_t[size], std::default_delete<uint8_t[]>());
    bufferEnd = size;
    bufferOffset = 0;
}

DDGMemoryBuffer::DDGMemoryBuffer(std::string file)
{
    std::ifstream f(file, std::ifstream::binary);
    if (!f.is_open())
    {
        bufferData = std::shared_ptr<uint8_t>(new uint8_t[0], std::default_delete<uint8_t[]>());
        bufferEnd = 0;
        bufferOffset = 0;
        return;
    }
    f.seekg(0, f.end);
    unsigned long length = f.tellg();
    f.seekg(0, f.beg);

    bufferData = std::shared_ptr<uint8_t>(new uint8_t[length], std::default_delete<uint8_t[]>());
    bufferEnd = length;
    bufferOffset = 0;

    f.read((char*)bufferData.get(), length);
    f.close();
}

DDGMemoryBuffer::DDGMemoryBuffer(std::shared_ptr<uint8_t> bufferData, size_t bufferSize, unsigned long bufferOffset)
{
    // These values are supposed to be verified by what assigns them
    this->bufferData = bufferData;
    this->bufferEnd = bufferSize;
    this->bufferOffset = bufferOffset;
}

DDGMemoryBuffer DDGMemoryBuffer::getPortion(unsigned long begin, unsigned long end)
{
    if (begin > end)
        throw std::runtime_error("Buffer portion begin can't be higher than it's end.");
    if (begin < 0 || begin >= getSize())
        throw std::runtime_error("Buffer not big enough to get portion of begin: " + std::to_string(begin) + " end: " + std::to_string(end));
    if (end < 0 || end > getSize())
        throw std::runtime_error("Buffer not big enough to get portion of begin: " + std::to_string(begin) + " end: " + std::to_string(end));
    return DDGMemoryBuffer(bufferData, bufferOffset + end, bufferOffset + begin);
}

uint8_t DDGMemoryBuffer::getU8(unsigned long offset)
{
    if (bufferOffset+offset >= 0 && bufferOffset+offset < bufferEnd)
        return bufferData.get()[bufferOffset+offset];
    return 0;
}

void DDGMemoryBuffer::setU8(unsigned long offset, uint8_t b)
{
    if (bufferOffset+offset >= 0 && bufferOffset+offset < bufferEnd)
        bufferData.get()[bufferOffset+offset] = b;
}

uint16_t DDGMemoryBuffer::getU16(unsigned long offset)
{
    return (((uint16_t)getU8(offset+1)) << 8) + ((uint16_t)getU8(offset));
}

void DDGMemoryBuffer::setU16(unsigned long offset, uint16_t b)
{
    setU8(offset, b & 0xff);
    setU8(offset+1, (b >> 8) & 0xff);
}

uint32_t DDGMemoryBuffer::getU32(unsigned long offset)
{
    uint32_t ret = getU8(offset+3) << 24;
    ret += getU8(offset+2) << 16;
    ret += getU8(offset+1) << 8;
    ret += getU8(offset);
    return ret;
}

void DDGMemoryBuffer::setU32(unsigned long offset, uint32_t b)
{
    setU8(offset, b & 0xff);
    setU8(offset+1, (b >> 8) & 0xff);
    setU8(offset+2, (b >> 16) & 0xff);
    setU8(offset+3, (b >> 24) & 0xff);
}

void DDGMemoryBuffer::saveToFile(std::string filename)
{
    std::ofstream outfile(filename, std::ios::binary);
    if (getPtr() == 0)
        throw std::runtime_error("DDGMemoryBuffer could not be saved.");
    if (getSize() != 0)
        outfile.write((char*)getPtr(), getSize());
    outfile.close();
}

size_t DDGMemoryBuffer::getSize()
{
    return bufferEnd-bufferOffset;
}

uint8_t *DDGMemoryBuffer::getPtr()
{
    return bufferData.get() + bufferOffset;
}
