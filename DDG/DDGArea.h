#ifndef DDGAREA_H
#define DDGAREA_H

#include "DDGContent.h"
#include "DDGCommon.h"

struct DDGTrackPiece
{
    DDGVector3 p1;
    DDGVector3 p2;

    int nextSpline;
    int previousSpline;

    int startDistance;
    int endDistance;

    int misc1;
    int misc2;
};

class DDGArea : public DDGContent
{
public:
    DDGArea();

    std::string getType();
    void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    DDGMemoryBuffer saveAsMemoryBuffer();
    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);
    std::string getInfoAsString();

    std::vector<float> getPoints();

private:
    std::vector<DDGTrackPiece> tracks;
};

#endif // DDGAREA_H
