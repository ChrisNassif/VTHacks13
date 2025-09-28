module uart_tx #(
    parameter CLOCK_FREQ = 50_000_000,
    parameter BAUD_RATE = 115200
)(
    input wire clk,
    input wire rst_n,
    input wire [7:0] data,
    input wire valid,
    output reg ready,
    output reg tx
);

    localparam BAUD_TICKS = CLOCK_FREQ / BAUD_RATE;
    
    typedef enum logic [2:0] {
        IDLE,
        START_BIT,
        DATA_BITS,
        STOP_BIT
    } state_t;
    
    state_t state;
    reg [7:0] shift_reg;
    reg [2:0] bit_count;
    reg [$clog2(BAUD_TICKS)-1:0] baud_counter;
    
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state <= IDLE;
            tx <= 1'b1;
            ready <= 1'b1;
            shift_reg <= 8'h00;
            bit_count <= 0;
            baud_counter <= 0;
        end else begin
            case (state)
                IDLE: begin
                    tx <= 1'b1;
                    ready <= 1'b1;
                    if (valid && ready) begin
                        shift_reg <= data;
                        ready <= 1'b0;
                        state <= START_BIT;
                        baud_counter <= 0;
                    end
                end
                
                START_BIT: begin
                    tx <= 1'b0;
                    if (baud_counter == BAUD_TICKS - 1) begin
                        baud_counter <= 0;
                        state <= DATA_BITS;
                        bit_count <= 0;
                    end else begin
                        baud_counter <= baud_counter + 1;
                    end
                end
                
                DATA_BITS: begin
                    tx <= shift_reg[bit_count];
                    if (baud_counter == BAUD_TICKS - 1) begin
                        baud_counter <= 0;
                        if (bit_count == 7) begin
                            state <= STOP_BIT;
                        end else begin
                            bit_count <= bit_count + 1;
                        end
                    end else begin
                        baud_counter <= baud_counter + 1;
                    end
                end
                
                STOP_BIT: begin
                    tx <= 1'b1;
                    if (baud_counter == BAUD_TICKS - 1) begin
                        baud_counter <= 0;
                        state <= IDLE;
                    end else begin
                        baud_counter <= baud_counter + 1;
                    end
                end
            endcase
        end
    end

endmodule