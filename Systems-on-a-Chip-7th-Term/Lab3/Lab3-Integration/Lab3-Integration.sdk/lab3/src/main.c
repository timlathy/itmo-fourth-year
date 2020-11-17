//#define TESTBENCH_SW

#include "platform.h"
#include "xil_io.h"

#include "log2.h"
#include "uart_io.h"

int main()
{
    init_platform();

#ifdef TESTBENCH_SW
    Xil_Out32(XPAR_GPIO_0_BASEADDR, ~0);

    float log2x = log2_sw(0.313f);
    Xil_Out32(XPAR_GPIO_0_BASEADDR, 0);
    uart_print_float(log2x);

    Xil_Out32(XPAR_GPIO_0_BASEADDR, ~0);
#else
    XLog2_hw log2;
    XLog2_hw_Initialize(&log2, XPAR_LOG2_HW_0_DEVICE_ID);

    Xil_Out32(XPAR_GPIO_0_BASEADDR, ~0);

    float log2x = log2_hw(&log2, 0.313f);

    Xil_Out32(XPAR_GPIO_0_BASEADDR, 0);
    uart_print_float(log2x);

    Xil_Out32(XPAR_GPIO_0_BASEADDR, ~0);
#endif

    cleanup_platform();
    return 0;
}
