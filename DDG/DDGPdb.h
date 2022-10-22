#ifndef DDGPDB_H
#define DDGPDB_H

#include "DDGContent.h"
#include "DDGCommon.h"

struct DDGModelTexture
{
    uint16_t textureIndex;// Refers to texture kept in a dat somewhere else
    uint16_t b;// Unknown
    uint16_t c;// Unknown
};

struct DDGVertexSegment
{
    uint8_t textureID = 0;
    std::vector<DDGVector4> vertices;
    std::vector<DDGVector3> normals;
    std::vector<DDGVector2> UVs;
};

struct DDGModelSegment
{
    std::vector<DDGModelTexture> textures;

    std::vector<DDGVertexSegment> vertexSegments;
};

class DDGPdb : public DDGContent
{
public:
    DDGPdb(DDGLoadingConfig config);

    std::string getType();
    void loadFromMemoryBuffer(DDGMemoryBuffer buffer);
    DDGMemoryBuffer saveAsMemoryBuffer();
    static bool possibleMatchForBuffer(DDGMemoryBuffer buffer);
    std::string getInfoAsString();

    std::vector<DDGVector4> getBoundsVertices();
    DDGModelSegment getModelSegment1();
    DDGModelSegment getModelSegment2();
    DDGModelSegment getModelSegment3();
    static std::vector<float> convertSegmentToVertexArray(const DDGVertexSegment &segment);
private:
    std::vector<DDGVector4> boundsVertices;

    DDGModelSegment segment1;
    DDGModelSegment segment2;
    DDGModelSegment segment3;

    DDGModelSegment readModelSegment(DDGMemoryBuffer buffer);
    float fixedPoint412BitToFloat(uint16_t v);
};

#endif // DDGPDB_H
