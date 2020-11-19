#include <cmath>
#include <iostream>
#include "log2.h"

//#define COSIM

struct verification_result
{
    float max_re, max_re_x, max_re_x_delta;
};

void verify_log2x(float x, verification_result* res)
{
    float expected = log2f(x);
    float actual = log2_hw(x);
    float delta = fabs(expected - actual);
    float re = delta / actual;
    if (re > 0.001f)
    {
        std::cerr << "ERROR: log2(" << x << ") = " << actual
                  << ", RE = " << re << " (delta = " << delta << ")" << std::endl;
        exit(1);
    }
    if (re > res->max_re)
    {
        res->max_re = re;
        res->max_re_x = x;
        res->max_re_x_delta = delta;
    }
}

int main()
{
    verification_result res{0.0f, 0.0f, 0.0f};
    float x;
#ifdef COSIM
    verify_log2x(0.313, &res);
#else
    std::cout << "Running log2(x) over the range of [0.00001; 2) with step = 0.00001" << std::endl;
    for (x = 0.00001f; x < 2.0f; x += 0.00001)
    {
        verify_log2x(x, &res);
    }
    std::cout << "Running log2(x) over the range of [2; 10000) with step = 0.001\n" << std::endl;
    for (x = 2.0f; x < 10000.0f; x += 0.001)
    {
        verify_log2x(x, &res);
    }
#endif
    std::cout << "PASSED: max RE = " << res.max_re << " at x = " << res.max_re_x
              << " (delta = " << res.max_re_x_delta << ")" << std::endl;
    return 0;
}
