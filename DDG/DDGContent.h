#ifndef DDGCONTENT_H
#define DDGCONTENT_H
#include <string>
#include <QFileDevice>
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
Q_DECLARE_METATYPE(DDGContent)

#endif // DDGCONTENT_H
