#include "cpu.h"
#include "riscv.h"

void fetch_rv_instr(rv_cpu_t *rv_cpu)
{
    rv_emu_t *rv_emu = container_of(rv_cpu, rv_emu_t, rv_cpu);
    rv_bus_t *rv_bus = &rv_emu->rv_bus;

                     /* TODO
                      * 1) use bus to get raw_size? 
                      * 2) to be a global var?
                      */
    if (rv_cpu->pc >= elf_raw_size) {
        ERR("[ERROR] Fetch an invalid address (PC : 0x%x)!\n", rv_cpu->pc);
        rv_cpu->exception_code = LOAD_ACCESS_FAULT;
        return;
    }
    
    /* Fetch 32bits wide a time */
    rv_cpu->fetch_instr = rv_cpu->rv_read_bus(rv_bus, rv_cpu->pc, 4); 
}
