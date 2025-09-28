module VT_HACKS_FPGA (
    input wire CLOCK_50,
    input wire [3:0] KEY,
    input wire [9:0] SW,
    output wire [9:0] LEDR,
    inout wire GPIO_0_D0   // Connect to UART TX pin
);

    wire rst_n = KEY[0];
    
    // Example 32-bit array (you can modify this)
    wire [31:0] test_array;
    assign test_array = {SW, SW, SW, 2'b00};
    
    // Send trigger (button press)
    wire send_trigger = ~KEY[1];  // Active low button
    wire busy;
    
    assign LEDR[0] = busy;
    
    array_uart_transmitter #(
        .CLOCK_FREQ(50_000_000),
        .BAUD_RATE(115200)
    ) transmitter (
        .clk(CLOCK_50),
        .rst_n(rst_n),
        .data_array(test_array),
        .send_trigger(send_trigger),
        .uart_tx(GPIO_0_D0),
        .busy(busy)
    );

endmodule