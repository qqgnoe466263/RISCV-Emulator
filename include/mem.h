#ifndef _MEM_H_
#define _MEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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

typedef struct riscv_mem rv_mem_t;

/* Private */
uint32_t elf_raw_size;

struct riscv_mem {
    uint32_t *mem_base; 
};

/* MEM */
int32_t init_rv_mem(rv_mem_t *rv_mem);
uint32_t read_rv_mem(rv_mem_t *rv_mem, uint32_t addr, uint32_t size);
void write_rv_mem(rv_mem_t *rv_mem, uint32_t addr, uint32_t data, uint32_t size);

#endif
