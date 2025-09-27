make all:
	iverilog -g2012 -o build/vthacks_test_bench.out vthacks_test_bench.sv vthacks.sv
	vvp build/vthacks_test_bench.out
	gtkwave build/vthacks_test_bench.vcd