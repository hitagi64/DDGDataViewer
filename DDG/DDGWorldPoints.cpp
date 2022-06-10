#include "DDGWorldPoints.h"

DDGWorldPoints::DDGWorldPoints()
{

}

std::string DDGWorldPoints::getType()
{
    return "WORLDPOINTS";
}

void DDGWorldPoints::loadFromMemoryBuffer(DDGMemoryBuffer buffer)
{
    unsigned int bufferCursor = 0;

    typedef union {
        float f;
        uint32_t i;
    } i2f;

    while (bufferCursor < buffer.getSize())
    {
        i2f x;
        x.i = buffer.getU32(bufferCursor + 4);

        i2f y;
        y.i = buffer.getU32(bufferCursor + 8);

        i2f z;
        z.i = buffer.getU32(bufferCursor + 12);

        points.push_back(DDGVector3(x.f, y.f, z.f));

        bufferCursor += 24;
    }
}

DDGMemoryBuffer DDGWorldPoints::saveAsMemoryBuffer()
{
    throw std::string("Saving DDGWorldPoints to Memory Buffer not yet possible.");
    return DDGMemoryBuffer(0);
}

bool DDGWorldPoints::possibleMatchForBuffer(DDGMemoryBuffer buffer)
{
    return false;
}

std::string DDGWorldPoints::getInfoAsString()
{
    return DDGContent::getInfoAsString()
            + "\nPoints: " + std::to_string(points.size());
}

std::vector<DDGVector3> DDGWorldPoints::getPoints()
{
    return points;
}
