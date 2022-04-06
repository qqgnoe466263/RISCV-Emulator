#ifndef _CPU_H_
#define _CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "bus.h"

typedef struct riscv_cpu rv_cpu_t;
typedef struct riscv_csr rv_csr_t;

typedef uint32_t (*riscv_read_bus)(rv_bus_t *rv_bus, uint32_t addr, uint32_t size);
typedef void (*riscv_write_bus)(rv_bus_t *rv_bus, uint32_t addr, uint32_t data, uint32_t size);

#define REG_NUM 32

#define MSTATUS_MIE     (1 << 3)
#define MSTATUS_MPIE    (1 << 7)

#define MIP_MTIP        (1 << 7)

// Machine ISA register
// Machine Trap-Vector base address register
// Machine STATUS register
// Machine Trap Value register
// Machine CAUSE register
// Machine SCRATCH register
// Machine Exception Program Counter register
// Machine Interrupt Pending register
enum {
    MSTATUS = 0x300,
    MISA = 0x301,
    MIE = 0x304,
    MTVEC = 0x305,
    MSCRATCH = 0x340,
    MEPC = 0x341,
    MCAUSE = 0x342,
    MTVAL = 0x343,
    MIP = 0x344,
    MHARTID = 0xf14,
};

struct riscv_csr {
    uint8_t privilege;
    uint32_t value;
};

struct riscv_cpu {
    /* General Purpose Registers */
    uint32_t xreg[REG_NUM];
    /* Control and Status Registers */
    rv_csr_t *csr;
    /* Program Counter*/
    uint32_t pc;

    /* Branch or Jump flag */
    int32_t branch_enable;

    int32_t exception_code;

    /* Private - Instr */
    uint32_t fetch_instr;
    uint8_t opcode;
    uint8_t rd;
    uint8_t func3;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t rl;
    uint8_t aq;
    uint8_t func5;
    uint8_t func7;
    uint32_t imm;

	/* Private - BUS Interface */
	riscv_read_bus rv_read_bus;
	riscv_write_bus rv_write_bus;
};

enum {
    INSTRUCTION_ADDRESS_MISALIGNED = 0,
    INSTRUCTION_ACCESS_FAULT = 1,
    ILLEGAL_INSTRUCTION = 2,
    BREAKPOINT = 3,
    LOAD_ADDRESS_MISALIGNED = 4,
    LOAD_ACCESS_FAULT = 5,
} exception;

#define INTERRUPT_MASK  0x80000000
enum {
    MACHINE_TIMER_INTERRUPT = 7,
} interrupt;


/* Init CPU */
int32_t init_rv_cpu(rv_cpu_t *rv_cpu);

/* Fetch the instruction from emulator memory */
void fetch_rv_instr(rv_cpu_t *rv_cpu);

/* Decode the instruction */
uint32_t decode_rv_instr(rv_cpu_t *rv_cpu);

/* Execute the instruction && Write Back stage */
uint32_t execution_rv_instr(rv_cpu_t *rv_cpu);

/* READ CSR */
/* WRITE CSR */

void interrupt_handler(rv_cpu_t *rv_cpu);
void exception_handler(rv_cpu_t *rv_cpu);

/* Debug */
void dump_reg(rv_cpu_t *rv_cpu);
void dump_csr(rv_cpu_t *rv_cpu);

#endif
