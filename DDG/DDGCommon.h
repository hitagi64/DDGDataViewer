#ifndef DDGCOMMON_H
#define DDGCOMMON_H
#include <memory>

struct DDGColorU8
{
    DDGColorU8()
    {
        r = 0;
        g = 0;
        b = 0;
        a = 0;
    }
    DDGColorU8(uint8_t x, uint8_t y, uint8_t z, uint8_t w)
    {
        this->r = x;
        this->g = y;
        this->b = z;
        this->a = w;
    }
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct DDGVector4
{
    DDGVector4()
    {
        x = 0;
        y = 0;
        z = 0;
        w = 0;
    }
    DDGVector4(float x, float y, float z, float w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    float x;
    float y;
    float z;
    float w;
};

struct DDGVector3
{
    DDGVector3()
    {
        x = 0;
        y = 0;
        z = 0;
    }
    DDGVector3(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    float x;
    float y;
    float z;
};

struct DDGVector2
{
    DDGVector2()
    {
        x = 0;
        y = 0;
    }
    DDGVector2(float x, float y)
    {
        this->x = x;
        this->y = y;
    }
    float x;
    float y;
};

#endif // DDGCOMMON_H
