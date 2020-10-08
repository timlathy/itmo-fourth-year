#include "platform.h"
#include "xuartlite_i.h"
#include <xil_io.h>
#include "xintc.h"
#include "xtmrctr.h"
#include "xil_exception.h"
#include "xstatus.h"
#include "sleep.h"
#include "xgpio.h"

// Disable interrupts and UART IO, only compute the result for a predetermined input vector
// and enable all LEDs to signify completion (used for Vivado simulation)
//#define COMPUTE_ONLY_BENCH

#define P_INPUT_MODE 1
#define X_INPUT_MODE 0

volatile int input_mode = X_INPUT_MODE;
volatile u16 in_p = 3;
volatile u16 in_data[10] = {0,0,0,0,0,0,0,0,0,0};
volatile u16 out_data[8] = {0,0,0,0,0,0,0,0};
volatile int timer_out_i = -1;
volatile int y_count = 8;

void timer_interrupt_handler(XTmrCtr* instance, u8 timer_ctr_num)
{
    if (XTmrCtr_IsExpired(instance, timer_ctr_num) && timer_out_i >= 0)
    {
        if (timer_out_i < y_count)
        {
            Xil_Out16(XPAR_GPIO_0_BASEADDR, out_data[timer_out_i++]);
        }
        else
        {
            Xil_Out16(XPAR_GPIO_0_BASEADDR, 0);
            // -1 = "don't print anything", reset in the output_result() function
            timer_out_i = -1;
        }
    }
}

void gpio_switch_interrupt_handler(XGpio* instance)
{
    XGpio_InterruptClear(instance, XGPIO_IR_CH2_MASK);
    int mode = Xil_In16(XPAR_GPIO_0_BASEADDR + 8);
    input_mode = mode & 1;
}

void fir_filter(u16 p)
{
    y_count = 10 - p + 1;
    for (int i = 0; i < y_count; ++i)
    {
        u32 acc = 0; // Use a 32-bit accumulator to prevent overflow
        for (int j = 0; j < p; ++j)
            acc += in_data[i + j];

        out_data[i] = acc / p;
    }
}

void uart_send_bytes(int count, ...)
{
    va_list ap;
    va_start(ap, count);

    for (int i = 0; i < count; ++i)
        XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, va_arg(ap, int));

    va_end(ap);
}

int read_ascii_num_uart(int initial_input_mode, volatile u16* out)
{
    u16 num = 0;

    u8 in_char = 0;
    int characters_entered = 0;
    while (1)
    {
        while (XUartLite_IsReceiveEmpty(XPAR_UARTLITE_0_BASEADDR))
            if (input_mode != initial_input_mode)
                return 0;

        in_char = XUartLite_RecvByte(XPAR_UARTLITE_0_BASEADDR);
        if (in_char >= 0x30 && in_char <= 0x39)
        {
            num = num * 10 + (in_char - 0x30);
            XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, in_char);
            characters_entered++;
        }
        else if (in_char == '\r')
        {
        	break;
        }
        else if (in_char == '\b' && characters_entered > 0)
        {
        	num /= 10;
            uart_send_bytes(3, '\b', ' ', '\b');
            characters_entered--;
        }
    }
    uart_send_bytes(2, '\r', '\n');

    *out = num;
    return 1;
}

void write_ascii_num_uart(u16 num)
{
    if (num == 0)
    {
        XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, '0');
        return;
    }

    char digits[5]; // floor(log10(2^16-1)) + 1 = 5
    int d = -1;

    while (num > 0)
    {
        u16 rem = num % 10;
        num /= 10;

        digits[++d] = rem + 0x30;
    }

    while (d >= 0)
        XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, digits[d--]);
}

void output_result()
{
    uart_send_bytes(3, 'x', '=', '{');
    for (int i = 0; i < 10; ++i)
    {
        write_ascii_num_uart(in_data[i]);
        if (i < 9)
            XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, ',');
        else
            XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, '}');
    }
    uart_send_bytes(3, ',', 'p', '=');
    write_ascii_num_uart(in_p);
    uart_send_bytes(2, '\r', '\n');

    fir_filter(in_p);

    // Flash the leds to indicate that the computation's finished
    for (int i = 0; i < 3; ++i)
    {
        usleep(100000);
        Xil_Out16(XPAR_GPIO_0_BASEADDR, 0xffff);
        usleep(100000);
        Xil_Out16(XPAR_GPIO_0_BASEADDR, 0);
    }

    // The timer interrupt handler will display output values starting from index 0 the next time it fires
    timer_out_i = 0;

    for (int i = 0; i < y_count; ++i)
    {
        uart_send_bytes(5, 'y', i + 0x30, ' ', '=', ' ');
        write_ascii_num_uart(out_data[i]);
        uart_send_bytes(2, '\r', '\n');
    }
}

