#ifndef DDGWORLDPOINTS_H
#define DDGWORLDPOINTS_H
#include "DDGContent.h"
#include "DDGCommon.h"

struct DDGWorldPoint
{
    DDGVector3 position;
    float rotation;
    uint16_t modelIndex;
};

class DDGWorldPoints : public DDGContent
{
public:
    DDGWorldPoints();

    std::string getType();
    void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    DDGMemoryBuffer saveAsMemoryBuffer();
    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);
    std::string getInfoAsString();

    std::vector<DDGWorldPoint> getPoints();
private:
    std::vector<DDGWorldPoint> points;
};

#endif // DDGWORLDPOINTS_H
