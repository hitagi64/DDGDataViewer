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

    uint8_t someBufferLength1 = buffer.getU8(bufferCursor+1);// Elements

    for (int i = 0; i < someBufferLength1; i++)
    {
        DDGU163Value v;
        v.a = buffer.getU16(bufferCursor+2 + (i*6));
        v.b = buffer.getU16(bufferCursor+2 + (i*6) + 2);
        v.c = buffer.getU16(bufferCursor+2 + (i*6) + 4);
        seg.beginBuf1.push_back(v);
    }

    // Move cursor past buffer that was just read.
    bufferCursor = bufferCursor+2+(someBufferLength1*6);

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

    // For some reason the vertex data is split up every
    //  so often so keep reading it until the end of
    //  vertexDataSize.
    while (bufferCursor < vertexDataSize)
    {
        // Every section of vertex data has a 96 byte header.

        // Vertices count is saved in 2 locations it seems and I can't
        //  find any files where the 2 have different values so I will
        //  assume they are the same and pick the first one.
        uint8_t verticesCount = buffer.getU8(bufferCursor + 64);// Vertices
        //vertices2Count = buffer.getU8(currentBufferCursor + 80);

        // Move past the header.
        bufferCursor += 96;

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

            seg.vertices.push_back({x.f, y.f, z.f, w.f});
        }

        // Move past vertices and skip the 4 bytes of data after it.
        bufferCursor += (verticesCount * 16) + 4;

        // Read the first array with 3x u16's.
        for (int i = 0; i < verticesCount; i++)
        {
            DDGU163Value v;
            v.a = buffer.getU16(bufferCursor + (i*6));

            v.b = buffer.getU16(bufferCursor + (i*6) + 2);

            v.c = buffer.getU16(bufferCursor + (i*6) + 4);

            seg.buf1.push_back(v);
        }

        // Move past first array of whatever and skip the 4 bytes
        //  of data after it again.
        bufferCursor += (verticesCount * 16) + 4;

        // Read the second array with 2x u16's.
        // I think these are uv's but I dont know in what format.
        for (int i = 0; i < verticesCount; i++)
        {
            DDGU162Value v;
            v.a = buffer.getU16(bufferCursor + (i*4));

            v.b = buffer.getU16(bufferCursor + (i*4) + 2);

            seg.buf2.push_back(v);
        }
    }

    return seg;
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

    ret += "\nSegment1 BeginBuf1 Element Count: " + std::to_string(segment1.beginBuf1.size());
    ret += "\nSegment1 Vertices Count: " + std::to_string(segment1.vertices.size());
    ret += "\nSegment1 Buf1 Element Count: " + std::to_string(segment1.buf1.size());
    ret += "\nSegment1 Buf2 Element Count: " + std::to_string(segment1.buf2.size());

    ret += "\nSegment2 BeginBuf1 Element Count: " + std::to_string(segment2.beginBuf1.size());
    ret += "\nSegment2 Vertices Count: " + std::to_string(segment2.vertices.size());
    ret += "\nSegment2 Buf1 Element Count: " + std::to_string(segment2.buf1.size());
    ret += "\nSegment2 Buf2 Element Count: " + std::to_string(segment2.buf2.size());

    ret += "\nSegment3 BeginBuf1 Element Count: " + std::to_string(segment3.beginBuf1.size());
    ret += "\nSegment3 Vertices Count: " + std::to_string(segment3.vertices.size());
    ret += "\nSegment3 Buf1 Element Count: " + std::to_string(segment3.buf1.size());
    ret += "\nSegment3 Buf2 Element Count: " + std::to_string(segment3.buf2.size());

    return ret;
}

std::vector<DDGVector4> DDGPdb::getBoundsVertices()
{
    return boundsVertices;
}
