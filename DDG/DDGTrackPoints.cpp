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

        unsigned int pointType = buffer.getU32(bufferCursor + 12);

        DDGTrackPoint point;
        point.position = DDGVector3(x.f, y.f, z.f);
        point.isOverheadWire = ((pointType>>29)&1) == 1;

        points.push_back(point);

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

std::vector<DDGTrackPoint> DDGTrackPoints::getPoints()
{
    return points;
}