int input_p()
{
    uart_send_bytes(2, '\r', '\n');

    do
    {
        uart_send_bytes(4, 'p', ' ', '=', ' ');
        if (!read_ascii_num_uart(P_INPUT_MODE, &in_p))
            return 0;
    }
    while (in_p < 3 || in_p > 8);

    return 1;
}

int input_x()
{
    uart_send_bytes(2, '\r', '\n');

    for (int i = 0; i < 10; ++i)
    {
        uart_send_bytes(5, 'x', i + 0x30, ' ', '=', ' ');
        if (!read_ascii_num_uart(X_INPUT_MODE, in_data + i))
            return 0;
    }

    return 1;
}

int main()
{
    init_platform();

#ifdef COMPUTE_ONLY_BENCH
    u16 test_data[10] = {13, 22, 3, 4, 6, 2, 3, 5, 3, 1};
    for (int i = 0; i < 10; ++i)
    	in_data[i] = test_data[i];

    u16 p = XUartLite_RecvByte(XPAR_UARTLITE_0_BASEADDR);
    Xil_Out16(XPAR_GPIO_0_BASEADDR, 1);
    fir_filter(p);
    Xil_Out16(XPAR_GPIO_0_BASEADDR, 0xffff);
    XUartLite_SendByte(XPAR_UARTLITE_0_BASEADDR, p);
#else
    XTmrCtr timer_counter;
    XGpio gpio;
    XIntc interrupt_controller;
    XTmrCtr_Initialize(&timer_counter, XPAR_TMRCTR_0_DEVICE_ID);
    XTmrCtr_SetHandler(&timer_counter,
            (XTmrCtr_Handler)timer_interrupt_handler,
            &timer_counter);

    XGpio_Initialize(&gpio, XPAR_AXI_GPIO_0_DEVICE_ID);
    XGpio_InterruptGlobalEnable(&gpio);
    // Enable interrupts for the second GPIO channel only (ch1 is mapped to leds, ch2 is mapped to a switch)
    XGpio_InterruptEnable(&gpio, XGPIO_IR_CH2_MASK);

    XIntc_Initialize(&interrupt_controller, XPAR_INTC_0_DEVICE_ID);
    XIntc_Connect(&interrupt_controller, XPAR_INTC_0_TMRCTR_0_VEC_ID,
            (XInterruptHandler)XTmrCtr_InterruptHandler,
            (void *)&timer_counter);
    XIntc_Connect(&interrupt_controller, XPAR_INTC_0_GPIO_0_VEC_ID,
            (XInterruptHandler)gpio_switch_interrupt_handler,
            (void *)&gpio);
    XIntc_Start(&interrupt_controller, XIN_REAL_MODE);
    XIntc_Enable(&interrupt_controller, XPAR_INTC_0_TMRCTR_0_VEC_ID);
    XIntc_Enable(&interrupt_controller, XPAR_INTC_0_GPIO_0_VEC_ID);

    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
            (Xil_ExceptionHandler)XIntc_InterruptHandler,
             &interrupt_controller);
    Xil_ExceptionEnable();

    XTmrCtr_SetOptions(&timer_counter, 0,
            XTC_INT_MODE_OPTION | // enable timer interrupts
            XTC_DOWN_COUNT_OPTION | // count down from the start value (set to an equivalent of 1 second below)
            XTC_AUTO_RELOAD_OPTION); // automatically reload the reset value when the counter reaches 0
    XTmrCtr_SetResetValue(&timer_counter, 0, 100000000); // 1 second at 100MHz
    XTmrCtr_Reset(&timer_counter, 0);
    XTmrCtr_Start(&timer_counter, 0);

    // Set current input mode from the initial switch position
    gpio_switch_interrupt_handler(&gpio);

    while (1)
    {
        int input_success = (input_mode == X_INPUT_MODE) ? input_x() : input_p();
        if (input_success)
            output_result();
    }
#endif

    cleanup_platform();
    return 0;
}


