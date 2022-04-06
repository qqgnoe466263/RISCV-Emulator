CC = riscv32-unknown-linux-gnu-gcc
CFLAGS = -nostdlib -fno-builtin -march=rv32g

QEMU = qemu-system-riscv32
QFLAGS = -nographic -smp 4 -machine virt -bios none

OBJDUMP = riscv64-unknown-elf-objdump

all: os.elf

os.elf: start.s sys.s lib.c os.c
	$(CC) $(CFLAGS) -T os.ld -o os.elf $^
	riscv32-unknown-linux-gnu-objcopy -O binary os.elf os.bin

qemu: $(TARGET)
	@qemu-system-riscv32 -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	$(QEMU) $(QFLAGS) -kernel os.elf

clean:
	rm -f *.elf
