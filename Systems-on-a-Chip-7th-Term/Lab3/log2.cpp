#include "log2.h"
#include <hls_math.h>

/* A polynomial approximation of float log2(float).
 *
 * An IEEE 754 single-precision floating-point number x is represented as
 * sign (1 bit) exponent - 127 (8 bits) fraction (23 bits),
 * x = (-1)^sign * 2^(exp-127) * (1+frac)
 *
 * We ignore the sign bit (assume it to be 0) and compute log2(x) as:
 * log2(x) = log2(2^(exp-127) * (1+frac)) = log2(2^(exp-127)) + log2(1+frac) =
 *         = (exp-127) + log2(1+frac)
 *
 * The interval of [1,2) for (1+frac) is reduced to [0.75,1.5) for better approximation [1].
 *
 * Finally, a 4th degree polynomial is chosen to satisfy the assignment constraints (delta < 0.001),
 * with the coefficients a, b, c, d taken from [2].
 *
 * Sources:
 * [1] https://tech.ebayinc.com/engineering/fast-approximate-logarithms-part-i-the-basics/
 * [2] https://tech.ebayinc.com/engineering/fast-approximate-logarithms-part-iii-the-formulas/
 */
float log2_hw(float x)
{
    #pragma HLS INTERFACE s_axilite port=x
    #pragma HLS INTERFACE s_axilite port=return

    union { float f; unsigned int i; } fp32;

    fp32.f = x;
    // bexp = exp-127 (bits 23-30)
    int bexp = (fp32.i & 0x7F800000) >> 23;

    // signif = 1+frac, fexp = exp
    float signif, fexp;
    // Is the highest bit of frac set? Then significand >= 1.5
    if (fp32.i & 0x00400000)
    {
        // significand >= 1.5? Divide by 2 by setting exponent to -1
        fp32.i = (fp32.i & 0x007FFFFF) | 0x3f000000;
        signif = fp32.f;
        fexp = bexp - 126; // compensate for division by 2
    }
    else
    {
        // Set exponent to 0 to get significand
        fp32.i = (fp32.i & 0x007FFFFF) | 0x3f800000;
        signif = fp32.f;
        fexp = bexp - 127;
    }
    signif -= 1.0;


    const float k[4] = {1.442547, -0.726980, 0.496404, -0.268344};

    float signif2 = signif*signif;

    float dsigniffexp = hls::fmaf(k[0], signif, fexp);
    float rhs = hls::fmaf(k[1], signif2, dsigniffexp);

    float bsignif2 = k[2]*signif2;
    float bsignif3 = bsignif2*signif;
    float signif4 = signif2*signif2;
    float lhs = hls::fmaf(k[3], signif4, bsignif3);

    return lhs + rhs;

    /*
    float acc = fexp;
    float mult = signif;
    for (int i = 0; i < 4; ++i)
    {
        #pragma HLS unroll
        acc += k[i] * mult;
        mult *= signif;
    }
    return acc;
    */
}
