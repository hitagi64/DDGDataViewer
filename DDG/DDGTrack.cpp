#include "DDGTrack.h"

DDGTrack::DDGTrack(DDGLoadingConfig config) : DDGContent(config)
{

}

std::string DDGTrack::getType()
{
    return "TRACK";
}

void DDGTrack::loadFromMemoryBuffer(DDGMemoryBuffer buffer)
{
    if (config.keepLoadedData)
        savedData = buffer;

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
        if (!config.useFixedPoint)
        {
            track.p1 = DDGVector3(x.f, y.f, z.f);
            track.p2 = DDGVector3(x2.f, y2.f, z2.f);
        }
        else
        {
            track.p1 = DDGVector3(fixedPoint2610BitToFloat(x.i),
                                  fixedPoint2610BitToFloat(y.i),
                                  fixedPoint2610BitToFloat(z.i));
            track.p2 = DDGVector3(fixedPoint2610BitToFloat(x2.i),
                                  fixedPoint2610BitToFloat(y2.i),
                                  fixedPoint2610BitToFloat(z2.i));
        }

        track.nextTrack = buffer.getU32(bufferCursor + 24);
        track.previousTrack = buffer.getU32(bufferCursor + 32);

        track.startDistance = buffer.getU32(bufferCursor + 40);
        track.endDistance = buffer.getU32(bufferCursor + 44);

        track.beginTrackPointIndex = buffer.getU32(bufferCursor + 52);
        track.endTrackPointIndex = buffer.getU32(bufferCursor + 56);

        tracks.push_back(track);

        bufferCursor += 76;
    }
}

DDGMemoryBuffer DDGTrack::saveAsMemoryBuffer()
{
    if (config.keepLoadedData)
        return savedData;
    throw std::runtime_error("Saving DDGTrack to Memory Buffer not yet possible.");
}

bool DDGTrack::possibleMatchForBuffer(DDGMemoryBuffer buffer)
{
    return false;
}

std::string DDGTrack::getInfoAsString()
{
    return DDGContent::getInfoAsString()
            + "\nTracks: " + std::to_string(tracks.size());
}

std::vector<DDGTrackPiece> DDGTrack::getTracks()
{
    return tracks;
}
