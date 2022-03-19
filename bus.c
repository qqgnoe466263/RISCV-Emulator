#include "riscv.h"

int32_t init_bus(rv_emu_t *rv_emu)
{
    
}

uint32_t read_bus(rv_emu_t *rv_emu, uint32_t addr, uint32_t size)
{
    uint32_t temp_addr;

    if (addr < IMEM_END) {
        temp_addr = rv_emu->rv_mem.imem_base_addr + addr;    
        return read_rv_mem(rv_emu, temp_addr, size);
    }

    if (addr < DMEM_END) {
        temp_addr = rv_emu->rv_mem.dmem_base_addr + addr;    
        return read_rv_mem(rv_emu, temp_addr, size);
    }

    if (addr < STACK_BEGIN) {
        return read_rv_mem(rv_emu, addr, size);
    }
}

void write_bus(rv_emu_t *rv_emu, uint32_t addr, uint32_t data, uint32_t size)
{
    if (addr < IMEM_END) {
    }

    if (addr < DMEM_END) {

    }

    if (addr < STACK_BEGIN) {
        write_rv_mem(rv_emu, addr, data, size);
    }
}
