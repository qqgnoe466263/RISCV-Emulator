#include "riscv.h"
#include "cpu.h"

static void rv_exception_load_access_fault(rv_cpu_t *rv_cpu)
{
    /* mtvec (Machine Trap-Vector Base Address Register) 
     * mtvec(MXLEN-1:2) : vector base address
     * mtvec[1:0]       : mode
     */
    uint32_t base = rv_cpu->csr[MTVEC].value & ~0x3; 
    uint32_t mode = rv_cpu->csr[MTVEC].value & 0x3;

    /* TODO */    
}

void exception_handler(rv_cpu_t *rv_cpu)
{
    switch (rv_cpu->exception_code) {
    case LOAD_ACCESS_FAULT:
        rv_exception_load_access_fault(rv_cpu); 
        break;
    }
}
