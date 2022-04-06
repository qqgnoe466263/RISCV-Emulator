#ifndef _MEM_H_
#define _MEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* MEM Layout
 *      .
 *      .
 *  |-------| <- VIRT_UART0
 *  |       |
 *      .
 *      .
 *  |-------| <- VIRT_CLINT
 *      .
 *      .
 *  |-------| <- STACK_END, DMEM_END
 *  |  4MB  | 
 *  |-------| <- DMEM_BEGIN, IMEM_END
 *  |  4MB  | 
 *  |-------| <- IMEM_BEGIN
 */
#define IMEM_SIZE       (uint32_t)(4 * 1024 * 1024) // 4MB
#define DMEM_SIZE       (uint32_t)(4 * 1024 * 1024) // 4MB

#define VIRT_UART0      0x10000000 // 256MB
#define VIRT_UART0_LEN  0x100

#define MEM_SIZE        (VIRT_UART0 + VIRT_UART0_LEN)

#define IMEM_BASE       0x0
#define DMEM_BASE       (IMEM_BASE + IMEM_SIZE)

#define IMEM_BEGIN      IMEM_BASE
#define IMEM_END        (IMEM_BEGIN + IMEM_SIZE)

#define DMEM_BEGIN      DMEM_BASE
#define DMEM_END        (DMEM_BEGIN + DMEM_SIZE)

typedef struct riscv_mem rv_mem_t;
typedef struct riscv_clint rv_clint_t;

/* Private */
uint32_t elf_raw_size;

struct riscv_mem {
    uint32_t *mem_base; 
};

#define VIRT_CLINT      0x2000000 // Software or Timer
#define VIRT_CLINT_LEN  0x10000
#define MSIP            VIRT_CLINT
#define MTIMECMP        (VIRT_CLINT + 0x4000)
#define MTIME           (VIRT_CLINT + 0xbff8)
struct riscv_clint {
    uint32_t msip;
    uint32_t mtimecmp;
    uint32_t mtime;
};

/* MEM */
int32_t init_rv_mem(rv_mem_t *rv_mem);
uint32_t read_rv_mem(rv_mem_t *rv_mem, uint32_t addr, uint32_t size);
void write_rv_mem(rv_mem_t *rv_mem, uint32_t addr, uint32_t data, uint32_t size);

#endif
