#ifndef DDGENVIRONMENT_H
#define DDGENVIRONMENT_H
#include "DDGContent.h"
#include "DDGCommon.h"

class DDGEnvironment : public DDGContent
{
public:
    DDGEnvironment();

    std::string getType();
    void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    DDGMemoryBuffer saveAsMemoryBuffer();
    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);
    std::string getInfoAsString();

    std::vector<DDGVector3> getPoints();
private:
    std::vector<DDGVector3> points;
};

#endif // DDGENVIRONMENT_H
