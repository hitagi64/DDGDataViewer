#include "DDGArea.h"

DDGArea::DDGArea()
{

}

std::string DDGArea::getType()
{
    return "AREA";
}

void DDGArea::loadFromMemoryBuffer(DDGMemoryBuffer buffer)
{

}

DDGMemoryBuffer DDGArea::saveAsMemoryBuffer()
{
    throw std::string("Saving DDGArea to Memory Buffer not yet possible.");
    return DDGMemoryBuffer(0);
}

bool DDGArea::possibleMatchForBuffer(DDGMemoryBuffer buffer)
{
    return false;
}

std::string DDGArea::getInfoAsString()
{
    return DDGContent::getInfoAsString();
}
