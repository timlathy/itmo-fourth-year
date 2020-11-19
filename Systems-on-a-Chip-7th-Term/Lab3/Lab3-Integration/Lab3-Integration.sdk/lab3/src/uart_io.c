#include "uart_io.h"

#include "xuartlite_i.h"

void uart_print_int(int x)
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
    char digits[10]; // floor(log10(2^31-1)) + 1 = 10
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

void uart_print_float(float x)
{
    if (x < 0)
    {
        XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, '-');
        x = -x;
    }

    x += 0.00005; // round to 4 decimal places

    int dec = (int)x;
    uart_print_int(dec);
    x -= dec;

    XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, '.');

    for (int i = 0; i < 4; ++i)
    {
        x *= 10;
        dec = (int)x;
        XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, 0x30 + dec);
        x -= dec;
    }
}

void uart_print_string(char* str)
{
    char c;
    while ((c = *(str++)) != 0)
        XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, c);
}

float uart_read_pos_float(char* prompt)
{
    while (1)
    {
        uart_print_string(prompt);

        float in = 0;
        int frac_part = 0;
        float frac_div = 1;

        while (1)
        {
            char in_c = XUartLite_RecvByte(XPAR_UARTLITE_0_BASEADDR);
            if (in_c == '\r')
                break;
            if (in_c != '.' && (in_c < '0' || in_c > '9'))
                continue;
            XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, in_c);

            if (in_c == '.')
            {
                frac_part = 1;
            }
            else if (frac_part)
            {
                frac_div *= 10;
                in += (float)(in_c - 0x30) / frac_div;
            }
            else {
                in *= 10;
                in += (in_c - 0x30);
            }
        }

        XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, '\r');
        XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, '\n');

        if (in > 0.0f)
            return in;

        uart_print_string("Please enter a valid number greater than zero\r\n");
    }

}
