#ifndef DDGMEMORYBUFFER_H
#define DDGMEMORYBUFFER_H
#include <memory>
#include <string>
#include <vector>

class DDGMemoryBuffer
{
public:
    DDGMemoryBuffer();
    DDGMemoryBuffer(size_t size);
    DDGMemoryBuffer(std::string file);
    DDGMemoryBuffer(std::shared_ptr<uint8_t> bufferData, size_t bufferSize, unsigned long bufferOffset);

    DDGMemoryBuffer getPortion(unsigned long begin, unsigned long end);

    uint8_t getU8(unsigned long offset);
    void setU8(unsigned long offset, uint8_t b);
    uint16_t getU16(unsigned long offset);
    void setU16(unsigned long offset, uint16_t b);
    uint32_t getU32(unsigned long offset);
    void setU32(unsigned long offset, uint32_t b);

    void saveToFile(std::string filename);

    size_t getSize();
    uint8_t *getPtr();

private:
    std::shared_ptr<uint8_t> bufferData;
    size_t bufferEnd;
    unsigned long bufferOffset;
};

#endif // DDGMEMORYBUFFER_H
