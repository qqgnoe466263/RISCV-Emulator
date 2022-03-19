#include "riscv.h"

rv_emu_t *init_rv_emu()
{
    int err;
    rv_emu_t *rv_emu = (rv_emu_t *)malloc(sizeof(rv_emu_t));
    
    if (rv_emu == NULL) {
        ERR("[ERROR] Allocate rv_emu failed!\n");
        return NULL;
    }
    
    /* cpu -> bus -> mem (I-MEM, D-MEM)
     *            -> Peripheral 
     */

    /* Init CPU */
    rv_emu->rv_cpu.pc = IMEM_BEGIN;
    for (uint32_t i = 0; i < REG_NUM; i++)
        rv_emu->rv_cpu.xreg[i] = 0;
    
    rv_emu->rv_cpu.xreg[2] = STACK_BEGIN; // sp

#if CONFIG_DBG
    DBG("[DEBUG] STACK begin : 0x%x\n", rv_emu->rv_cpu.xreg[2]);
#endif
   
    /* Init MEM */
    err = init_rv_mem(rv_emu);
    if (err)
        return NULL;

    /* Init BUS */
    init_rv_bus(rv_emu);

    return rv_emu;
}

int32_t load_rv_elf(rv_emu_t *rv_emu, uint8_t *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        ERR("[ERROR] Open %s error!\n", filename);
        return ERROR;
    }

    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (file_size == 0) {
        fclose(f);
        return ERROR;
    }

#ifdef CONFIG_DBG
    DBG("[DEBUG] ELF Size : 0x%lx\n", file_size);
#endif
    if (file_size > IMEM_SIZE) {
        ERR("[ERROR] ELF size is large than I MEM Size!\n");
        return ERROR;
    }

    rv_emu->rv_mem.elf_raw_size = file_size;

    size_t r = fread(rv_emu->rv_mem.mem_base, 1, file_size, f);
    fclose(f);
    if (r != file_size)
        return ERROR;

    return 0;
}

