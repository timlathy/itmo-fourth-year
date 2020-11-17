`timescale 1ns / 1ps

module testbench;
  reg clk;
  reg rst;
  wire [31:0] gpio;
  reg uart_rx;
  wire uart_tx;
  
  design_1_wrapper dut(
    .clk_100MHz(clk),
    .reset_rtl_0(rst),
    .gpio_rtl_0_tri_o(gpio),
    .uart_rtl_0_rxd(uart_rx),
    .uart_rtl_0_txd(uart_tx));
    
  task read_uart_byte;
    integer t;
    output [7:0] b;
    begin
      b = 0;
      @(negedge uart_tx);
      #1000
      for (t = 0; t < 8; t = t + 1) begin
        #4300
        b[t] = uart_tx;
      end
    end
  endtask
  
  task write_uart_byte;
    integer t;
    input [7:0] b;
    begin
      #4300
      uart_rx = 0;
      for (t = 0; t < 8; t = t + 1) begin
        #4300
        uart_rx = b[t];
      end
      #4300
      uart_rx = 1;
    end
  endtask

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
    uart_rx = 0;
    
    @(posedge gpio[0]);  
    compute_start = $realtime;
    
    @(negedge gpio[31]); // set to 0 when the computation is done
    compute_end = $realtime;
    
    $write("Computation took %t, log2(0.313) = ", compute_end - compute_start);
    
    uart_char = 0;
    while (uart_char != 'h0d) begin
      read_uart_byte(uart_char);
      $write("%s", uart_char);
    end
    $write("\n");
    $stop;
  end
endmodule
