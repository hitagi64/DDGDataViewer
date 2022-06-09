#include "DDGDat.h"
#include "DDGTxm.h"
#include "DDGPdb.h"
#include "DDGTrack.h"
#include "DDG/DDGEnvironment.h"

DDGDat::DDGDat()
{
    containsMapData = false;
}

std::string DDGDat::getType()
{
    if (!containsMapData)
        return "DAT";
    else
        return "DAT (mapdata)";
}

void DDGDat::loadFromFile(std::string filename)
{
    DDGMemoryBuffer buffer(filename);
    loadFromMemoryBuffer(buffer);
}

void DDGDat::saveToFile(std::string filename)
{
    throw std::string("Saving DDGDat to file not yet possible.");
}

void DDGDat::loadFromMemoryBuffer(DDGMemoryBuffer buffer)
{
    uint32_t baseDatOffset = 0;

    // It seems that model files in mapdata have dat objects in their
    //  dat files that have a 4 byte offset. This seems to be the case
    //  only with files in mapdata, so I will associate offsetted dats
    //  as dats containing mapdata.
    // Also these offsetted dats seem to always have 8 entries in similar
    //  format, which is convinient.
    if (buffer.getU8(0) == 'D' &&
        buffer.getU8(1) == 'A' &&
        buffer.getU8(2) == 'T' &&
        buffer.getU8(3) == '\0')
    {
        baseDatOffset = 0;
        containsMapData = false;
    }
    else if (buffer.getU8(4) == 'D' &&
             buffer.getU8(5) == 'A' &&
             buffer.getU8(6) == 'T' &&
             buffer.getU8(7) == '\0')
    {
        baseDatOffset = 4;
        containsMapData = true;
    }
    else
        throw std::string("Dat file header magic sequence not found.");

    objectCount = buffer.getU32(baseDatOffset + 4);
    for (int i = 0; i < objectCount; i++)
    {
        uint32_t offset = buffer.getU32(baseDatOffset + 8 + (i*8)) + baseDatOffset;
        uint32_t size = buffer.getU32(baseDatOffset + 8 + (i*8) + 4);
        if (size == 0)
            continue;
        DDGMemoryBuffer subBuf = buffer.getPortion(offset, offset+size);

        if (containsMapData)
        {
            if (i == 0)
            {
                std::shared_ptr<DDGContent> obj = std::make_shared<DDGTrack>();
                obj->loadFromMemoryBuffer(subBuf);
                objects.push_back(obj);
            }
            else if (i == 1 || i == 2)
            {
                std::shared_ptr<DDGContent> obj = std::make_shared<DDGEnvironment>();
                obj->loadFromMemoryBuffer(subBuf);
                objects.push_back(obj);
            }
            else
            {
                bool match;
                std::shared_ptr<DDGContent> obj = findAndLoadContentFromBuffer(subBuf, match);
                objects.push_back(obj);
            }
        }
        else
        {
            bool match;
            std::shared_ptr<DDGContent> obj = findAndLoadContentFromBuffer(subBuf, match);
            objects.push_back(obj);
        }
    }
}

DDGMemoryBuffer DDGDat::saveAsMemoryBuffer()
{
    throw std::string("Saving DDGDat to Memory Buffer not yet possible.");
    return DDGMemoryBuffer(0);
}

bool DDGDat::possibleMatchForBuffer(DDGMemoryBuffer buffer)
{
    if (buffer.getU8(0) == 'D' &&
        buffer.getU8(1) == 'A' &&
        buffer.getU8(2) == 'T' &&
        buffer.getU8(3) == '\0')
        return true;
    if (buffer.getU8(4) == 'D' &&
        buffer.getU8(5) == 'A' &&
        buffer.getU8(6) == 'T' &&
        buffer.getU8(7) == '\0')
        return true;
    return false;
}

std::string DDGDat::getInfoAsString()
{
    return DDGContent::getInfoAsString() + "\nObject Count: " + std::to_string(objectCount);
}

std::vector<std::shared_ptr<DDGContent> > DDGDat::getObjects()
{
    return objects;
}

std::shared_ptr<DDGContent> DDGDat::findAndLoadContentFromBuffer(DDGMemoryBuffer buffer, bool &foundMatch)
{
    foundMatch = true;
    if (DDGDat::possibleMatchForBuffer(buffer))
    {
        std::shared_ptr<DDGContent> dat = std::make_shared<DDGDat>();
        dat->loadFromMemoryBuffer(buffer);
        return dat;
    }
    else if (DDGTxm::possibleMatchForBuffer(buffer))
    {
        std::shared_ptr<DDGContent> dat = std::make_shared<DDGTxm>();
        dat->loadFromMemoryBuffer(buffer);
        return dat;
    }
    else if (DDGPdb::possibleMatchForBuffer(buffer))
    {
        std::shared_ptr<DDGContent> dat = std::make_shared<DDGPdb>();
        dat->loadFromMemoryBuffer(buffer);
        return dat;
    }
    else
    {
        foundMatch = false;
        return std::make_shared<DDGContent>();
    }
}
