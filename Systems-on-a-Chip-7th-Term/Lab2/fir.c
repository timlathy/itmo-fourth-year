#include "fir.h"

//void fir_filter(short x[FIR_MAX_IN], char p, short y[FIR_MAX_OUT])
//{
//    if (p < 3 || p > 10)
//        return;
//
//    char y_count = 10 - p + 1;
//
//    LoopOverOutputs:
//    for (char i = 0; i < y_count; ++i)
//    {
//        int acc = 0; // Use a 32-bit accumulator to prevent overflow
//
//        SumAccLoop:
//        for (char j = 0; j < p; ++j)
//        {
//            acc += x[i + j];
//        }
//
//        y[i] = acc / p;
//    }
//}


void fir_filter(short x[FIR_MAX_IN], char p, short y[FIR_MAX_OUT])
{
    #pragma HLS array_partition variable=x complete

    if (p < 3 || p > 10)
        return;

    char y_count = 10 - p + 1;

    LoopOverOutputs:
    for (char i = 0; i < y_count; ++i)
    {
        #pragma HLS pipeline

        int acc = 0; // Use a 32-bit accumulator to prevent overflow

        switch (p)
        {
            case 10: acc += x[i + 9]; // @suppress("No break at end of case")
            case 9: acc += x[i + 8]; // @suppress("No break at end of case")
            case 8: acc += x[i + 7]; // @suppress("No break at end of case")
            case 7: acc += x[i + 6]; // @suppress("No break at end of case")
            case 6: acc += x[i + 5]; // @suppress("No break at end of case")
            case 5: acc += x[i + 4]; // @suppress("No break at end of case")
            case 4: acc += x[i + 3]; // @suppress("No break at end of case")
            case 3:
                acc += x[i + 2];
                acc += x[i + 1];
                acc += x[i + 0];
        }

        int tmp;
        switch (p)
        {
            case 3: y[i] = ((((long long)acc * 0x55555556) >> 32) - (acc >> 31)); break;
            case 4: y[i] = acc >> 2; break;
            case 5: y[i] = ((((long long)acc * 0x66666667) >> 33) - (acc >> 31)); break;
            case 6: y[i] = ((((long long)acc * 0x2AAAAAAB) >> 32) - (acc >> 31)); break;
            case 7:
                tmp = (((long long)acc * (int)0x92492493) >> 32) + acc;
                y[i] = (tmp >> 2) + (tmp >> 31);
                break;
            case 8: y[i] = acc >> 3; break;
            case 9: y[i] = ((((long long)acc * 0x38E38E39) >> 33) - (acc >> 31)); break;
            case 10: y[i] = ((((long long)acc * 0x66666667) >> 34) - (acc >> 31)); break;
        }
    }
}
