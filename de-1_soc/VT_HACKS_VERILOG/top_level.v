module top_level (
    input CLOCK_50,
    input [3:0] KEY,
    output [9:0] LEDR,
    
    // HPS DDR3 Memory Interface (Required for Linux)
    output [14:0] HPS_DDR3_ADDR,
    output [2:0]  HPS_DDR3_BA,
    output        HPS_DDR3_CAS_N,
    output        HPS_DDR3_CKE,
    output        HPS_DDR3_CK_N,
    output        HPS_DDR3_CK_P,
    output        HPS_DDR3_CS_N,
    output [3:0]  HPS_DDR3_DM,
    inout  [31:0] HPS_DDR3_DQ,
    inout  [3:0]  HPS_DDR3_DQS_N,
    inout  [3:0]  HPS_DDR3_DQS_P,
    output        HPS_DDR3_ODT,
    output        HPS_DDR3_RAS_N,
    output        HPS_DDR3_RESET_N,
    input         HPS_DDR3_RZQ,
    output        HPS_DDR3_WE_N,
    
    // HPS USB Interface (Required for Linux)
    inout  [7:0]  HPS_USB_DATA,
    input         HPS_USB_CLK,
    output        HPS_USB_STP,
    input         HPS_USB_DIR,
    input         HPS_USB_NXT
);

    uart_small u0 (
        .clk_clk(CLOCK_50),
        .reset_reset_n(KEY[0]),
        .key_0_export_export(KEY[1]),
        
        // DDR3 Memory (Required)
        .memory_mem_a(HPS_DDR3_ADDR),
        .memory_mem_ba(HPS_DDR3_BA),
        .memory_mem_ck(HPS_DDR3_CK_P),
        .memory_mem_ck_n(HPS_DDR3_CK_N),
        .memory_mem_cke(HPS_DDR3_CKE),
        .memory_mem_cs_n(HPS_DDR3_CS_N),
        .memory_mem_ras_n(HPS_DDR3_RAS_N),
        .memory_mem_cas_n(HPS_DDR3_CAS_N),
        .memory_mem_we_n(HPS_DDR3_WE_N),
        .memory_mem_reset_n(HPS_DDR3_RESET_N),
        .memory_mem_dq(HPS_DDR3_DQ),
        .memory_mem_dqs(HPS_DDR3_DQS_P),
        .memory_mem_dqs_n(HPS_DDR3_DQS_N),
        .memory_mem_odt(HPS_DDR3_ODT),
        .memory_mem_dm(HPS_DDR3_DM),
        .memory_oct_rzqin(HPS_DDR3_RZQ),
        
        // HPS USB (Required)
        .hps_io_hps_io_usb0_inst_D0(HPS_USB_DATA[0]),
        .hps_io_hps_io_usb0_inst_D1(HPS_USB_DATA[1]),
        .hps_io_hps_io_usb0_inst_D2(HPS_USB_DATA[2]),
        .hps_io_hps_io_usb0_inst_D3(HPS_USB_DATA[3]),
        .hps_io_hps_io_usb0_inst_D4(HPS_USB_DATA[4]),
        .hps_io_hps_io_usb0_inst_D5(HPS_USB_DATA[5]),
        .hps_io_hps_io_usb0_inst_D6(HPS_USB_DATA[6]),
        .hps_io_hps_io_usb0_inst_D7(HPS_USB_DATA[7]),
        .hps_io_hps_io_usb0_inst_CLK(HPS_USB_CLK),
        .hps_io_hps_io_usb0_inst_STP(HPS_USB_STP),
        .hps_io_hps_io_usb0_inst_DIR(HPS_USB_DIR),
        .hps_io_hps_io_usb0_inst_NXT(HPS_USB_NXT)
    );

    assign LEDR = {9'b0, ~KEY[1]};

endmodule