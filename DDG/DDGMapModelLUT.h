#ifndef DDGMAPMODELLUT_H
#define DDGMAPMODELLUT_H
#include "DDGContent.h"
#include "DDGCommon.h"

class DDGMapModelLUT : public DDGContent
{
public:
    DDGMapModelLUT(DDGLoadingConfig config);

    std::string getType();
    void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    DDGMemoryBuffer saveAsMemoryBuffer();
    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);
    std::string getInfoAsString();
    std::vector<int16_t> getEntries();
private:
    std::vector<int16_t> entries;
};

#endif // DDGMAPMODELLUT_H
