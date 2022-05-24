#ifndef DDGAREA_H
#define DDGAREA_H

#include "DDGContent.h"

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
    std::vector<float> points;
};

#endif // DDGAREA_H
