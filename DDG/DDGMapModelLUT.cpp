#include "DDGMapModelLUT.h"
#include <stdexcept>

DDGMapModelLUT::DDGMapModelLUT(DDGLoadingConfig config) : DDGContent(config)
{
}

std::string DDGMapModelLUT::getType()
{
    return "MAPMODELLUT";
}

void DDGMapModelLUT::loadFromMemoryBuffer(DDGMemoryBuffer buffer)
{
    if (config.keepLoadedData)
        savedData = buffer;

    int16_t itemCount = buffer.getU16(0);
    if (itemCount < 0)
        return;

    for (unsigned int i = 0; i < itemCount; i++)
    {
        entries.push_back(buffer.getU16(2 + (i*2)));
    }
}

DDGMemoryBuffer DDGMapModelLUT::saveAsMemoryBuffer()
{
    if (config.keepLoadedData)
        return savedData;
    throw std::runtime_error("Saving DDGMapModelLUT to Memory Buffer not yet possible.");
}

bool DDGMapModelLUT::possibleMatchForBuffer(DDGMemoryBuffer buffer)
{
    int16_t itemCount = buffer.getU16(0);
    if (itemCount*2 >= buffer.getSize()-2)
        return false;
    if (itemCount*2 < buffer.getSize()-34)
        return false;
    if (itemCount > 20000)
        return false;
    if (itemCount == 0)
        return false;
    return true;
}

std::string DDGMapModelLUT::getInfoAsString()
{
    std::string result = DDGContent::getInfoAsString()
            + "\nCount: " + std::to_string(entries.size());
    return result;
}

std::vector<int16_t> DDGMapModelLUT::getEntries()
{
    return entries;
}
