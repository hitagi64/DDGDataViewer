#ifndef DDGPDB_H
#define DDGPDB_H

#include "DDGContent.h"

class DDGPdb : public DDGContent
{
public:
    DDGPdb();

    std::string getType();

    void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    DDGMemoryBuffer saveAsMemoryBuffer();

    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);

    std::string getInfoAsString();
};

#endif // DDGPDB_H
