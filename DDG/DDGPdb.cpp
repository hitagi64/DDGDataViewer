#include "DDGPdb.h"

DDGPdb::DDGPdb()
{

}

std::string DDGPdb::getType()
{
    return "PDB";
}

void DDGPdb::loadFromMemoryBuffer(DDGMemoryBuffer buffer)
{
    // First header is 48 bytes long
    uint32_t verticesOffset1 = buffer.getU32(8);
    uint32_t verticesSize1 = buffer.getU32(12);

    uint32_t verticesDataOffset1 = buffer.getU32(16);
    uint32_t verticesDataSize1 = buffer.getU32(20);

    uint32_t verticesDataOffset2 = buffer.getU32(24);
    uint32_t verticesDataSize2 = buffer.getU32(28);

    uint32_t verticesDataOffset3 = buffer.getU32(32);
    uint32_t verticesDataSize3 = buffer.getU32(36);
}

DDGMemoryBuffer DDGPdb::saveAsMemoryBuffer()
{
    throw std::string("Saving DDGPdb to Memory Buffer not yet possible.");
    return DDGMemoryBuffer(0);
}

bool DDGPdb::possibleMatchForBuffer(DDGMemoryBuffer buffer)
{
    //if (buffer.getU32(0) != 0)
    //    return false;

    if (buffer.getU32(40) != 0xCBCAC9C8)
        return false;
    if (buffer.getU32(44) != 0xCFCECDCC)
        return false;

    return true;
}

std::string DDGPdb::getInfoAsString()
{
    return DDGContent::getInfoAsString();
}
