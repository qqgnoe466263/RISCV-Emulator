#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "mem.h"
#include "bus.h"
#include "uart.h"
#include "riscv.h"

void uart_init(rv_emu_t *rv_emu)
{
    /* Init UART TX */
    *((uint8_t *)rv_emu->rv_mem.mem_base + UART0_LSR) = (UART0_LSR_THR_EMPTY | UART0_LSR_THR_SR_EMPTY);
}

int main(int argc, char **argv)
{
    rv_emu_t *rv_emu;

    rv_emu = init_rv_emu();
    if (rv_emu == NULL) {
        return ERROR;
    }

    if (load_rv_elf(rv_emu, argv[1])) {
        return ERROR;    
    }
    
    uart_init(rv_emu);

    while (tick(rv_emu)) {
        /* TODO : interrupt_handler */
        interrupt_handler(&rv_emu->rv_cpu);

#if 0 /* Debug */
        if (rv_emu->rv_cpu.pc == 0x80b4)
            dump_reg(&rv_emu->rv_cpu);
#endif
        /* x0 always is 0 */
        rv_emu->rv_cpu.xreg[0] = 0;

        fetch_rv_instr(&rv_emu->rv_cpu);
        decode_rv_instr(&rv_emu->rv_cpu); 
        execution_rv_instr(&rv_emu->rv_cpu);

        /* PC Sel */
        if (rv_emu->rv_cpu.branch_enable)
            rv_emu->rv_cpu.branch_enable = 0;
        else
            rv_emu->rv_cpu.pc += 4;

        /* TODO : Exception Handler */

        /* TODO : Stop Machine */
        if (rv_emu->rv_cpu.pc == 0)
            break;
    }

#if 1
    dump_reg(&rv_emu->rv_cpu);
    dump_csr(&rv_emu->rv_cpu);
    printf("%-10s = 0x%x \n", "MSIP", *((uint8_t *)rv_emu->rv_mem.mem_base + MSIP));
    printf("%-10s = 0x%x \n", "MTIMECMP", rv_emu->rv_mem.mem_base[MTIMECMP/4]);
    printf("%-10s = 0x%x \n", "MTIME", *((uint8_t *)rv_emu->rv_mem.mem_base + MTIME));
#endif


    /* TODO : Free rv_emu (mem ...) */
}
