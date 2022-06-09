#include "DDGEnvironment.h"

DDGEnvironment::DDGEnvironment()
{

}

std::string DDGEnvironment::getType()
{
    return "ENVIRONMENT";
}

void DDGEnvironment::loadFromMemoryBuffer(DDGMemoryBuffer buffer)
{

}

DDGMemoryBuffer DDGEnvironment::saveAsMemoryBuffer()
{
    throw std::string("Saving DDGEnvironment to Memory Buffer not yet possible.");
    return DDGMemoryBuffer(0);
}

bool DDGEnvironment::possibleMatchForBuffer(DDGMemoryBuffer buffer)
{
    return false;
}

std::string DDGEnvironment::getInfoAsString()
{
    return DDGContent::getInfoAsString()
            + "\nPoints: " + std::to_string(points.size());
}

std::vector<DDGVector3> DDGEnvironment::getPoints()
{
    return points;
}
