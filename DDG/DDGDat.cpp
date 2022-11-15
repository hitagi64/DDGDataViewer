#include "DDGDat.h"
#include "DDGTxm.h"
#include "DDGPdb.h"
#include "DDGTrack.h"
#include "DDGTrackPoints.h"
#include "DDGWorldPoints.h"
#include "DDGMapModelLUT.h"
#include <fstream>
#include <cstring>

DDGDat::DDGDat(DDGLoadingConfig config) : DDGContent(config)
{
    containsMapData = false;
}

std::string DDGDat::getType()
{
    if (!containsMapData)
        return "DAT";
    else
        return "DAT (MAPDATA)";
}

void DDGDat::loadFromFile(std::string filename)
{
    DDGMemoryBuffer buffer(filename);
    loadFromMemoryBuffer(buffer);
}

void DDGDat::saveToFile(std::string filename)
{
    DDGMemoryBuffer outBuffer = saveAsMemoryBuffer();
    if (outBuffer.getSize() == 0 || outBuffer.getPtr() == 0)
        throw std::runtime_error("Dat file was empty and cannot be saved.");
    outBuffer.saveToFile(filename);
}

void DDGDat::loadFromMemoryBuffer(DDGMemoryBuffer buffer)
{
    if (config.keepLoadedData)
        savedData = buffer;

    uint32_t baseDatOffset = 0;

    // It seems that model files in mapdata have dat objects in their
    //  dat files that have a 4 byte offset. This seems to be the case
    //  only with files in mapdata, so I will associate offsetted dats
    //  as dats containing mapdata.
    // I do not know what the value in the first 4 bytes is.
    // Also these offsetted dats seem to always have 8 entries in similar
    //  format, which is convenient.
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
        throw std::runtime_error("Dat file header not found.");

    objectCount = buffer.getU32(baseDatOffset + 4);
    for (int i = 0; i < objectCount; i++)
    {
        uint32_t offset = buffer.getU32(baseDatOffset + 8 + (i*8)) + baseDatOffset;
        uint32_t size = buffer.getU32(baseDatOffset + 8 + (i*8) + 4);
        if (size == 0)
            continue;
        DDGMemoryBuffer subBuf = buffer.getPortion(offset, offset+size);

        if (isThisDatAreapacMapData())
        {
            if (i == 0)
            {
                std::shared_ptr<DDGContent> obj = std::make_shared<DDGTrack>(config);
                obj->loadFromMemoryBuffer(subBuf);
                objects.push_back(obj);
            }
            else if (i == 1 || i == 2)
            {
                std::shared_ptr<DDGContent> obj = std::make_shared<DDGTrackPoints>(config);
                obj->loadFromMemoryBuffer(subBuf);
                objects.push_back(obj);
            }
            else if (i == 7)
            {
                std::shared_ptr<DDGContent> obj = std::make_shared<DDGWorldPoints>(config);
                obj->loadFromMemoryBuffer(subBuf);
                objects.push_back(obj);
            }
            else
            {
                bool match;
                std::shared_ptr<DDGContent> obj = findAndCreateFromBuffer(config, subBuf, match);
                obj->loadFromMemoryBuffer(subBuf);
                objects.push_back(obj);
            }
        }
        else
        {
            bool match;
            std::shared_ptr<DDGContent> obj = findAndCreateFromBuffer(config, subBuf, match);
            obj->loadFromMemoryBuffer(subBuf);
            objects.push_back(obj);

            /*if (i == 2 && isThisDatAreapac())
            {
                std::shared_ptr<DDGContent> obj = std::make_shared<DDGMapModelLUT>();
                obj->loadFromMemoryBuffer(subBuf);
                objects.push_back(obj);
            }
            else
            {
                bool match;
                std::shared_ptr<DDGContent> obj = findAndLoadContentFromBuffer(subBuf, match);
                objects.push_back(obj);
            }*/
        }
    }
}

