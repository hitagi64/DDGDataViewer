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
    std::vector<unsigned int> getPointTypes();
private:
    std::vector<DDGVector3> points;
    std::vector<unsigned int> pointTypes;
};

#endif // DDGENVIRONMENT_H
