

all : test.vcd

test.vcd : tb.v myrisc16.v memory.txt
	rm -f test.vcd
	iverilog -o test.vvp tb.v myrisc16.v
	./test.vvp 

clean :
	rm -f test.vcd
	rm -f test.vvp


