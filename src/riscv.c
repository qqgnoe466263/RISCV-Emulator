#include "riscv.h"
#include "uart.h"

rv_emu_t *init_rv_emu()
{
    int err;
    rv_emu_t *rv_emu = (rv_emu_t *)malloc(sizeof(rv_emu_t));
    
    if (rv_emu == NULL) {
        ERR("[ERROR] Allocate rv_emu failed!\n");
        return NULL;
    }
    
    /* Init CPU */
    err = init_rv_cpu(&rv_emu->rv_cpu);

    /* Init MEM */
    err = init_rv_mem(&rv_emu->rv_mem);

    /* Init BUS */
    err = init_rv_bus(&rv_emu->rv_bus);

    return rv_emu;
}

int32_t load_rv_elf(rv_emu_t *rv_emu, uint8_t *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        ERR("[ERROR] Open %s error!\n", filename);
        return ERROR;
    }

    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (file_size == 0) {
        fclose(f);
        return ERROR;
    }

#if CONFIG_DBG
    DBG("[DEBUG] ELF Size : 0x%lx\n", file_size);
#endif
    if (file_size > IMEM_SIZE) {
        ERR("[ERROR] ELF size is large than I MEM Size!\n");
        return ERROR;
    }

    elf_raw_size = file_size;

    size_t r = fread(rv_emu->rv_mem.mem_base, 1, file_size, f);
    fclose(f);
    if (r != file_size)
        return ERROR;

    return 0;
}

static void tick_clint(rv_emu_t *rv_emu)
{
    uint32_t mtime = ++rv_emu->rv_mem.mem_base[MTIME/4];
    uint32_t mtimecmp = rv_emu->rv_mem.mem_base[MTIMECMP/4];

    // TODO : Software Interrupt

    if (mtimecmp != 0 && mtime >= mtimecmp)
        rv_emu->rv_cpu.csr[MIP].value = (1 << 7);
#if 1
    else
        /* TODO : This is a workaround, doesn't defined by spec */
        rv_emu->rv_cpu.csr[MIP].value &= ~(1 << 7);
#endif
}

static void tick_plic(rv_emu_t *rv_emu)
{
    uint8_t *uart0_lsr = (uint8_t *)rv_emu->rv_mem.mem_base + UART0_LSR;
    uint8_t *uart0_thr = (uint8_t *)rv_emu->rv_mem.mem_base + UART0_THR;

    /* TODO : UART handler */
    if (*uart0_thr) {
        /* PLIC IRQ */
        *uart0_lsr &= ~(UART0_LSR_THR_EMPTY | UART0_LSR_THR_SR_EMPTY);
        printf("%c", *uart0_thr);
        *uart0_thr = '\x00';
        *uart0_lsr = (UART0_LSR_THR_EMPTY | UART0_LSR_THR_SR_EMPTY);
    }
}

int32_t tick(rv_emu_t *rv_emu)
{
    tick_clint(rv_emu);
    tick_plic(rv_emu);

    return 1;
}
