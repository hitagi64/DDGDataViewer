#ifndef DDGTRACK_H
#define DDGTRACK_H

#include "DDGContent.h"
#include "DDGCommon.h"

struct DDGTrackPiece
{
    DDGVector3 p1;
    DDGVector3 p2;

    int nextTrack;
    int previousTrack;

    int startDistance;
    int endDistance;

    int beginTrackPointIndex;
    int endTrackPointIndex;
};

class DDGTrack : public DDGContent
{
public:
    DDGTrack();

    std::string getType();
    void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    DDGMemoryBuffer saveAsMemoryBuffer();
    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);
    std::string getInfoAsString();

    std::vector<float> getPoints();

private:
    std::vector<DDGTrackPiece> tracks;
};

#endif // DDGTRACK_H
