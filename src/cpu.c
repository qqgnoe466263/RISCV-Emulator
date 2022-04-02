#include "cpu.h"
#include "mem.h"

/* CSRs Init */
rv_csr_t rv_csr[] = {
    [MISA]      = {0, 0},
    [MTVEC]     = {0, 0},
    [MSTATUS]   = {0, 0},
    [MSCRATCH]  = {0, 0},
    [MEPC]      = {0, 0},
    [MCAUSE]    = {0, 0},
    [MTVAL]     = {0, 0},
    [MIP]       = {0, 0},
    [MHARTID]   = {0, 0},
};

int32_t init_rv_cpu(rv_cpu_t *rv_cpu)
{
    /* Set PC to the begin of mem */
    rv_cpu->pc = IMEM_BEGIN;

    rv_cpu->csr = rv_csr;

    /* Reset the registers */
    for (uint32_t i = 0; i < REG_NUM; i++)
        rv_cpu->xreg[i] = 0;
    
    rv_cpu->exception_code = -1;

    /* Set the SP */
    rv_cpu->xreg[2] = STACK_BEGIN; // sp
#if CONFIG_DBG
    DBG("[DEBUG] STACK begin : 0x%x\n", rv_cpu->xreg[2]);
#endif

    /* Attach the BUS interfaces */
    rv_cpu->rv_read_bus = read_bus;
    rv_cpu->rv_write_bus = write_bus;
}

void dump_reg(rv_cpu_t *rv_cpu)
{
    static char *reg_abi_name[] = {
        "zr", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
        "a1", "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
        "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

    printf("pc = 0x%x\n", rv_cpu->pc);
    for (uint32_t i = 0; i < 32; i++) {
        printf("x%-2d(%-3s) = 0x%-8x,", i, reg_abi_name[i], rv_cpu->xreg[i]);
        if (!((i + 1) & 3))
            printf("\n");
    }
    printf("\n");
}
