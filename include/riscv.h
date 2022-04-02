#ifndef _RISCV_H_
#define _RISCV_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "mem.h"
#include "bus.h"
#include "debug.h"

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({\
        const typeof( ((type *)0)->member ) *__mptr = (ptr); \
        (type *)( (char *)__mptr - offsetof(type,member));})

/* CPU -> BUS -> MEM
 *            -> Peripheral (Maybe?)
 */
typedef struct riscv_emu {
    rv_cpu_t rv_cpu;
    rv_bus_t rv_bus;
    rv_mem_t rv_mem;

} rv_emu_t;

/* Init the risc-v emulator */
rv_emu_t *init_rv_emu();

/* Load the risc-v executable file to emulator memory */
int32_t load_rv_elf(rv_emu_t *rv_emu, uint8_t *filename);

// TODO : destroy emulator, free mem ....

#endif 
