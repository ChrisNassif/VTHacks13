`timescale 1ns / 1ps
`default_nettype wire

module vthacks_test_bench();
    logic clock;
    logic [31:0] button_array;
    logic uart_tx;

    fpga_vthacks fpga_vthacks(.clock(clock), .button_array(button_array), .uart_tx(uart_tx));


    initial begin
        $dumpfile("build/vthacks_test_bench.vcd"); //file to store value change dump (vcd)
        $dumpvars(0, vthacks_test_bench); //store everything at the current level and below


        clock = 0;
        button_array = 32'b00000000000000000000000000000000;

        # 160

        button_array = 32'b10000000000000000000000000000000;

        # 160

        button_array = 32'b11000000000000000000000000000000;

        # 160 

        button_array = 32'b00000000000000000000000000000111;

        # 160

        button_array = 32'b00000000000000000000000000000000;

        # 160

        button_array = 32'b10000000000000000000000000000000;

        # 160 

        button_array = 32'b11000000000000000000000000000000;

        # 160 

        button_array = 32'b00000000000000000000000000000111;

        $display("Finishing Sim");
        $finish;
    end


    always begin
        #1 clock = !clock;
    end
endmodule 