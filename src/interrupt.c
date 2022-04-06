#include "riscv.h"
#include "cpu.h"

void interrupt_handler(rv_cpu_t *rv_cpu)
{
    uint32_t pending = rv_cpu->csr[MIP].value & rv_cpu->csr[MIE].value;

    /* Machine Timer Interrupt */
    /* MTIP of MIP */
    if (pending & MIP_MTIP) {
        /* Check MIE of MSTATUS enable or not */
        if ((rv_cpu->csr[MSTATUS].value & MSTATUS_MIE)) {
            /* 3. MEPC Saves the current PC which cause Interrupt/Exception. */
            rv_cpu->csr[MEPC].value = rv_cpu->pc;
            /* 1. When an Interrupt/Exception occurs, PC will be updated by MTVEC. */
            rv_cpu->pc = rv_cpu->csr[MTVEC].value;
            /* 2. Update the MCAUSE. It talks to CPU which trap occurs. */
            rv_cpu->csr[MCAUSE].value = (INTERRUPT_MASK | MACHINE_TIMER_INTERRUPT);
            /* 5. Disable MIE (Machine Mode Global Interrupt) */
            rv_cpu->csr[MSTATUS].value = (rv_cpu->csr[MSTATUS].value & ~MSTATUS_MPIE) |
                                         ((rv_cpu->csr[MSTATUS].value & MSTATUS_MIE) << 4);
            rv_cpu->csr[MSTATUS].value &= ~MSTATUS_MIE;

            /* If interrupt has been handled, resume MIP */
            rv_cpu->csr[MIP].value &= ~(1 << 7);
        }
    }
}
