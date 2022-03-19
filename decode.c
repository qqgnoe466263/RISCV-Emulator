#include "riscv.h"

static void decode_r_type_instr(rv_emu_t *rv_emu)
{
    rv_emu->rv_mem.rd    = (rv_emu->rv_mem.fetch_instr >> 7) & 0x1f;
    rv_emu->rv_mem.func3 = (rv_emu->rv_mem.fetch_instr >> 12) & 0x3;
    rv_emu->rv_mem.rs1   = (rv_emu->rv_mem.fetch_instr >> 15) & 0x1f;
    rv_emu->rv_mem.rs2   = (rv_emu->rv_mem.fetch_instr >> 20) & 0x1f;
    rv_emu->rv_mem.func7 = (rv_emu->rv_mem.fetch_instr >> 25) & 0x7f;
#ifdef CONFIG_DBG
    DBG("[DEBUG] R-type, rd = 0x%x, func3 = 0x%x, rs1 = 0x%x, rs2 = 0x%x, func7 = 0x%x \n",
                rv_emu->rv_mem.rd, rv_emu->rv_mem.func3, rv_emu->rv_mem.rs1, 
                rv_emu->rv_mem.rs2, rv_emu->rv_mem.func7);
#endif
}

static void decode_i_type_instr(rv_emu_t *rv_emu)
{
    rv_emu->rv_mem.rd    = (rv_emu->rv_mem.fetch_instr >> 7) & 0x1f;
    rv_emu->rv_mem.func3 = (rv_emu->rv_mem.fetch_instr >> 12) & 0x3;
    rv_emu->rv_mem.rs1   = (rv_emu->rv_mem.fetch_instr >> 15) & 0x1f;
    rv_emu->rv_mem.imm   = (int32_t)(rv_emu->rv_mem.fetch_instr & 0xfff00000) >> 20;
#ifdef CONFIG_DBG
    DBG("[DEBUG] I-type, rd = 0x%x, func3 = 0x%x, rs1 = 0x%x, imm = 0x%x\n",
                rv_emu->rv_mem.rd, rv_emu->rv_mem.func3, rv_emu->rv_mem.rs1, 
                rv_emu->rv_mem.imm);
#endif
}

static void decode_s_type_instr(rv_emu_t *rv_emu)
{
    rv_emu->rv_mem.func3 = (rv_emu->rv_mem.fetch_instr >> 12) & 0x3;
    rv_emu->rv_mem.rs1   = (rv_emu->rv_mem.fetch_instr >> 15) & 0x1f;
    rv_emu->rv_mem.rs2   = (rv_emu->rv_mem.fetch_instr >> 20) & 0x1f;
    rv_emu->rv_mem.imm   = ((int32_t)(rv_emu->rv_mem.fetch_instr & 0xfe000000) >> 20) | 
                           ((int32_t)((rv_emu->rv_mem.fetch_instr >> 7) & 0x1f));
#ifdef CONFIG_DBG
    DBG("[DEBUG] S-type, func3 = 0x%x, rs1 = 0x%x, rs2 = 0x%x, imm = 0x%x\n",
                rv_emu->rv_mem.func3, rv_emu->rv_mem.rs1, rv_emu->rv_mem.rs2, 
                rv_emu->rv_mem.imm);
#endif
}
static void decode_b_type_instr(rv_emu_t *rv_emu)
{
    rv_emu->rv_mem.func3 = (rv_emu->rv_mem.fetch_instr >> 12) & 0x3;
    rv_emu->rv_mem.rs1   = (rv_emu->rv_mem.fetch_instr >> 15) & 0x1f;
    rv_emu->rv_mem.rs2   = (rv_emu->rv_mem.fetch_instr >> 20) & 0x1f;
    /* This field composed by imm[12|10:5] imm[4:1|11];
     * It means :
     *     imm[12] : instr[31]
     *     imm[11] : instr[7]
     *     imm[10] : instr[30]
     *     imm[ 5] : instr[25]
     *     ...
     * Ref : https://ithelp.ithome.com.tw/articles/10194907 */
    rv_emu->rv_mem.imm   = ((int32_t)(rv_emu->rv_mem.fetch_instr & 0x80000000) >> 19) // bit12 (31 - 19)
                           | (int32_t)((rv_emu->rv_mem.fetch_instr & 0x80) << 4) // bit 11 (7 + 4)
                           | (int32_t)((rv_emu->rv_mem.fetch_instr >> 20) & 0x7e0) // bit 10:5
                           | (int32_t)((rv_emu->rv_mem.fetch_instr >> 7) & 0x1e); // bit 4:1
#ifdef CONFIG_DBG
    DBG("[DEBUG] B-type, func3 = 0x%x, rs1 = 0x%x, rs2 = 0x%x, imm = 0x%x\n",
                rv_emu->rv_mem.func3, rv_emu->rv_mem.rs1, rv_emu->rv_mem.rs2,
                rv_emu->rv_mem.imm);
#endif
}

static void decode_u_type_instr(rv_emu_t *rv_emu)
{
    rv_emu->rv_mem.rd  = (rv_emu->rv_mem.fetch_instr >> 7) & 0x1f;
    rv_emu->rv_mem.imm = (int32_t)(rv_emu->rv_mem.fetch_instr & 0xfffff000);
#ifdef CONFIG_DBG
    DBG("[DEBUG] S-type, rd = 0x%x, imm = 0x%x\n",
                rv_emu->rv_mem.rd, rv_emu->rv_mem.imm);
#endif
}

static void decode_j_type_instr(rv_emu_t *rv_emu)
{
    rv_emu->rv_mem.rd  = (rv_emu->rv_mem.fetch_instr >> 7) & 0x1f;
    rv_emu->rv_mem.imm = ((int32_t)(rv_emu->rv_mem.fetch_instr & 0x80000000) >> 11) // bit20
                         | (int32_t)((rv_emu->rv_mem.fetch_instr & 0xff000)) // bit 19:12
                         | (int32_t)((rv_emu->rv_mem.fetch_instr >> 9) & 0x800) // bit 11
                         | (int32_t)((rv_emu->rv_mem.fetch_instr >> 20) & 0x7fe); // bit 10:1
#ifdef CONFIG_DBG
    DBG("[DEBUG] J-type, opcode = 0x%x, rd = 0x%x, imm = 0x%x\n",
                rv_emu->rv_mem.opcode, rv_emu->rv_mem.rd, rv_emu->rv_mem.imm);
#endif
}

uint32_t decode_rv_instr(rv_emu_t *rv_emu)
{
    uint32_t opcode = rv_emu->rv_mem.fetch_instr & 0x7f;
    rv_emu->rv_mem.opcode = opcode;

#ifdef CONFIG_DBG
        DBG("[PC : 0x%02x][Instr : 0x%08x]", 
                rv_emu->rv_cpu.pc, rv_emu->rv_mem.fetch_instr);
#endif

    switch (opcode) {
    case 0x33: // R-type
        decode_r_type_instr(rv_emu);
        break;
    case 0x03: // I-type
    case 0x13:
    case 0x67:
        decode_i_type_instr(rv_emu);
        break;
    case 0x23: // S-type
        decode_s_type_instr(rv_emu);
        break; 
    case 0x63: // B-type
        decode_b_type_instr(rv_emu);
        break; 
    case 0x17: // U-type
    case 0x37:
        decode_u_type_instr(rv_emu);
        break;
    case 0x6f: // J-type
        decode_j_type_instr(rv_emu);
        break;
    default:
        ERR("[ERROR] Unknown Instruction !\n");
    }
}


