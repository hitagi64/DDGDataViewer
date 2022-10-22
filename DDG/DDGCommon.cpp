#include "DDGCommon.h"

float fixedPoint2610BitToFloat(uint32_t v)
{
    int32_t vs = v;
    bool neg = vs < 0;
    if (neg)
        vs *= -1;
    float frac = vs & 1023;
    frac /= 1024;
    return ((vs >> 10)+frac) * (neg?-1.f:1.f);
}
