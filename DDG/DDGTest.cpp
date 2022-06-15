#include "DDGTest.h"

DDGTest::DDGTest()
{

}

std::string DDGTest::getType()
{
    return "TEST";
}

void DDGTest::loadFromMemoryBuffer(DDGMemoryBuffer buffer)
{
    unsigned int bufferCursor = 0;

    typedef union {
        float f;
        uint32_t i;
    } i2f;

    while (bufferCursor < buffer.getSize())
    {
        i2f x;
        x.i = buffer.getU32(bufferCursor + 12);

        i2f y;
        y.i = buffer.getU32(bufferCursor + 28);

        i2f z;
        z.i = buffer.getU32(bufferCursor + 44);
        /*i2f x;
        x.i = buffer.getU32(bufferCursor + 44);

        i2f y;
        y.i = buffer.getU32(bufferCursor + 48);

        i2f z;
        z.i = buffer.getU32(bufferCursor + 52);*/

        points.push_back(DDGVector3(x.f, y.f, z.f));

        bufferCursor += 56;
    }
}

DDGMemoryBuffer DDGTest::saveAsMemoryBuffer()
{
    throw std::string("Saving DDGTest to Memory Buffer not yet possible.");
    return DDGMemoryBuffer(0);
}

bool DDGTest::possibleMatchForBuffer(DDGMemoryBuffer buffer)
{
    return false;
}

std::string DDGTest::getInfoAsString()
{
    std::string result = DDGContent::getInfoAsString()
            + "\nCount: " + std::to_string(points.size());
    return result;
}

std::vector<DDGVector3> DDGTest::getPoints()
{
    return points;
}
