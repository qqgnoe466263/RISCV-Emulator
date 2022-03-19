#include <stdio.h>
#include <stdlib.h>

#include "riscv.h"

const char reg_abi_name[][5] = {
    "zero", "  ra", "  sp", "  gp", "  tp", "  t0", "  t1",
    "  t2", "  s0", "  s1", "  a0", "  a1", "  a2", "  a3",
    "  a4", "  a5", "  a6", "  a7", "  s2", "  s3", "  s4",
    "  s5", "  s6", "  s7", "  s8", "  s9", " s10", " s11",
    "  t3", "  t4", "  t5", "  t6",
};

static void reg_info(rv_emu_t *rv_emu)
{
    uint32_t i;
    uint32_t j;
    uint32_t k;

    for (i = 1; i < (REG_NUM / 4) - 1; i++) {
        for (j = 0; j <= 5; j++) {
            k = i + j * 6 - 1;
            if (k >= REG_NUM)
                break;
            printf("x%02d(%s) = 0x%08x ", k, reg_abi_name[k], rv_emu->rv_cpu.xreg[k]);
        }
        printf("\n");
    }

    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++) {
            k = i + j * 5;
            printf("[STACK - %02d] 0x%08x ", k * 4, read_rv_mem(rv_emu, STACK_BEGIN - k, 4));
        }
        printf("\n");
    }
    
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
    
    while (!fetch_rv_instr(rv_emu)) {
        decode_rv_instr(rv_emu); 
        execution_rv_instr(rv_emu);
        reg_info(rv_emu);
        rv_emu->rv_cpu.pc += 4;
        
        /* TODO */
        if (rv_emu->rv_cpu.pc == 0)
            break;
    }

    /* TODO : Free rv_emu (mem ...) */
}
