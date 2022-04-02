#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "mem.h"
#include "bus.h"
#include "riscv.h"

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
    
    while (1) {
        fetch_rv_instr(&rv_emu->rv_cpu);
        decode_rv_instr(&rv_emu->rv_cpu); 
        execution_rv_instr(&rv_emu->rv_cpu);
        rv_emu->rv_cpu.pc += 4;
        
        if (rv_emu->rv_cpu.exception_code > -1) {
            printf("\nException\n");
            //exception_handler(&rv_emu->rv_cpu); 
            break;
        }

        /* TODO */
        if (rv_emu->rv_cpu.pc == 0)
            break;
    }
    dump_reg(&rv_emu->rv_cpu);

    /* TODO : Free rv_emu (mem ...) */
}
