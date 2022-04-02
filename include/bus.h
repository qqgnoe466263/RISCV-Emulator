#ifndef _BUS_H_
#define _BUS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct riscv_bus rv_bus_t;

/* The BUS need to know where the address of variety memory */
struct riscv_bus {
    uint32_t imem_base_addr;
    uint32_t dmem_base_addr;
};

/* BUS */
int32_t init_rv_bus(rv_bus_t *rv_bus);
uint32_t read_bus(rv_bus_t *rv_bus, uint32_t addr, uint32_t size);
void write_bus(rv_bus_t *rv_bus, uint32_t addr, uint32_t data, uint32_t size);

#endif
