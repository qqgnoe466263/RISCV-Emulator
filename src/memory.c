#include "riscv.h"

int32_t init_rv_mem(rv_mem_t *rv_mem)
{
    rv_mem->mem_base = (uint32_t *)malloc(MEM_SIZE);     
    if (rv_mem->mem_base == NULL) {
        ERR("ERROR Allocate mem_base failed!\n");
        return ERROR;
    }

    memset(rv_mem->mem_base, '\x00', MEM_SIZE);

    return 0;
}

uint32_t read_rv_mem(rv_mem_t *rv_mem, uint32_t addr, uint32_t size)
{
    uint32_t idx = addr / 4; // TODO
    uint32_t val = 0;

    switch (size) {
    /* 1bytes */
    case 1:
        val = *((uint8_t *)rv_mem->mem_base + addr);
        break;
    /* 2bytes */
    case 2:
        val = *(uint16_t *)&rv_mem->mem_base[idx];
        break;
    /* 4bytes */
    case 4:
        val = *(uint32_t *)&rv_mem->mem_base[idx];
        break;
    } 
    return val;
}

void write_rv_mem(rv_mem_t *rv_mem, uint32_t addr, uint32_t data, uint32_t size)
{
    uint32_t idx = addr / 4; // TODO

    switch (size) {
    /* 1bytes */
    case 1:
        *((uint8_t *)rv_mem->mem_base + addr) = (uint8_t)data;
        break;
    /* 2bytes */
    case 2:
        rv_mem->mem_base[idx] = (uint16_t)data;
        break;
    /* 4bytes */
    case 4:
        rv_mem->mem_base[idx] = (uint32_t)data;
        break;
    } 
}
