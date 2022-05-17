#include "DDGDat.h"
#include "DDGTxm.h"

DDGDat::DDGDat()
{

}

std::string DDGDat::getType()
{
    return "DAT";
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
    if (buffer.getU8(0) != 'D' ||
        buffer.getU8(1) != 'A' ||
        buffer.getU8(2) != 'T' ||
        buffer.getU8(3) != '\0')
        throw std::string("Dat file header magic sequence not found.");
    objectCount = buffer.getU32(4);
    for (int i = 0; i < objectCount; i++)
    {
        uint32_t offset = buffer.getU32(8 + (i*8));
        uint32_t size = buffer.getU32(8 + (i*8) + 4);
        DDGMemoryBuffer subBuf = buffer.getPortion(offset, offset+size);

        bool match;
        std::shared_ptr<DDGContent> obj = findAndLoadContentFromBuffer(subBuf, match);
        objects.push_back(obj);
    }
}

DDGMemoryBuffer DDGDat::saveAsMemoryBuffer()
{
    throw std::string("Saving DDGDat to Memory Buffer not yet possible.");
    return DDGMemoryBuffer(0);
}

bool DDGDat::possibleMatchForBuffer(DDGMemoryBuffer buffer)
{
    if (buffer.getU8(0) != 'D' ||
        buffer.getU8(1) != 'A' ||
        buffer.getU8(2) != 'T' ||
        buffer.getU8(3) != '\0')
        return false;
    return true;
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
    else
    {
        foundMatch = false;
        return std::make_shared<DDGContent>();
    }
}
