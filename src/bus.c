#include "bus.h"
#include "riscv.h"

int32_t init_rv_bus(rv_bus_t *rv_bus)
{
    /* BUS attach */
    rv_bus->imem_base_addr = IMEM_BASE;
    rv_bus->dmem_base_addr = DMEM_BASE;

#if CONFIG_DBG
    DBG("[DEBUG] I MEM begin : 0x%x\n", rv_bus->imem_base_addr);
    DBG("[DEBUG] D MEM begin : 0x%x\n", rv_bus->dmem_base_addr);
#endif

}

uint32_t read_bus(rv_bus_t *rv_bus, uint32_t addr, uint32_t size)
{
    rv_emu_t *rv_emu = container_of(rv_bus, rv_emu_t, rv_bus);
    rv_cpu_t *rv_cpu = &rv_emu->rv_cpu;
    rv_mem_t *rv_mem = &rv_emu->rv_mem;

    if (addr > MEM_SIZE) {
        rv_cpu->exception_code = LOAD_ACCESS_FAULT;
        return 0;
    }

    return read_rv_mem(rv_mem, addr, size);
}

void write_bus(rv_bus_t *rv_bus, uint32_t addr, uint32_t data, uint32_t size)
{
    rv_emu_t *rv_emu = container_of(rv_bus, rv_emu_t, rv_bus);
    rv_cpu_t *rv_cpu = &rv_emu->rv_cpu;
    rv_mem_t *rv_mem = &rv_emu->rv_mem;

    if (addr > MEM_SIZE) {
        rv_cpu->exception_code = LOAD_ACCESS_FAULT;
        return;
    }

    write_rv_mem(rv_mem, addr, data, size);
}
