#include "DDGTrackPoints.h"

DDGTrackPoints::DDGTrackPoints()
{

}

std::string DDGTrackPoints::getType()
{
    return "TRACKPOINTS";
}

void DDGTrackPoints::loadFromMemoryBuffer(DDGMemoryBuffer buffer)
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

DDGMemoryBuffer DDGTrackPoints::saveAsMemoryBuffer()
{
    throw std::string("Saving DDGTrackPoints to Memory Buffer not yet possible.");
    return DDGMemoryBuffer(0);
}

bool DDGTrackPoints::possibleMatchForBuffer(DDGMemoryBuffer buffer)
{
    return false;
}

std::string DDGTrackPoints::getInfoAsString()
{
    return DDGContent::getInfoAsString()
            + "\nPoints: " + std::to_string(points.size());
}

std::vector<DDGVector3> DDGTrackPoints::getPoints()
{
    return points;
}

std::vector<unsigned int> DDGTrackPoints::getPointTypes()
{
    return pointTypes;
}
