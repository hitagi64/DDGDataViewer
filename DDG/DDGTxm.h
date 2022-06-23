#ifndef DDGTXM_H
#define DDGTXM_H

#include "DDGContent.h"
#include "DDGCommon.h"
#include <QMetaEnum>

enum DDGTxmPixelFormat : uint8_t
{
    PSMCT32 = 0x00,
    PSMCT24 = 0x01,
    PSMCT16 = 0x02,
    PSMCT16S = 0x0a,
    PSMT8 = 0x13,
    PSMT4 = 0x14,
    PSMT8H = 0x1b,
    PSMT4HL = 0x24,
    PSMT4HH = 0x2c,
    PSMZ32 = 0x30,
    PSMZ24 = 0x31,
    PSMZ16 = 0x32,
    PSMZ16S = 0x3a
};

struct DDGImage
{
    int width;
    int height;
    // Data stored as RGBA32
    std::vector<uint8_t> data;
};

class DDGTxm : public DDGContent
{
public:
    DDGTxm();

    std::string getType();
    void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    DDGMemoryBuffer saveAsMemoryBuffer();
    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);
    std::string getInfoAsString();

    DDGImage convertToImage();
private:
    static uint8_t getTxmPixelFormatBitCount(DDGTxmPixelFormat p);
    static std::string txmPixelFormatAsString(DDGTxmPixelFormat format);

    inline DDGColorU8 getColorFromClutAt(unsigned int index);

    DDGTxmPixelFormat imagePixelType;
    uint16_t imageWidth;
    uint16_t imageHeight;

    uint16_t misc1;

    DDGTxmPixelFormat clutPixelType;
    uint16_t misc2;
    uint16_t clutWidth;
    uint16_t clutHeight;

    uint16_t misc3;

    DDGMemoryBuffer clutData;
    DDGMemoryBuffer imageData;
};

#endif // DDGTXM_H
