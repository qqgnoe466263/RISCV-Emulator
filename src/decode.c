#include "cpu.h"
#include "riscv.h"

static void decode_r_type_instr(rv_cpu_t *rv_cpu)
{
    rv_cpu->rd    = (rv_cpu->fetch_instr >> 7) & 0x1f;
    rv_cpu->func3 = (rv_cpu->fetch_instr >> 12) & 0x3;
    rv_cpu->rs1   = (rv_cpu->fetch_instr >> 15) & 0x1f;
    rv_cpu->rs2   = (rv_cpu->fetch_instr >> 20) & 0x1f;
    rv_cpu->func7 = (rv_cpu->fetch_instr >> 25) & 0x7f;
#ifdef CONFIG_DBG
    DBG("[DEBUG] R-type, rd = 0x%x, func3 = 0x%x, rs1 = 0x%x, rs2 = 0x%x, func7 = 0x%x \n",
                rv_cpu->rd, rv_cpu->func3, rv_cpu->rs1, 
                rv_cpu->rs2, rv_cpu->func7);
#endif
}

static void decode_i_type_instr(rv_cpu_t *rv_cpu)
{
    rv_cpu->rd    = (rv_cpu->fetch_instr >> 7) & 0x1f;
    rv_cpu->func3 = (rv_cpu->fetch_instr >> 12) & 0x3;
    rv_cpu->rs1   = (rv_cpu->fetch_instr >> 15) & 0x1f;
    rv_cpu->imm   = (int32_t)(rv_cpu->fetch_instr & 0xfff00000) >> 20;
#ifdef CONFIG_DBG
    DBG("[DEBUG] I-type, rd = 0x%x, func3 = 0x%x, rs1 = 0x%x, imm = 0x%x\n",
                rv_cpu->rd, rv_cpu->func3, rv_cpu->rs1, 
                rv_cpu->imm);
#endif
}

static void decode_s_type_instr(rv_cpu_t *rv_cpu)
{
    rv_cpu->func3 = (rv_cpu->fetch_instr >> 12) & 0x3;
    rv_cpu->rs1   = (rv_cpu->fetch_instr >> 15) & 0x1f;
    rv_cpu->rs2   = (rv_cpu->fetch_instr >> 20) & 0x1f;
    rv_cpu->imm   = ((int32_t)(rv_cpu->fetch_instr & 0xfe000000) >> 20) | 
                           ((int32_t)((rv_cpu->fetch_instr >> 7) & 0x1f));
#ifdef CONFIG_DBG
    DBG("[DEBUG] S-type, func3 = 0x%x, rs1 = 0x%x, rs2 = 0x%x, imm = 0x%x\n",
                rv_cpu->func3, rv_cpu->rs1, rv_cpu->rs2, 
                rv_cpu->imm);
#endif
}
static void decode_b_type_instr(rv_cpu_t *rv_cpu)
{
    rv_cpu->func3 = (rv_cpu->fetch_instr >> 12) & 0x3;
    rv_cpu->rs1   = (rv_cpu->fetch_instr >> 15) & 0x1f;
    rv_cpu->rs2   = (rv_cpu->fetch_instr >> 20) & 0x1f;
    /* This field composed by imm[12|10:5] imm[4:1|11];
     * It means :
     *     imm[12] : instr[31]
     *     imm[11] : instr[7]
     *     imm[10] : instr[30]
     *     imm[ 5] : instr[25]
     *     ...
     * Ref : https://ithelp.ithome.com.tw/articles/10194907 */
    rv_cpu->imm   = ((int32_t)(rv_cpu->fetch_instr & 0x80000000) >> 19) // bit12 (31 - 19)
                           | (int32_t)((rv_cpu->fetch_instr & 0x80) << 4) // bit 11 (7 + 4)
                           | (int32_t)((rv_cpu->fetch_instr >> 20) & 0x7e0) // bit 10:5
                           | (int32_t)((rv_cpu->fetch_instr >> 7) & 0x1e); // bit 4:1
#ifdef CONFIG_DBG
    DBG("[DEBUG] B-type, func3 = 0x%x, rs1 = 0x%x, rs2 = 0x%x, imm = 0x%x\n",
                rv_cpu->func3, rv_cpu->rs1, rv_cpu->rs2,
                rv_cpu->imm);
#endif
}

static void decode_u_type_instr(rv_cpu_t *rv_cpu)
{
    rv_cpu->rd  = (rv_cpu->fetch_instr >> 7) & 0x1f;
    rv_cpu->imm = (int32_t)(rv_cpu->fetch_instr & 0xfffff000);
#ifdef CONFIG_DBG
    DBG("[DEBUG] S-type, rd = 0x%x, imm = 0x%x\n",
                rv_cpu->rd, rv_cpu->imm);
#endif
}

static void decode_j_type_instr(rv_cpu_t *rv_cpu)
{
    rv_cpu->rd  = (rv_cpu->fetch_instr >> 7) & 0x1f;
    rv_cpu->imm = ((int32_t)(rv_cpu->fetch_instr & 0x80000000) >> 11) // bit20
                         | (int32_t)((rv_cpu->fetch_instr & 0xff000)) // bit 19:12
                         | (int32_t)((rv_cpu->fetch_instr >> 9) & 0x800) // bit 11
                         | (int32_t)((rv_cpu->fetch_instr >> 20) & 0x7fe); // bit 10:1
#ifdef CONFIG_DBG
    DBG("[DEBUG] J-type, opcode = 0x%x, rd = 0x%x, imm = 0x%x\n",
                rv_cpu->opcode, rv_cpu->rd, rv_cpu->imm);
#endif
}

uint32_t decode_rv_instr(rv_cpu_t *rv_cpu)
{
    uint32_t opcode = rv_cpu->fetch_instr & 0x7f;
    rv_cpu->opcode = opcode;

#ifdef CONFIG_DBG
        DBG("[PC : 0x%02x][Instr : 0x%08x]", 
                rv_cpu->pc, rv_cpu->fetch_instr);
#endif

    switch (opcode) {
    case 0x33: // R-type
        decode_r_type_instr(rv_cpu);
        break;
    case 0x03: // I-type
    case 0x13:
    case 0x67:
    case 0x73: // System Instr
        decode_i_type_instr(rv_cpu);
        break;
    case 0x23: // S-type
        decode_s_type_instr(rv_cpu);
        break; 
    case 0x63: // B-type
        decode_b_type_instr(rv_cpu);
        break; 
    case 0x17: // U-type
    case 0x37:
        decode_u_type_instr(rv_cpu);
        break;
    case 0x6f: // J-type
        decode_j_type_instr(rv_cpu);
        break;
    default:
        rv_cpu->exception_code = LOAD_ACCESS_FAULT;
        ERR("[ERROR] Unknown Instruction !\n");
    }
}


