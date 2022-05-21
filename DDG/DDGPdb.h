#ifndef DDGPDB_H
#define DDGPDB_H

#include "DDGContent.h"

struct DDGVector4
{
    float x;
    float y;
    float z;
    float w;
};

struct DDGU163Value
{
    uint16_t a;
    uint16_t b;
    uint16_t c;
};

struct DDGU162Value
{
    uint16_t a;
    uint16_t b;
};

struct DDGModelSegment
{
    std::vector<DDGU163Value> beginBuf1;

    std::vector<DDGVector4> vertices;
    std::vector<DDGU163Value> buf1;
    std::vector<DDGU162Value> buf2;
};

class DDGPdb : public DDGContent
{
public:
    DDGPdb();

    std::string getType();
    void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    DDGMemoryBuffer saveAsMemoryBuffer();
    DDGModelSegment readModelSegment(DDGMemoryBuffer buffer);
    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);
    std::string getInfoAsString();

    std::vector<DDGVector4> getBoundsVertices();
private:
    std::vector<DDGVector4> boundsVertices;

    DDGModelSegment segment1;
    DDGModelSegment segment2;
    DDGModelSegment segment3;
};

#endif // DDGPDB_H