DDGMemoryBuffer DDGDat::saveAsMemoryBuffer()
{
    std::vector<DDGMemoryBuffer> datItems;
    for (auto &obj : objects)
        datItems.push_back(obj->saveAsMemoryBuffer());

    std::vector<uint32_t> alignedSizes;

    uint32_t bufferSize = 0;

    // Header
    if (containsMapData)
        bufferSize += 8;
    else
        bufferSize += 4;

    bufferSize += 4;// Object count
    bufferSize += datItems.size() * 8;// Offsets and sizes

    // 16 align the dat header
    if ((bufferSize%16) != 0)
        bufferSize += 16-(bufferSize%16);

    uint32_t headerSize = bufferSize;

    for (auto &datItem : datItems)
    {
        uint32_t itemSize = datItem.getSize();

        // 16 align the item blocks, because that is what the game seems to do.
        if (((bufferSize+itemSize)%16) != 0)
            itemSize += 16-((bufferSize+itemSize)%16);

        alignedSizes.push_back(itemSize);
        bufferSize += itemSize;
    }

    DDGMemoryBuffer result(bufferSize);
    std::memset(result.getPtr(), 0, result.getSize());

    uint32_t bufferCursor = 0;

    // Header
    if (containsMapData)
    {
        result.setU8('D', 4);
        result.setU8('A', 5);
        result.setU8('T', 6);
        result.setU8('\0', 7);
        bufferCursor += 8;
    }
    else
    {
        result.setU8('D', 0);
        result.setU8('A', 1);
        result.setU8('T', 2);
        result.setU8('\0', 3);
        bufferCursor += 4;
    }

    // Object count
    result.setU32(datItems.size(), bufferCursor);
    bufferCursor += 4;

    // Offsets, sizes and objects
    uint32_t bufferObjectDataCursor = headerSize;
    for (int i = 0; i < alignedSizes.size(); i++)
    {
        // Mapdata dats have 4 bytes at the beginning that the game doesn't count.
        if (containsMapData)
            result.setU32(bufferObjectDataCursor-4, bufferCursor);
        else
            result.setU32(bufferObjectDataCursor, bufferCursor);
        result.setU32(alignedSizes[i], bufferCursor+4);
        bufferCursor += 8;
        bufferObjectDataCursor += alignedSizes[i];
        std::memcpy(result.getPtr()+bufferObjectDataCursor,
                    datItems[i].getPtr(),
                    datItems[i].getSize()
                    );
    }

    return result;
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
    return DDGContent::getInfoAsString() +
            "\nObject Count: " + std::to_string(objectCount);
}

std::vector<std::shared_ptr<DDGContent> > DDGDat::getObjects()
{
    return objects;
}

std::shared_ptr<DDGContent> DDGDat::findAndCreateFromBuffer(DDGLoadingConfig config, DDGMemoryBuffer buffer, bool &foundMatch)
{
    // The order of this list is based on how accurate the type detection is.

    foundMatch = true;
    if (DDGDat::possibleMatchForBuffer(buffer))
        return std::make_shared<DDGDat>(config);
    else if (DDGTxm::possibleMatchForBuffer(buffer))
        return std::make_shared<DDGTxm>(config);
    else if (DDGPdb::possibleMatchForBuffer(buffer))
        return std::make_shared<DDGPdb>(config);
    else if (DDGMapModelLUT::possibleMatchForBuffer(buffer))
        return std::make_shared<DDGMapModelLUT>(config);

    foundMatch = false;
    return std::make_shared<DDGContent>(config);
}

bool DDGDat::isThisDatAreapac()
{
    if (objectCount != 4)
        return false;

    if (objects.size() < 2)
        return false;

    DDGDat *cD1 = dynamic_cast<DDGDat*>(objects[0].get());
    if (cD1 == nullptr)
        return false;

    if (cD1->objects.size() != 9)
        return false;

    DDGDat *cD2 = dynamic_cast<DDGDat*>(objects[1].get());
    if (cD2 == nullptr)
        return false;

    if (cD2->getObjects().size() < 1)
        return false;

    DDGPdb *cD2Pdb = dynamic_cast<DDGPdb*>(cD2->getObjects()[0].get());
    if (cD2Pdb == nullptr)
        return false;

    return true;
}

bool DDGDat::isThisDatAreapacMapData()
{
    return containsMapData && objectCount == 9;
}
