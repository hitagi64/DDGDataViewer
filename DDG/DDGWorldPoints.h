#ifndef DDGWORLDPOINTS_H
#define DDGWORLDPOINTS_H
#include "DDGContent.h"
#include "DDGCommon.h"

class DDGWorldPoints : public DDGContent
{
public:
    DDGWorldPoints();

    std::string getType();
    void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    DDGMemoryBuffer saveAsMemoryBuffer();
    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);
    std::string getInfoAsString();

    std::vector<DDGVector3> getPoints();
private:
    std::vector<DDGVector3> points;
};

#endif // DDGWORLDPOINTS_H
