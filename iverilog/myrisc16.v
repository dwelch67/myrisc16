
module myrisc16
(
    input   clk,
    input   rstn,
    output  halt
);

reg [15:0]  pc;
reg [15:0]  gpr [7:0];
reg         halt_flag;

wire [15:0] inst;
wire [2:0]  ra;
wire [2:0]  rb;
wire [2:0]  rc;
wire [15:0] imm;
wire [15:0] simm;

wire beq;
wire [15:0] bdest;
wire [2:0]  op;
wire [15:0] lsoff;
wire [15:0] pc_next;

reg [15:0]  mem [65535:0];

wire [15:0] reg0;
wire [15:0] reg1;
wire [15:0] reg2;
wire [15:0] reg3;
wire [15:0] reg4;
wire [15:0] reg5;
wire [15:0] reg6;
wire [15:0] reg7;

always @ ( posedge clk or rstn )
begin
    if(rstn == 1'b0)
    begin
        halt_flag = 1'b0;
        pc = 16'h0000;
        gpr[0] = 16'h0000;
        gpr[1] = 16'h0000;
        gpr[2] = 16'h0000;
        gpr[3] = 16'h0000;
        gpr[4] = 16'h0000;
        gpr[5] = 16'h0000;
        gpr[6] = 16'h0000;
        gpr[7] = 16'h0000;
    end
    else
    begin
        case(op)
            //000aaabbb0000ccc add ra,rb,rc
            3'b000:
                begin
                    if(ra!=3'b000)
                    begin
                        gpr[ra] = gpr[rb] + gpr[rc];
                    end
                    pc = pc_next;
                end
            //001aaabbbsssssss addi ra,rb,simm
            3'b001:
                begin
                    if(ra!=3'b000)
                    begin
                        gpr[ra] = gpr[rb] + simm;
                    end
                    pc = pc_next;
                end
            //010aaabbb0000ccc nand ra,rb,rc  ra = ~(rb|rc)
            3'b010:
                begin
                    if(ra!=3'b000)
                    begin
                        gpr[ra] = ~(gpr[rb]|gpr[rc]);
                    end
                    pc = pc_next;
                end
            //011aaaiiiiiiiiii lui ra,imm   ra=imm<<6
            3'b011:
                begin
                    if(ra!=3'b000)
                    begin
                        gpr[ra] = {inst[9:0],6'h00};
                    end
                    pc = pc_next;
                end
            //100aaabbbsssssss sw ra,[rb+simm]
            3'b100:
                begin
                    mem[lsoff] = gpr[ra];
                    pc = pc_next;
                end
            //101aaabbbsssssss lw ra,[rb+simm]
            3'b101:
                begin
                    if(ra!=3'b000)
                    begin
                        gpr[ra] = mem[lsoff];
                    end
                    pc = pc_next;
                end
            //110aaabbbsssssss beq ra,rb,simm
            3'b110:
                begin
                   pc = pc_next;
                end
            //111aaabbb0000000 jalr ra,rb
            3'b111:
                begin
                    if(inst==16'hFFFF)
                    begin
                        //dont want the pc to move, dont change the pc
                        halt_flag = 1'b1;
                    end
                    else
                    begin
                        if(ra!=3'b000)
                        begin
                            gpr[ra]= pc + 16'h0001;
                        end
                        pc = gpr[rb];
                    end
                end
        endcase
    end
end

assign op = inst[15:13];
assign beq = (op == 3'b110) ? (gpr[ra] == gpr[rb]) ? 1'b1 : 1'b0 : 0;
assign lsoff = gpr[rb] + simm;
assign inst = mem[pc];
assign bdest = pc + simm + 16'h0001;
assign pc_next = (beq == 1) ? bdest : pc + 16'h0001;

assign ra = inst[12:10];
assign rb = inst[9:7];
assign rc = inst[2:0];

assign imm = {6'b0,inst[9:0]};

assign simm = {inst[6],inst[6],inst[6],inst[6],inst[6],inst[6],inst[6],inst[6],inst[6],inst[6:0]};

assign halt = halt_flag;

assign reg0 = gpr[0];
assign reg1 = gpr[1];
assign reg2 = gpr[2];
assign reg3 = gpr[3];
assign reg4 = gpr[4];
assign reg5 = gpr[5];
assign reg6 = gpr[6];
assign reg7 = gpr[7];


endmodule
