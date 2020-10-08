`timescale 1ns / 1ps

module soc_testbench;
  reg clk;
  reg rst;
  wire [15:0] gpio_o;
  wire gpio_i;
  reg uart_rx;
  wire uart_tx;
  soc_design_wrapper dut(
    .clk_100MHz(clk),
    .reset_rtl_0(rst),
    .gpio_rtl_0_tri_o(gpio_o),
    .gpio_rtl_1_tri_i(gpio_i),
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
        #8600
        b[t] = uart_tx;
      end
    end
  endtask
  
  task write_uart_byte;
    integer t;
    input [7:0] b;
    begin
      #8600
      uart_rx = 0;
      for (t = 0; t < 8; t = t + 1) begin
        #8600
        uart_rx = b[t];
      end
      #8600
      uart_rx = 1;
    end
  endtask

  always begin
    #5;
    clk = ~clk;
  end
  
  integer p;
  integer uart_p;
  
  realtime compute_start = 0.0;
  realtime compute_end = 0.0;
  
  initial begin
    clk = 0;
    uart_rx = 1;

    // 10E-9 = ns, 2 decimal points
    $timeformat(-9, 2, " ns", 1);
  
    for (p = 3; p <= 8; p = p + 1) begin
      rst = 1;
      #100;
      rst = 0;
      #10000;
    
      write_uart_byte(p);
      
      @(posedge gpio_o[0]);
      compute_start = $realtime;
    
      @(posedge gpio_o[1]);
      compute_end = $realtime;
      
      read_uart_byte(uart_p);
      if (uart_p != p) begin
        $display("Error: sent p = %d, got p = %d over UART", p, uart_p);
        $finish;
      end
      
      $display("Computation with p = %d took %t", p, compute_end - compute_start);
    end
    
    $stop;
  end
endmodule
