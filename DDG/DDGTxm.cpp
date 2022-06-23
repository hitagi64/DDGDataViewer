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
    int headerSize = 16;

    if (buffer.getU8(0) == 'T' &&
        buffer.getU8(1) == 'X' &&
        buffer.getU8(2) == 'D')
    {
        // Not present in DDG Shinkansen it seems.
        misc1 = 0;
        misc2 = 0;
        misc3 = 0;

        clutWidth = buffer.getU8(3);
        clutHeight = buffer.getU8(4);
        clutPixelType = (DDGTxmPixelFormat)buffer.getU8(5);

        imagePixelType = (DDGTxmPixelFormat)0x13;
        imageWidth = buffer.getU16(6);
        imageHeight = buffer.getU16(8);

        // Images in DDG Shinkansen have 20 byte header.
        headerSize = 20;
    }
    else
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

    // I am not sure but misc1 and misc3 might be texture id's.
    // This because they are referenced by pdb models.

    unsigned int totalClutSize = (clutWidth * clutHeight * getTxmPixelFormatBitCount(clutPixelType))/8;
    unsigned int totalImageSize = (imageWidth * imageHeight * getTxmPixelFormatBitCount(imagePixelType))/8;

    // Copy clut and image data
    clutData = DDGMemoryBuffer(totalClutSize);
    for (int i = 0; i < totalClutSize; i++)
    {
        clutData.setU8(buffer.getU8(headerSize + i), i);
    }

    imageData = DDGMemoryBuffer(totalImageSize);
    for (int i = 0; i < totalImageSize; i++)
    {
        imageData.setU8(buffer.getU8(headerSize + totalClutSize + i), i);
    }
}

DDGMemoryBuffer DDGTxm::saveAsMemoryBuffer()
{
    throw std::string("Saving DDGTxm to Memory Buffer not yet possible.");
    return DDGMemoryBuffer(0);
}

bool DDGTxm::possibleMatchForBuffer(DDGMemoryBuffer buffer)
{
    // DDG Shinkansen has textures prefixed with TXD and uses a different
    //  format than Final and Professional 2
    if (buffer.getU8(0) == 'T' &&
        buffer.getU8(1) == 'X' &&
        buffer.getU8(2) == 'D')
        return true;

    // Seems the first 2 bytes are often the same but not always
    //if (buffer.getU8(0) != buffer.getU8(0x1))
    //    return false;

    // Start of the 16 byte header.
    DDGTxmPixelFormat imagePixelType = (DDGTxmPixelFormat)buffer.getU8(0x0);
    uint16_t imageWidth = buffer.getU16(0x2);
    uint16_t imageHeight = buffer.getU16(0x4);

    DDGTxmPixelFormat clutPixelType = (DDGTxmPixelFormat)buffer.getU8(0x8);
    uint16_t clutWidth = buffer.getU16(0x0A);
    uint16_t clutHeight = buffer.getU16(0x0C);

    unsigned int totalClutSize = (clutWidth * clutHeight * getTxmPixelFormatBitCount(clutPixelType))/8;
    unsigned int totalImageSize = (imageWidth * imageHeight * getTxmPixelFormatBitCount(imagePixelType))/8;

    if (totalImageSize == 0)
        return false;

    if (getTxmPixelFormatBitCount(imagePixelType) <= 8)
        if (totalClutSize == 0)
            return false;

    // Check if calculated size of block matches or is slightly
    //  smaller than the actual size.
    // It can be slighly smaller because some files seem to be
    //  zero padded at the end. (To fill a page maybe?)
    if (buffer.getSize() >= 16 + totalClutSize + totalImageSize
        && buffer.getSize() < 16 + totalClutSize + totalImageSize + 2000)
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
            + "\nClut Height: " + std::to_string(clutHeight)
            + "\nMiscs: " + std::to_string(misc1) + ", " + std::to_string(misc2) + ", " + std::to_string(misc3);
}

DDGImage DDGTxm::convertToImage()
{
    DDGImage image;

    image.width = imageWidth;
    image.height = imageHeight;
    image.data = std::vector<uint8_t>(image.width * image.height * 4, 255);

    unsigned int totalImageSize = (imageWidth * imageHeight * getTxmPixelFormatBitCount(imagePixelType))/8;

    //uint8_t bpp = getTxmPixelFormatBitCount(imagePixelType);
    if (clutPixelType == PSMCT32 || clutPixelType == PSMCT16)
    {
        unsigned int i = 0;
        while(i < totalImageSize)
        {
            if (imagePixelType == PSMT4)
            {
                uint8_t doubleIndex = imageData.getU8(i);

                uint8_t firstIndex = doubleIndex & 0xF;
                uint8_t secondIndex = (doubleIndex >> 4) & 0xF;

                DDGColorU8 c1 = getColorFromClutAt(firstIndex);
                DDGColorU8 c2 = getColorFromClutAt(secondIndex);

                image.data[i*8]   = c1.r;
                image.data[i*8+1] = c1.g;
                image.data[i*8+2] = c1.b;
                image.data[i*8+3] = c1.a;

                image.data[i*8+4] = c2.r;
                image.data[i*8+5] = c2.g;
                image.data[i*8+6] = c2.b;
                image.data[i*8+7] = c2.a;

                i++;
            }
            else if (imagePixelType == PSMT8)
            {
                uint8_t index = imageData.getU8(i);

                DDGColorU8 c = getColorFromClutAt(index);

                image.data[i*4]   = c.r;
                image.data[i*4+1] = c.g;
                image.data[i*4+2] = c.b;
                image.data[i*4+3] = c.a;

                i++;
            }
            else if (imagePixelType == PSMCT32)
            {
                image.data[i] = imageData.getU8(i);

                i++;
            }
            else
                throw std::string("Cannot convert to image, pixel format " + txmPixelFormatAsString(imagePixelType) + " unspported.");
        }
    }
    else
        throw std::string("Cannot convert to image, bpp was 0.");


    return image;
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

DDGColorU8 DDGTxm::getColorFromClutAt(unsigned int index)
{
    DDGColorU8 c;
    c.r = 255;
    c.a = 255;
    if (clutPixelType == PSMCT32)
    {
        c.r = clutData.getU8(index*4);
        c.g = clutData.getU8(index*4+1);
        c.b = clutData.getU8(index*4+2);
        c.a = clutData.getU8(index*4+3);
    }
    else if (clutPixelType == PSMCT16)
    {
        uint32_t cr = clutData.getU16(index*2);
        c.r = (cr       & 31) << 3;
        c.g = ((cr>>5)  & 31) << 3;
        c.b = ((cr>>10) & 31) << 3;
        c.a = ((cr>>15) & 1) * 255;
    }
    return c;
}
