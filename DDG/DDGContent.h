#ifndef DDGCONTENT_H
#define DDGCONTENT_H
#include <string>
#include "DDGMemoryBuffer.h"

class DDGContent
{
public:
    virtual std::string getType();
    virtual void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    virtual DDGMemoryBuffer saveAsMemoryBuffer();

    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);

    virtual std::string getInfoAsString();
};

#endif // DDGCONTENT_H
