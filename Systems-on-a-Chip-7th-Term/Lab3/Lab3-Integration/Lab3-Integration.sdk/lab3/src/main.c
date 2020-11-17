#include "platform.h"
#include "xil_io.h"
#include "xuartlite_i.h"

#include "log2.h"

void print_int(int x)
{
    if (x == 0)
    {
        XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, '0');
        return;
    }
    if (x < 0)
    {
        XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, '-');
        x = -x;
    }
    char digits[5]; // floor(log10(2^16-1)) + 1 = 5
    int d = -1;

    while (x > 0)
    {
        int rem = x % 10;
        x /= 10;

        digits[++d] = rem + 0x30;
    }

    while (d >= 0)
        XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, digits[d--]);
}

void print_float(float x)
{
    if (x < 0)
    {
        XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, '-');
        x = -x;
    }

    x += 0.00005; // round to 4 decimal places

    int dec = (int)x;
    print_int(dec);
    x -= dec;

    XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, '.');

    for (int i = 0; i < 4; ++i)
    {
        x *= 10;
        dec = (int)x;
        XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, 0x30 + dec);
        x -= dec;
    }

    XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, '\r');
    XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, '\n');
}

int main()
{
    init_platform();

    Xil_Out32(XPAR_GPIO_0_BASEADDR, ~0);

    float log2x = log2_sw(0.313f);
    Xil_Out32(XPAR_GPIO_0_BASEADDR, 0);
    print_float(log2x);

    Xil_Out32(XPAR_GPIO_0_BASEADDR, ~0);

    cleanup_platform();
    return 0;
}
