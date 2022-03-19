#include "riscv.h"

int32_t fetch_rv_instr(rv_emu_t *rv_emu)
{
    if (rv_emu->rv_cpu.pc >= rv_emu->rv_mem.elf_raw_size) {
        ERR("[ERROR] Fetch an invalid address (PC : 0x%x)!\n", rv_emu->rv_cpu.pc);
        return ERROR;
    }
    
    /* Fetch 32bits wide a time */
    rv_emu->rv_mem.fetch_instr = 
            read_bus(rv_emu, rv_emu->rv_cpu.pc, 4); 
    
    return 0;
}
