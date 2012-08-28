
module tb;

    reg TOP_CLK;
    reg TOP_RST;

    reg [31:0] SHOW_CLOCK;

always @ ( posedge TOP_CLK or TOP_RST )
begin
    if(TOP_RST == 0)
    begin

        SHOW_CLOCK <= 0;

    end
    else
    begin

        SHOW_CLOCK <= SHOW_CLOCK + 1;

        if(halt == 1'b1)
        begin
            $finish;
        end

    end

end

myrisc16 mr16
(
    .clk(TOP_CLK),
    .rstn(TOP_RST),
    .halt(halt)
);

initial
begin
    TOP_RST <= 0;
    #1000;
    TOP_RST <= 1;
end

always
begin
    TOP_CLK <= 1;
    #100;
    TOP_CLK <= 0;
    #100;
end

initial begin
    $dumpfile("test.vcd");
    $dumpvars(20, tb);
end

initial begin
    #400000;
    $finish;
end

initial $readmemh("memory.txt",mr16.mem,16'h0000,16'hFFFF);

endmodule
