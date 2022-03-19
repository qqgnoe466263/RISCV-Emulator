#ifndef _RISCV_H_
#define _RISCV_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "debug.h"

/* MEM Layout
 *  |-------| <- STACK_BEGIN
 *  |  1MB  | 
 *  |-------| <- STACK_END, DMEM_END
 *  |  4MB  | 
 *  |-------| <- DMEM_BEGIN, IMEM_END
 *  |  4MB  | 
 *  |-------| <- IMEM_BEGIN
 */
#define IMEM_SIZE       (uint32_t)(4 * 1024 * 1024) // 4MB
#define DMEM_SIZE       (uint32_t)(4 * 1024 * 1024) // 4MB
#define STACK_SIZE      (uint32_t)(1 * 1024 * 1024) // 1MB
#define MEM_SIZE        (IMEM_SIZE + DMEM_SIZE + STACK_SIZE)

#define IMEM_BASE       0x0
#define DMEM_BASE       IMEM_BASE + IMEM_SIZE

#define IMEM_BEGIN      IMEM_BASE
#define IMEM_END        IMEM_BEGIN + IMEM_SIZE

#define DMEM_BEGIN      DMEM_BASE
#define DMEM_END        DMEM_BEGIN + DMEM_SIZE

#define STACK_BEGIN     DMEM_END + STACK_SIZE

#define REG_NUM 32

typedef struct riscv_cpu rv_cpu_t;
typedef struct riscv_mem rv_mem_t;

/*
union instruction {
    struct {
        uint32_t opcode:7;
        uint32_t rd:5;
        uint32_t func3:3;
        uint32_t rs1:5;
        uint32_t rs2:5;
        uint32_t func7:7;
    } fields;
    uint32_t instr;
};
*/

struct riscv_cpu {
    uint32_t xreg[REG_NUM];
    uint32_t pc;
};

struct riscv_mem {
    uint32_t *mem_base; 
    uint32_t imem_base_addr;
    uint32_t dmem_base_addr;
    uint32_t elf_raw_size;

    uint32_t fetch_instr;
    uint8_t opcode;
    uint8_t rd;
    uint8_t func3;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t func7;
    uint32_t imm;
};

typedef struct riscv_emu {
    rv_cpu_t rv_cpu;
    rv_mem_t rv_mem;
} rv_emu_t;

/* Init the risc-v emulator */
rv_emu_t *init_rv_emu();

/**/
int32_t init_rv_mem(rv_emu_t *rv_emu);
// TODO : read mem
uint32_t read_rv_mem(rv_emu_t *rv_emu, uint32_t addr, uint32_t size);
// TODO : write mem
void write_rv_mem(rv_emu_t *rv_emu, uint32_t addr, uint32_t data, uint32_t size);

/* TODO */
int32_t init_rv_bus(rv_emu_t *rv_emu);
uint32_t read_bus(rv_emu_t *rv_emu, uint32_t addr, uint32_t size);
void write_bus(rv_emu_t *rv_emu, uint32_t addr, uint32_t data, uint32_t size);

/* Load the risc-v executable file to emulator memory */
int32_t load_rv_elf(rv_emu_t *rv_emu, uint8_t *filename);

/* Fetch the instruction from emulator memory */
int32_t fetch_rv_instr(rv_emu_t *rv_emu);

/* Decode the instruction */
uint32_t decode_rv_instr(rv_emu_t *rv_emu);

/* Execute the instruction */
uint32_t execution_rv_instr(rv_emu_t *rv_emu);

/* Write Back the result to emulator memory or register */


// TODO : destroy emulator, free mem ....

#endif 
