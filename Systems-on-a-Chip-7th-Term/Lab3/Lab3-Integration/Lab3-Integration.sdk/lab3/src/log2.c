#include "log2.h"

float log2_sw(float x)
{
    const float a = -0.268344;
    const float b = 0.496404;
    const float c = -0.726980;
    const float d = 1.442547;

    union { float f; unsigned int i; } fp32;

    fp32.f = x;
    int bexp = (fp32.i & 0x7F800000) >> 23;

    float signif, fexp;
    if (fp32.i & 0x00400000)
    {
        fp32.i = (fp32.i & 0x007FFFFF) | 0x3f000000;
        signif = fp32.f;
        fexp = bexp - 126;
    }
    else
    {
        fp32.i = (fp32.i & 0x007FFFFF) | 0x3f800000;
        signif = fp32.f;
        fexp = bexp - 127;
    }
    signif -= 1.0;

    float signif2 = signif*signif;
    return fexp + a*signif2*signif2 + b*signif2*signif + c*signif2 + d*signif;
}

