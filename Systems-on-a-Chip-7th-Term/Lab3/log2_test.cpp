#include <cmath>
#include <iostream>
#include "log2.h"

void verify_log2x(float x, float* max_delta)
{
    float expected = log2f(x);
    float actual = log2_hw(x);
    float delta = fabs(expected - actual);
    if (delta > 0.001f)
    {
        std::cerr << "ERROR: log2(" << x << ") = " << actual << ", delta = " << delta << std::endl;
        exit(1);
    }
    if (delta > *max_delta)
    {
        *max_delta = delta;
    }
}

int main()
{
    float max_delta = 0.0f;
    float x;
    std::cout << "Running log2(x) over the range of [0.00001; 2) with step = 0.00001" << std::endl;
    for (x = 0.00001f; x < 2.0f; x += 0.00001)
    {
        verify_log2x(x, &max_delta);
    }
    std::cout << "Running log2(x) over the range of [2; 10000) with step = 0.001\n" << std::endl;
    for (x = 2.0f; x < 10000.0f; x += 0.001)
    {
        verify_log2x(x, &max_delta);
    }
    std::cout << "PASSED: max delta = " << max_delta << std::endl;

    return 0;
}
