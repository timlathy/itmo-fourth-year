UART over MicroUSB:
RX -> C4, TX -> D4

# Creating a project

1. Open Vivado
2. Create an _RTL_ project, no sources, no constraints, `100tcsg324-1`
3. Create a new block design
4. RMB -> `Add IP` -> `MicroBlaze`
5. `Run Block Automation` -> set `Local Memory` to `32KB`, `Debug Mode` to `None`
6. Double-click on the `Clocking Wizard` component -> set `Input Clock Source` to `Single ended clock capable pin`
7. RMB -> `Add IP` -> `AXI Uartlite`, repeat for `AXI GPIO`
8. `Run Connection Automation` -> `All automation` -> `OK`
9. `Validate Design (F6)`
10. Select the design in `Sources` -> `Design Sources`, RMB -> `Create HDL Wrapper` -> `Let Vivado manage ...`
12. `Run Synthesis`, go to `Design Runs` -> `Out-of-Context Module Runs` to observe the progress
13. `File` -> `Export` -> `Export Hardware`
14. `File` -> `Launch SDK` -> `OK`

1. `File` -> `New` -> `Application Project`, set `OS Platform` to `standalone`, `Board Support Package` to `Create New`, select `Hello World` template
2. Edit `helloworld.c`: include `xil_io.h`, use `Xil_Out32` to output a number to GPIO's address (see `Address Editor` in Vivado), but before that set GPIO mode to output (write `0x0` to `....0004`).
3. Go to Vivado, RMB on MicroBlaze -> `Associate ELF Files` -> `...` -> `Add Files` -> `project_sdk\Proj\Debug\Proj.elf`
4. Double-click on `Processor System Reset`, check `Ext Reset Logic Level` (if set to 0, 0 = rst)
5. `Add sources` -> `Simulation sources` ->  `Create Source File` -> `testbench`
6. Add the wrapper module to `testbench`

# AXI

https://www.xilinx.com/support/documentation/ip_documentation/axi_gpio/v2_0/pg144-axi-gpio.pdf

Page 10 Register Space
