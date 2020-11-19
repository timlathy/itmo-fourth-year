//#define TESTBENCH_SW
//#define TESTBENCH_HW
#define DEVICE

#include "platform.h"
#include "xil_io.h"

#include "log2.h"
#include "uart_io.h"

void device_loop()
{
    XLog2_hw log2;
    XLog2_hw_Initialize(&log2, XPAR_LOG2_HW_0_DEVICE_ID);

    while (1)
    {
        float x = uart_read_pos_float("x = ");
        float log2x = log2_hw(&log2, x);

        uart_print_string("log2(");
        uart_print_float(x);
        uart_print_string(") = ");
        uart_print_float(log2x);
        uart_print_string("\r\n");
    }
}

int main()
{
    init_platform();

#ifdef TESTBENCH_SW
    Xil_Out32(XPAR_GPIO_0_BASEADDR, ~0);

    float log2x = log2_sw(0.313f);
    Xil_Out32(XPAR_GPIO_0_BASEADDR, 0);
    uart_print_float(log2x);

    Xil_Out32(XPAR_GPIO_0_BASEADDR, ~0);
#endif
#ifdef TESTBENCH_HW
    XLog2_hw log2;
    XLog2_hw_Initialize(&log2, XPAR_LOG2_HW_0_DEVICE_ID);

    Xil_Out32(XPAR_GPIO_0_BASEADDR, ~0);

    float log2x = log2_hw(&log2, 0.313f);

    Xil_Out32(XPAR_GPIO_0_BASEADDR, 0);
    uart_print_float(log2x);

    Xil_Out32(XPAR_GPIO_0_BASEADDR, ~0);
#endif
#ifdef DEVICE
    device_loop();
#endif

    cleanup_platform();
    return 0;
}
