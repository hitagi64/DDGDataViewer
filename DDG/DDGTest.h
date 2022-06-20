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
    std::vector<int16_t> getEntries();
private:
    std::vector<int16_t> entries;
};

#endif // DDGTEST_H
