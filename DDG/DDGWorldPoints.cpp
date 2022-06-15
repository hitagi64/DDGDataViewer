#include "DDGWorldPoints.h"
#include <iostream>
#include <map>

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

    std::map<uint16_t, uint16_t> modelIndexes;

    while (bufferCursor < buffer.getSize())
    {
        i2f x;
        x.i = buffer.getU32(bufferCursor + 4);

        i2f y;
        y.i = buffer.getU32(bufferCursor + 8);

        i2f z;
        z.i = buffer.getU32(bufferCursor + 12);

        i2f r;
        r.i = buffer.getU32(bufferCursor + 16);// Rotation as radians

        DDGWorldPoint wp;
        wp.position = DDGVector3(x.f, y.f, z.f);
        wp.modelIndex = buffer.getU16(bufferCursor + 20);
        wp.rotation = r.f;
        points.push_back(wp);
        modelIndexes[wp.modelIndex] = 0;

        bufferCursor += 24;
    }

    int i = 0;
    for (auto &index : modelIndexes)
    {
        index.second = i;
        i++;
    }

    for (DDGWorldPoint &p : points)
    {
        p.modelIndex = modelIndexes[p.modelIndex];
    }

    std::cout << "modelindexes size: " << modelIndexes.size() << std::endl;
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

std::vector<DDGWorldPoint> DDGWorldPoints::getPoints()
{
    return points;
}
