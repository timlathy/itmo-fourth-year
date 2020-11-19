#include "log2.h"

float log2_hw(XLog2_hw* instance, float x)
{
    union { float f; unsigned int i; } fp32;
    fp32.f = x;

    while (!XLog2_hw_IsReady(instance));
    XLog2_hw_Set_x(instance, fp32.i);
    XLog2_hw_Start(instance);
    while (!XLog2_hw_IsDone(instance));

    fp32.i = XLog2_hw_Get_return(instance);
    return fp32.f;
}

#ifdef TESTBENCH_SW
float log2_sw(float x)
{
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

    const float k[4] = {1.442547, -0.726980, 0.496404, -0.268344};
    float acc = fexp;
    float mult = signif;
    for (int i = 0; i < 4; ++i)
    {
        acc += k[i] * mult;
        mult *= signif;
    }
    return acc;
}
#endif
