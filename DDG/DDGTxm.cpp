#include "DDGTxm.h"

DDGTxm::DDGTxm()
{

}

std::string DDGTxm::getType()
{
    return "TXM";
}

void DDGTxm::loadFromMemoryBuffer(DDGMemoryBuffer buffer)
{
    misc1 = buffer.getU16(0x6);
    imagePixelType = (DDGTxmPixelFormat)buffer.getU8(0x0);
    imageWidth = buffer.getU16(0x2);
    imageHeight = buffer.getU16(0x4);

    misc2 = buffer.getU8(0x9);

    clutPixelType = (DDGTxmPixelFormat)buffer.getU8(0x8);
    clutWidth = buffer.getU16(0x0A);
    clutHeight = buffer.getU16(0x0C);

    misc3 = buffer.getU16(0xE);
}

DDGMemoryBuffer DDGTxm::saveAsMemoryBuffer()
{
    throw std::string("Saving DDGTxm to Memory Buffer not yet possible.");
    return DDGMemoryBuffer(0);
}

bool DDGTxm::possibleMatchForBuffer(DDGMemoryBuffer buffer)
{
    if (buffer.getU8(0) != buffer.getU8(0x1))
        return false;

    DDGTxmPixelFormat imagePixelType = (DDGTxmPixelFormat)buffer.getU8(0x0);
    uint16_t imageWidth = buffer.getU16(0x2);
    uint16_t imageHeight = buffer.getU16(0x4);

    DDGTxmPixelFormat clutPixelType = (DDGTxmPixelFormat)buffer.getU8(0x8);
    uint16_t clutWidth = buffer.getU16(0x0A);
    uint16_t clutHeight = buffer.getU16(0x0C);

    unsigned int totalClutSize = (clutWidth * clutHeight * getTxmPixelFormatBitCount(clutPixelType))/8;
    unsigned int totalImageSize = (imageWidth * imageHeight * getTxmPixelFormatBitCount(imagePixelType))/8;

    // Check if calculated size of block matches the actual size.
    // 16 is the header size
    if (buffer.getSize() == 16 + totalClutSize + totalImageSize)
        return true;

    return false;
}

std::string DDGTxm::getInfoAsString()
{
    return DDGContent::getInfoAsString()
             + "\nImage Pixel Format: " + txmPixelFormatAsString(imagePixelType)
             + "\nImage Width: " + std::to_string(imageWidth)
             + "\nImage Height: " + std::to_string(imageHeight)
            + "\nClut Pixel Format: " + txmPixelFormatAsString(clutPixelType)
            + "\nClut Width: " + std::to_string(clutWidth)
            + "\nClut Height: " + std::to_string(clutHeight);
}

uint8_t DDGTxm::getTxmPixelFormatBitCount(DDGTxmPixelFormat p)
{
    switch (p) {
    case PSMCT32:
        return 32;
        break;
    case PSMCT24:
        return 24;
        break;
    case PSMCT16:
        return 16;
        break;
    case PSMCT16S:
        return 16;
        break;
    case PSMT8:
        return 8;
        break;
    case PSMT4:
        return 4;
        break;
    case PSMT8H:
        return 8;
        break;
    case PSMT4HL:
        return 4;
        break;
    case PSMT4HH:
        return 4;
        break;
    case PSMZ32:
        return 32;
        break;
    case PSMZ24:
        return 24;
        break;
    case PSMZ16:
        return 16;
        break;
    case PSMZ16S:
        return 16;
        break;
    }
    return 0;
}

std::string DDGTxm::txmPixelFormatAsString(DDGTxmPixelFormat format)
{
    switch (format)
    {
    case PSMCT32:
        return "PSMCT32";
    case PSMCT24:
        return "PSMCT24";
    case PSMCT16:
        return "PSMCT16";
    case PSMCT16S:
        return "PSMCT16S";
    case PSMT8:
        return "PSMT8";
    case PSMT4:
        return "PSMT4";
    case PSMT8H:
        return "PSMT8H";
    case PSMT4HL:
        return "PSMT4HL";
    case PSMT4HH:
        return "PSMT4HH";
    case PSMZ32:
        return "PSMZ32";
    case PSMZ24:
        return "PSMZ24";
    case PSMZ16:
        return "PSMZ16";
    case PSMZ16S:
        return "PSMZ16S";
    default:
        return "Unknown";
    }
}
