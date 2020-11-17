#pragma once

#include "xlog2_hw.h"

float log2_hw(XLog2_hw* instance, float x);

#ifdef TESTBENCH_SW
float log2_sw(float x);
#endif
