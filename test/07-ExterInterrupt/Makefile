CC = riscv32-unknown-linux-gnu-gcc
CFLAGS = -nostdlib -fno-builtin -march=rv32g
GDB = riscv64-unknown-elf-gdb

OBJ = \
start.s \
sys.s \
lib.c \
timer.c \
task.c \
os.c \
user.c \
trap.c \
lock.c \
plic.c

QEMU = qemu-system-riscv32
QFLAGS = -nographic -smp 1 -machine virt -bios none

OBJDUMP = riscv32-unknown-linux-gnu-objdump

all: os.elf

os.elf: $(OBJ)
	$(CC) $(CFLAGS) -T os.ld -o os.elf $^
	riscv32-unknown-linux-gnu-objcopy -O binary os.elf os.bin

qemu: $(TARGET)
	@qemu-system-riscv32 -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	$(QEMU) $(QFLAGS) -kernel os.elf

clean:
	rm -f *.elf

.PHONY : debug
debug: all
	@echo "Press Ctrl-C and then input 'quit' to exit GDB and QEMU"
	@echo "-------------------------------------------------------"
	@${QEMU} ${QFLAGS} -kernel os.elf -s -S &
	@${GDB} os.elf -q -x ./gdbinit
