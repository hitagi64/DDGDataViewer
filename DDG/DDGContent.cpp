#include "DDGContent.h"

std::string DDGContent::getType()
{
    return "Content";
}

void DDGContent::loadFromMemoryBuffer(DDGMemoryBuffer buffer)
{
    throw std::string("Can't load a DDGContent because its a base type.");
}

DDGMemoryBuffer DDGContent::saveAsMemoryBuffer()
{
    throw std::string("Can't save a DDGContent because its a base type.");
    return DDGMemoryBuffer(0);
}

bool DDGContent::possibleMatchForBuffer(DDGMemoryBuffer buffer)
{
    return false;
}

std::string DDGContent::getInfoAsString()
{
    return "Type: " + getType();
}
