#ifndef DDGDAT_H
#define DDGDAT_H
#include <string>
#include "DDGContent.h"

class DDGDat : public DDGContent
{
public:
    DDGDat(DDGLoadingConfig config);

    virtual std::string getType();

    void loadFromFile(std::string filename);///< Load .dat from file
    void saveToFile(std::string filename);

    void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    DDGMemoryBuffer saveAsMemoryBuffer();

    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);

    virtual std::string getInfoAsString();

    std::vector<std::shared_ptr<DDGContent>> getObjects();

    static std::shared_ptr<DDGContent> findAndCreateFromBuffer(DDGLoadingConfig config, DDGMemoryBuffer buffer, bool &foundMatch);
private:
    bool isThisDatAreapac();
    bool isThisDatAreapacMapData();
    uint32_t objectCount;
    std::vector<std::shared_ptr<DDGContent>> objects;
    bool containsMapData;
};

#endif // DDGDAT_H
