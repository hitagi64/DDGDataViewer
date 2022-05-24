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

        i2f x2;
        x2.i = buffer.getU32(bufferCursor + 12);

        i2f y2;
        y2.i = buffer.getU32(bufferCursor + 16);

        i2f z2;
        z2.i = buffer.getU32(bufferCursor + 20);

        points.push_back(x.f);
        points.push_back(y.f);
        points.push_back(z.f);
        points.push_back(x2.f);
        points.push_back(y2.f);
        points.push_back(z2.f);

        bufferCursor += 76;
    }
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
    return DDGContent::getInfoAsString()
            + "\nPoints: " + std::to_string(points.size());
}

std::vector<float> DDGArea::getPoints()
{
    return points;
}
