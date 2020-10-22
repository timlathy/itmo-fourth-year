#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fir.h"

void cmp_outputs(short* expected, short* actual, char p)
{
    char y_count = 11 - p;
    printf("Comparing %d values (p = %d):\n", y_count, p);

    int outputs_eq = 1;
    for (int i = 0; i < y_count; ++i)
    {
        if (expected[i] == actual[i])
        {
            printf("y_%d: %d = %d\n", i, expected[i], actual[i]);
        }
        else
        {
            printf("y_%d: expected %d, got %d\n", i, expected[i], actual[i]);
            outputs_eq = 0;
        }
    }
    puts("");
    if (!outputs_eq)
    {
        fprintf(stderr, "tests failed\n");
        exit(1);
    }
}

void test_all_window_sizes()
{
    short test_in[10] = {13, 22, 3, 4, 6, 2, 3, 5, 3, 1};
    short expected_out[8][8] = {
        {12, 9, 4, 4, 3, 3, 3, 3}, // p = 3
        {10, 8, 3, 3, 4, 3, 3, 0}, // p = 4
        {9, 7, 3, 4, 3, 2, 0, 0}, // p = 5
        {8, 6, 3, 3, 3, 0, 0, 0}, // p = 6
        {7, 6, 3, 3, 0, 0, 0, 0}, // p = 7
        {7, 6, 3, 0, 0, 0, 0, 0}, // p = 8
        {6, 5, 0, 0, 0, 0, 0, 0}, // p = 9
        {6, 0, 0, 0, 0, 0, 0, 0} // p = 10
    };

    short test_out[8];
    for (char p = 3; p <= 10; ++p)
    {
        memset(test_out, 0, 8 * sizeof(short));
        fir_filter(test_in, p, test_out);

        cmp_outputs(expected_out[p - 3], test_out, p);
    }
}

int main()
{
    test_all_window_sizes();
    puts("tests passed");
    return 0;
}
