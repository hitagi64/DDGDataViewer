#ifndef DDGTRACKPOINTS_H
#define DDGTRACKPOINTS_H
#include "DDGContent.h"
#include "DDGCommon.h"

class DDGTrackPoints : public DDGContent
{
public:
    DDGTrackPoints();

    std::string getType();
    void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    DDGMemoryBuffer saveAsMemoryBuffer();
    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);
    std::string getInfoAsString();

    std::vector<DDGVector3> getPoints();
    std::vector<unsigned int> getPointTypes();
private:
    std::vector<DDGVector3> points;
    std::vector<unsigned int> pointTypes;
};

#endif // DDGTRACKPOINTS_H
