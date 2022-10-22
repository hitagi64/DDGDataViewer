#ifndef DDGCONTENT_H
#define DDGCONTENT_H
#include <string>
#include "DDGMemoryBuffer.h"
#include "DDGLoadingConfig.h"

class DDGContent
{
public:
    DDGContent(DDGLoadingConfig config);
    virtual std::string getType();
    virtual void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    virtual DDGMemoryBuffer saveAsMemoryBuffer();

    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);

    virtual std::string getInfoAsString();

protected:
    DDGLoadingConfig config;
};

#endif // DDGCONTENT_H
