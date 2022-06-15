#ifndef DDGTEST_H
#define DDGTEST_H
#include "DDGContent.h"
#include "DDGCommon.h"

class DDGTest : public DDGContent
{
public:
    DDGTest();

    std::string getType();
    void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    DDGMemoryBuffer saveAsMemoryBuffer();
    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);
    std::string getInfoAsString();
    std::vector<DDGVector3> getPoints();
private:
    std::vector<DDGVector3> points;
};

#endif // DDGTEST_H
