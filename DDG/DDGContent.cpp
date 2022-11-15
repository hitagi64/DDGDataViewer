#include "DDGContent.h"

DDGContent::DDGContent(DDGLoadingConfig config)
{
    this->config = config;
}

std::string DDGContent::getType()
{
    return "CONTENT";
}

void DDGContent::loadFromMemoryBuffer(DDGMemoryBuffer buffer)
{
    if (config.keepLoadedData)
        savedData = buffer;
}

DDGMemoryBuffer DDGContent::saveAsMemoryBuffer()
{
    if (config.keepLoadedData)
        return savedData;
    throw std::runtime_error("Can't save a DDGContent because its a base type.");
}

bool DDGContent::possibleMatchForBuffer(DDGMemoryBuffer buffer)
{
    return false;
}

std::string DDGContent::getInfoAsString()
{
    std::string info = "Type: " + getType();
    if (getType() == "CONTENT")
        info += "\nSize: " + std::to_string(savedData.getSize());
    return info;
}
