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
    // Header is 48 bytes long

    // First 8 bytes are unknown

    // This points to an array that seems to contain
    //  vertices indicating the bounding box of the model.
    uint32_t boundsVerticesOffset = buffer.getU32(8);
    uint32_t boundsVerticesSize = buffer.getU32(12);// Bytes

    // Then 3 offsets and sizes that seem to all point to
    //  vertex data. I will call those segments later.
    uint32_t vertexDataOffset1 = buffer.getU32(16);
    uint32_t vertexDataSize1 = buffer.getU32(20);// Bytes

    uint32_t vertexDataOffset2 = buffer.getU32(24);
    uint32_t vertexDataSize2 = buffer.getU32(28);// Bytes

    uint32_t vertexDataOffset3 = buffer.getU32(32);
    uint32_t vertexDataSize3 = buffer.getU32(36);// Bytes

    // Last 8 bytes of the header always seem to be counting up from 0xC8 till 0xCF.

    // This assumes a float to be 4 bytes
    typedef union {
        float f;
        uint32_t i;
    } i2f;

    if (boundsVerticesSize % 16 != 0)
        throw std::string("Invalid size for vertices1.");

    for (int i = 0; i < boundsVerticesSize/16; i++)
    {
        i2f x;
        x.i = buffer.getU32(boundsVerticesOffset + (i*16));

        i2f y;
        y.i = buffer.getU32(boundsVerticesOffset + (i*16) + 4);

        i2f z;
        z.i = buffer.getU32(boundsVerticesOffset + (i*16) + 8);

        i2f w;
        w.i = buffer.getU32(boundsVerticesOffset + (i*16) + 12);

        boundsVertices.push_back({x.f, y.f, z.f, w.f});
    }

    // Read the vertex data from the 3 vertex buffers specified in the header
    if (vertexDataSize1 > 0)
        segment1 = readModelSegment(
                    buffer.getPortion(
                        vertexDataOffset1,
                        vertexDataOffset1 + vertexDataSize1)
                    );
    if (vertexDataSize2 > 0)
        segment2 = readModelSegment(
                    buffer.getPortion(
                        vertexDataOffset2,
                        vertexDataOffset2 + vertexDataSize2)
                    );
    if (vertexDataSize3 > 0)
        segment3 = readModelSegment(
                    buffer.getPortion(
                        vertexDataOffset3,
                        vertexDataOffset3 + vertexDataSize3)
                    );
}

DDGMemoryBuffer DDGPdb::saveAsMemoryBuffer()
{
    throw std::string("Saving DDGPdb to Memory Buffer not yet possible.");
    return DDGMemoryBuffer(0);
}

DDGModelSegment DDGPdb::readModelSegment(DDGMemoryBuffer buffer)
{
    DDGModelSegment seg;

    uint32_t bufferCursor = 0;

    // First array is a list of textures indexes and references to the
    //  textures misc numbers.
    uint8_t texturesLength = buffer.getU8(bufferCursor+1);// Elements
    for (int i = 0; i < texturesLength; i++)
    {
        DDGModelTexture v;
        v.textureIndex = buffer.getU16(bufferCursor+2 + (i*6));
        v.b = buffer.getU16(bufferCursor+2 + (i*6) + 2);
        v.c = buffer.getU16(bufferCursor+2 + (i*6) + 4);
        seg.textures.push_back(v);
    }

    // Move cursor past buffer that was just read.
    bufferCursor = bufferCursor+2+(texturesLength*6);

    // Seems that it wants to 16 align in the file so that is done here
    // The current buffer seems to be 16 aligned in the pdb so it aligns
    //  to that.
    if (bufferCursor % 16 != 0)
        bufferCursor += 16 - (bufferCursor%16);

    // From here is the 16 byte header for the vertex and some
    //  other stuff data.
    uint16_t vertexDataSize = buffer.getU16(bufferCursor);// Bytes
    // Skip the rest of the 16 bytes
    bufferCursor += 16;

    // Check if the buffer didn't suddenly end
    if (buffer.getSize() < bufferCursor)
        return seg;

    unsigned int vertexDataStart = bufferCursor;

    // For some reason the vertex data is split up every
    //  so often so keep reading it until the end of
    //  vertexDataSize.
    while (bufferCursor-vertexDataStart < vertexDataSize)
    {
        // Every section of vertex data has a 96 byte header.

        // Vertices count is saved in 2 locations it seems and I can't
        //  find any files where the 2 have different values so I will
        //  assume they are the same and pick the first one.
        uint8_t verticesCount = buffer.getU8(bufferCursor + 64);// Vertices
        //vertices2Count = buffer.getU8(currentBufferCursor + 80);

        // This texture id references an item in textures array in
        //  the DDGModelSegment object
        uint8_t textureID = buffer.getU8(bufferCursor + 24);

        // Move past the header.
        bufferCursor += 96;

        seg.vertexSegments.push_back(DDGVertexSegment());
        seg.vertexSegments.back().textureID = textureID;

        // Read the vertices
        typedef union {
            float f;
            uint32_t i;
        } i2f;
        for (int i = 0; i < verticesCount; i++)
        {
            i2f x;
            x.i = buffer.getU32(bufferCursor + (i*16));

            i2f y;
            y.i = buffer.getU32(bufferCursor + (i*16) + 4);

            i2f z;
            z.i = buffer.getU32(bufferCursor + (i*16) + 8);

            i2f w;
            w.i = buffer.getU32(bufferCursor + (i*16) + 12);

            seg.vertexSegments.back().vertices.push_back({x.f, y.f, z.f, w.f});
        }

        // Move past vertices and skip the 4 bytes of data after it.
        bufferCursor += (verticesCount * 16) + 4;

        // Read the first array with 3x u16's.
        for (int i = 0; i < verticesCount; i++)
        {
            DDGVector3 normal;
            uint16_t x = buffer.getU16(bufferCursor + (i*6));
            normal.x = fixedPoint412BitToFloat(x);

            uint16_t y = buffer.getU16(bufferCursor + (i*6) + 2);
            normal.y = fixedPoint412BitToFloat(y);

            uint16_t z = buffer.getU16(bufferCursor + (i*6) + 4);
            normal.z = fixedPoint412BitToFloat(z);

            seg.vertexSegments.back().normals.push_back(normal);
        }

        // Move past first array of whatever and skip the 4 bytes
        //  of data after it again.
        bufferCursor += (verticesCount * 6) + 4;

        // Read the array with UV's.
        // These are in fixed point format of 12 bit.
        for (int i = 0; i < verticesCount; i++)
        {
            DDGVector2 v;
            uint16_t x = buffer.getU16(bufferCursor + (i*4));
            v.x = fixedPoint412BitToFloat(x);

            uint16_t y = buffer.getU16(bufferCursor + (i*4) + 2);
            v.y = fixedPoint412BitToFloat(y);

            seg.vertexSegments.back().UVs.push_back(v);
        }

        // Move past second array of uv's.
        bufferCursor += (verticesCount * 4);

        // Files seem to 16 align here again
        if (bufferCursor % 16 != 0)
            bufferCursor += 16 - (bufferCursor%16);
    }

    return seg;
}

