#include "DDGTest.h"

DDGTest::DDGTest()
{

}

std::string DDGTest::getType()
{
    return "TEST";
}

void DDGTest::loadFromMemoryBuffer(DDGMemoryBuffer buffer)
{
    int16_t itemCount = buffer.getU16(0);
    if (itemCount < 0)
        return;

    for (unsigned int i = 0; i < itemCount; i++)
    {
        entries.push_back(buffer.getU16(2 + (i*2)));
    }
}

DDGMemoryBuffer DDGTest::saveAsMemoryBuffer()
{
    throw std::string("Saving DDGTest to Memory Buffer not yet possible.");
    return DDGMemoryBuffer(0);
}

bool DDGTest::possibleMatchForBuffer(DDGMemoryBuffer buffer)
{
    return false;
}

std::string DDGTest::getInfoAsString()
{
    std::string result = DDGContent::getInfoAsString()
            + "\nCount: " + std::to_string(entries.size());
    return result;
}

std::vector<int16_t> DDGTest::getEntries()
{
    return entries;
}
