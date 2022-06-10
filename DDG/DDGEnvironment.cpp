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
    unsigned int bufferCursor = 0;

    typedef union {
        float f;
        uint32_t i;
    } i2f;

    while (bufferCursor < buffer.getSize())
    {
        i2f x;
        x.i = buffer.getU32(bufferCursor);

        i2f y;
        y.i = buffer.getU32(bufferCursor + 4);

        i2f z;
        z.i = buffer.getU32(bufferCursor + 8);

        unsigned int misc = buffer.getU32(bufferCursor + 12);

        points.push_back(DDGVector3(x.f, y.f, z.f));
        pointTypes.push_back(misc);

        bufferCursor += 16;
    }
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

std::vector<unsigned int> DDGEnvironment::getPointTypes()
{
    return pointTypes;
}