float DDGPdb::fixedPoint412BitToFloat(uint16_t v)
{
    int16_t resA = (v >> 12) & 15;
    //resA += resA & 32768;

    uint16_t resB = v & 4095;
    float resBF = resB / 4096.f;

    return ((float)resA) + resBF;
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
    std::string ret = DDGContent::getInfoAsString();
    ret += "\nBounds Vertices Count: " + std::to_string(boundsVertices.size());

    ret += "\nSegment1 Textures Count: " + std::to_string(segment1.textures.size());
    ret += "\nSegment1 Vertex Segment Count: " + std::to_string(segment1.vertexSegments.size());

    ret += "\nSegment2 Textures Count: " + std::to_string(segment2.textures.size());
    ret += "\nSegment2 Vertex Segment Count: " + std::to_string(segment2.vertexSegments.size());

    ret += "\nSegment3 Textures Count: " + std::to_string(segment3.textures.size());
    ret += "\nSegment3 Vertex Segment Count: " + std::to_string(segment3.vertexSegments.size());

    return ret;
}

std::vector<float> DDGPdb::convertSegmentToVertexArray(const DDGVertexSegment &segment)
{
    // Vertices will be in the format 3f pos, 3f norm
    std::vector<float> vertices;

    // There need to be the same amount of vertices and uv's.
    if (segment.vertices.size() != segment.UVs.size())
        return vertices;

    bool lastStripW = false;
    unsigned int stripCount = 0;// Triangles since strip begin
    // A strip will start with 2 vertices where the w is not exactly 0
    for (int i = 0; i < segment.vertices.size(); i++)
    {
        if (!lastStripW && segment.vertices[i].w > 0)
            stripCount = 0;
        stripCount++;
        lastStripW = segment.vertices[i].w > 0;

        if (stripCount >= 3 && i >= 2)
        {
            // Vertex 1
            vertices.push_back(segment.vertices[i-2].x);
            vertices.push_back(segment.vertices[i-2].y);
            vertices.push_back(segment.vertices[i-2].z);

            vertices.push_back(segment.normals[i-2].x);
            vertices.push_back(segment.normals[i-2].y);
            vertices.push_back(segment.normals[i-2].z);

            vertices.push_back(segment.UVs[i-2].x);
            vertices.push_back(segment.UVs[i-2].y);

            // Vertex 2
            vertices.push_back(segment.vertices[i-1].x);
            vertices.push_back(segment.vertices[i-1].y);
            vertices.push_back(segment.vertices[i-1].z);

            vertices.push_back(segment.normals[i-1].x);
            vertices.push_back(segment.normals[i-1].y);
            vertices.push_back(segment.normals[i-1].z);

            vertices.push_back(segment.UVs[i-1].x);
            vertices.push_back(segment.UVs[i-1].y);

            // Vertex 3
            vertices.push_back(segment.vertices[i].x);
            vertices.push_back(segment.vertices[i].y);
            vertices.push_back(segment.vertices[i].z);

            vertices.push_back(segment.normals[i].x);
            vertices.push_back(segment.normals[i].y);
            vertices.push_back(segment.normals[i].z);

            vertices.push_back(segment.UVs[i].x);
            vertices.push_back(segment.UVs[i].y);
        }
    }
    return vertices;
}

std::vector<DDGVector4> DDGPdb::getBoundsVertices()
{
    return boundsVertices;
}

DDGModelSegment DDGPdb::getModelSegment1()
{
    return segment1;
}

DDGModelSegment DDGPdb::getModelSegment2()
{
    return segment2;
}

DDGModelSegment DDGPdb::getModelSegment3()
{
    return segment3;
}
