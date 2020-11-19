RTL diagram:
* Draw from scratch (less detailed than synthesis report)
* Draw switches (MUXes) for registers shared between several blocks
* Can skip clock and reset signals

Iterative algorithms?

https://www.xilinx.com/support/documentation/white_papers/wp491-floating-to-fixed-point.pdf
DSP48E1 (25x18)

Block interfaces: AXI or AXI stream
Can use interrupts if you want but it's not required
  while (Status & AP_DONE == 0)
  readstatus
Pipeline: AP_DONE, AP_Y_VLD (can't use AP_READY)

Testbench:
  Recommended to have 100s of values

Getting more points:
  User interface over UART IO (similar to the first lab)
  Launch on the devboard

Vivado HLS:
1. Export RTL -> change Display Name
solution/impl/ip:
* .zip - can be exported (IP-XACT)
* component.xml - interfaces, etc.
* hdl/ - block code

Vivado Design Suite:
1. Create a project with MicroBlaze and other required components
2. Tools -> Settings -> Project -> IP -> Repository -> enter path to ip/
3. Add IP
4. Run Connection Automation
5. Don't forget to Export hardware
