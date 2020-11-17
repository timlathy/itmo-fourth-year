`timescale 1ns / 1ps

module uart_utils(
  input uart_rx,
  output uart_tx
  );
  
  reg uart_rx_r;
  assign uart_rx = uart_rx_r;
    
  task read_byte;
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
  
  task write_byte;
    integer t;
    input [7:0] b;
    begin
      #4300
      uart_rx_r = 0;
      for (t = 0; t < 8; t = t + 1) begin
        #4300
        uart_rx_r = b[t];
      end
      #4300
      uart_rx_r = 1;
    end
  endtask
endmodule
