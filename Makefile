#!Makefile

TEST=test

all:main.o riscv.o memory.o fetch.o bus.o decode.o execute.o
	gcc -g $^ -o rv_emu

main.o:main.c
	gcc -g -c $< -o $@

riscv.o:riscv.c
	gcc -g -c $< -o $@

memory.o:memory.c
	gcc -g -c $< -o $@

bus.o:bus.c
	gcc -g -c $< -o $@

fetch.o:fetch.c
	gcc -g -c $< -o $@

decode.o:decode.c
	gcc -g -c $< -o $@

execute.o:execute.c
	gcc -g -c $< -o $@

check:
	riscv32-unknown-linux-gnu-gcc -Wl,-Ttext=0x0 -nostdlib -march=rv32g -O0 $(TEST)/test.s -o $(TEST)/test.obj
	riscv32-unknown-linux-gnu-objcopy -O binary $(TEST)/test.obj $(TEST)/test.bin
	riscv32-unknown-linux-gnu-gcc -Wl,-Ttext=0x0 -nostdlib -march=rv32g -O0 $(TEST)/fib.c -o $(TEST)/fib.obj
	riscv32-unknown-linux-gnu-objcopy -O binary $(TEST)/fib.obj $(TEST)/fib.bin
	riscv32-unknown-linux-gnu-gcc -Wl,-Ttext=0x0 -nostdlib -march=rv32g -O0 $(TEST)/add.c -o $(TEST)/add.obj
	riscv32-unknown-linux-gnu-objcopy -O binary $(TEST)/add.obj $(TEST)/add.bin

clean:
	rm *.o rv_emu $(TEST)/*.obj $(TEST)/*.bin
