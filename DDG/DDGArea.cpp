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

        DDGTrackPiece track;
        track.p1 = DDGVector3(x.f, y.f, z.f);
        track.p2 = DDGVector3(x2.f, y2.f, z2.f);

        track.nextSpline = buffer.getU32(bufferCursor + 24);
        track.previousSpline = buffer.getU32(bufferCursor + 32);

        track.startDistance = buffer.getU32(bufferCursor + 40);
        track.endDistance = buffer.getU32(bufferCursor + 44);

        track.misc1 = buffer.getU32(bufferCursor + 52);
        track.misc2 = buffer.getU32(bufferCursor + 56);

        bufferCursor += 76;
        tracks.push_back(track);
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
            + "\nTracks: " + std::to_string(tracks.size());
}

std::vector<float> DDGArea::getPoints()
{
    std::vector<float> points;
    for (DDGTrackPiece &track : tracks)
    {
        points.push_back(track.p1.x);
        points.push_back(track.p1.y);
        points.push_back(track.p1.z);
        if (track.misc1 == -1)
        {
            points.push_back(0);
            points.push_back(1.0f);
            points.push_back(0);
        }
        else
        {
            points.push_back(1);
            points.push_back(0);
            points.push_back(1);
        }

        points.push_back(track.p2.x);
        points.push_back(track.p2.y);
        points.push_back(track.p2.z);
        if (track.misc2 == -1)
        {
            points.push_back(0);
            points.push_back(1.0f);
            points.push_back(0);
        }
        else
        {
            points.push_back(1);
            points.push_back(0);
            points.push_back(1);
        }
    }
    return points;
}
