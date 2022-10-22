#ifndef DDGTRACKPOINTS_H
#define DDGTRACKPOINTS_H
#include "DDGContent.h"
#include "DDGCommon.h"

struct DDGTrackPoint
{
    DDGVector3 position;
    bool isOverheadWire;
};

class DDGTrackPoints : public DDGContent
{
public:
    DDGTrackPoints(DDGLoadingConfig config);

    std::string getType();
    void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    DDGMemoryBuffer saveAsMemoryBuffer();
    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);
    std::string getInfoAsString();

    std::vector<DDGTrackPoint> getPoints();
private:
    std::vector<DDGTrackPoint> points;
};

#endif // DDGTRACKPOINTS_H
