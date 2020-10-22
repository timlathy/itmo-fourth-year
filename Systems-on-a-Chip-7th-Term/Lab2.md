FPGA Field Programmable Gate Array:
  * minimal entity is a logic gate (NAND, NOR, ...)
CPLD Complex Programmable Logic Device
  * minimal entity is an ALU, an FMAD unit, ...
  * non-volatile memory

IP-XACT: IP synthesized using HLS and packed in an archive (.zip)

Open Vivado HLS, then:
1. Create New Project
2. Add Files -> `design-files/Introduction/lab1/fir.c`
3. Specify top function (`fir`), which will be synthesized from the source file
4. Add testbench Files -> `fir_test.c`, `out.gold.dat` (expected outputs)
5. Part Selection -> `100tcsg324-1`

1. Run C Simulation ("green arrow in a window" icon in the tool bar) -> Tick Launch Debugger
2. Optionally set Input Arguments if using argc/argv
3. Ok

1. Switch to Synthesis in the top right corner
2. Run C Synthesis (green arrow icon in the tool bar)
3. Timing summary: estimated + uncertainty should be lower than target

1. Run C/RTL Co-simulation
2. Dump Trace -> port (after simulation we can see the timing diagrams for
signal level changes on ports)
3. Click Open Wave Viewer in the toolbar

1. Open Synthesis -> Directive
2. Select the loop in the source code
3. Double-click on it in the Directive tab, add UNROLL
4. Optionally specify unroll factor (could be larger than the synthesizer can achieve)
(This inserts a `#pragma` in the source code, you can add it manually)

1. Double-click on every argument in the Directive tab
2. Add INTERFACE -> mode = s_axilite
3. Also add `#pragma HLS INTERFACE` for `port=return`

1. Open solution -> `impl` -> `verilog` -> `_AXILiteS_s_axi.v`
2. Navigate to `Address Info`
3. These can be accessed by code running on MicroBlaze using `Xil_In`/`Xil_Out`
