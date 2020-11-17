`timescale 1ns / 1ps

module testbench;
  reg clk;
  reg rst;
  wire [31:0] gpio;
  wire uart_rx;
  wire uart_tx;
  
  design_1_wrapper dut(
    .clk_100MHz(clk),
    .reset_rtl_0(rst),
    .gpio_rtl_0_tri_o(gpio),
    .uart_rtl_0_rxd(uart_rx),
    .uart_rtl_0_txd(uart_tx));

  uart_utils uart(
    .uart_rx(uart_rx),
    .uart_tx(uart_tx));
  
  always begin
    #5;
    clk = ~clk;
  end

  realtime compute_start = 0.0;
  realtime compute_end = 0.0;

  reg [7:0] uart_char;

  initial begin
    // 10E-9 = ns, 2 decimal points
    $timeformat(-9, 2, " ns", 1);
    
    clk = 0;
    
    @(posedge gpio[0]);  
    compute_start = $realtime;
    
    @(negedge gpio[31]); // set to 0 when the computation is done
    compute_end = $realtime;
    
    $write("Computation took %t, log2(0.313) = ", compute_end - compute_start);
    
    uart_char = 0;
    while (uart_char != 'h0d) begin
      uart.read_byte(uart_char);
      $write("%s", uart_char);
    end
    $write("\n");
    $stop;
  end
endmodule
