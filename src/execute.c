#include "cpu.h"
#include "riscv.h"

static void execution_r_type_instr(rv_cpu_t *rv_cpu)
{
    /* R-type
     *  31 - 25 24 - 20 19 - 15 14 - 12 11 - 7 6 - 0
     * | func7 |  rs2  |  rs1  | func3 |  rd  | op |
     */
    uint8_t func3 = rv_cpu->func3;
    uint8_t func7 = rv_cpu->func7;
    uint32_t *rd_reg = &rv_cpu->xreg[rv_cpu->rd];
    uint32_t *rs1_reg = &rv_cpu->xreg[rv_cpu->rs1];
    uint32_t *rs2_reg = &rv_cpu->xreg[rv_cpu->rs2];

    /* RV32M Multiply Extension */
    if (func7 == 0x1) {
        switch (func3) {
        case 0x0: // MUL
            *rd_reg = *rs1_reg * *rs2_reg;
            break;
        case 0x1: // MULH
            break;
        case 0x2: // MULSU
            break;
        case 0x3: // MULU
            break;
        case 0x4: // DIV
            *rd_reg = *rs1_reg / *rs2_reg;
            break;
        case 0x5: // DIVU
            break;
        case 0x6: // REM
            *rd_reg = *rs1_reg % *rs2_reg;
            break;
#if 0
        case 0x7: // REMU
            break;
#endif
        default:
            rv_cpu->exception_code = LOAD_ACCESS_FAULT;
        }
        return;
    }

    switch (func3) {
    case 0x0: 
        if (func7 == 0x0) // ADD
            *rd_reg = *rs1_reg + *rs2_reg; 
        else // SUB
            *rd_reg = *rs1_reg - *rs2_reg;
        break;
    case 0x1: // SLL (Shift Left Logical)
        *rd_reg = *rs1_reg << *rs2_reg;
        break;
    case 0x2: // SLT (Set Less Than)
        *rd_reg = (*rs1_reg < *rs2_reg) ? 1 : 0;
        break;
    case 0x3: // SLTU (Set Less Than U), Note : zero-extends
        *rd_reg = (*rs1_reg < *rs2_reg) ? 1 : 0;
        break;
    case 0x4: // XOR
        *rd_reg = *rs1_reg ^ *rs2_reg;
        break;
    case 0x5: 
        if (func7 == 0x0) // SRL (Shift Right Logical) 
            *rd_reg = *rs1_reg >> *rs2_reg;
        else // SRA (Shift Right Arith*), Note : msb-extends
            *rd_reg = *rs1_reg >> *rs2_reg;
        break;
    case 0x6: // OR
        *rd_reg = *rs1_reg | *rs2_reg;
        break;
    case 0x7: // AND
        *rd_reg = *rs1_reg & *rs2_reg;
        break;
    default:
        rv_cpu->exception_code = LOAD_ACCESS_FAULT;
    }
}

static void execution_rv32a_type_instr(rv_cpu_t *rv_cpu)
{
    rv_emu_t *rv_emu = container_of(rv_cpu, rv_emu_t, rv_cpu);
    rv_bus_t *rv_bus = &rv_emu->rv_bus;

    uint8_t func3 = rv_cpu->func3;
    uint8_t rl = rv_cpu->rl;
    uint8_t aq = rv_cpu->aq;
    uint8_t func5 = rv_cpu->func5;
    uint32_t *rd_reg = &rv_cpu->xreg[rv_cpu->rd];
    uint32_t *rs1_reg = &rv_cpu->xreg[rv_cpu->rs1];
    uint32_t *rs2_reg = &rv_cpu->xreg[rv_cpu->rs2];
    uint32_t a, b;

    switch (func5) {
    case 0x00: // AMOADD.W
        break;
    case 0x01: // AMOSWAP.W
        a = (uint32_t)read_bus(rv_bus, *rs1_reg, 4);
        b = *rs2_reg;
        // TODO : Imp SWAP
        *rs2_reg = a;
        rv_cpu->rv_write_bus(rv_bus, *rs1_reg, b, 4);
        break;
#if 0
    case 0x02: // LR.W
        break;
    case 0x03: // SC.W
        break;
    case 0x04: // AMOXOR.W
        break;
    case 0x0A: // AMOOR.W
        break;
    case 0x0C: // AMOAND.W
        break;
    case 0x10: // AMOMIN.W
        break;
    case 0x14: // AMOMAX.W
        break;
#endif
    default:
        rv_cpu->exception_code = LOAD_ACCESS_FAULT;
    }
}

static void execution_i_type_instr(rv_cpu_t *rv_cpu)
{
    rv_emu_t *rv_emu = container_of(rv_cpu, rv_emu_t, rv_cpu);
    rv_bus_t *rv_bus = &rv_emu->rv_bus;
    /* I-type
     * 31 - 20 19 - 15 14 - 12 11 - 7 6 - 0
     * | imm  |  rs1  | func3 |  rd  | op |
     */
    uint8_t opcode = rv_cpu->opcode;
    uint8_t func3 = rv_cpu->func3;
    uint32_t imm = rv_cpu->imm;
    uint32_t *rd_reg = &rv_cpu->xreg[rv_cpu->rd];
    uint32_t *rs1_reg = &rv_cpu->xreg[rv_cpu->rs1];
    uint32_t *pc = &rv_cpu->pc;

    if (opcode == 0x03) {
        switch (func3) {
        case 0x0: // LB (Load Byte)
            *rd_reg = (uint8_t)rv_cpu->rv_read_bus(rv_bus, *rs1_reg + imm, 1);
            break;
        case 0x1: // LH (Load Half)
            *rd_reg = (uint16_t)read_bus(rv_bus, *rs1_reg + imm, 2);
            break;
        case 0x2: // LW (Load Word)
            *rd_reg = (uint32_t)read_bus(rv_bus, *rs1_reg + imm, 4);
            break;
        case 0x4: // LBU, Note : zero-extends
            *rd_reg = (uint8_t)rv_cpu->rv_read_bus(rv_bus, *rs1_reg + imm, 1);
            break;
        case 0x5: // LHU, Note : zero-extends
            *rd_reg = (uint16_t)read_bus(rv_bus, *rs1_reg + imm, 2);
            break;
        default:
            rv_cpu->exception_code = LOAD_ACCESS_FAULT;
        }

        return;
    }

    if (opcode == 0x13) {
        switch (func3) {
        case 0x0: // ADDI (ADD Imm)
            *rd_reg = *rs1_reg + imm;
            break;
        case 0x1: // SLLI (SLL Imm)
            *rd_reg = *rs1_reg << (imm & 0x1f);
            break;
        case 0x2: // SLTI (SLT Imm)
            *rd_reg = (*rs1_reg < imm) ? 1 : 0;
            break;
        case 0x3: // SLTIU (SLTI U), Note : zero-extends
            *rd_reg = (*rs1_reg < imm) ? 1 : 0;
            break;
        case 0x4: // XORI (XOR Imm)
            *rd_reg = *rs1_reg ^ imm;
            break;
        case 0x5:
            if (!(imm >> 5)) // SRLI (SRL Imm)
                *rd_reg = *rs1_reg >> (imm & 0x1f);
            else // SRAI (SRA Imm), Note : msb-extends
                *rd_reg = *rs1_reg >> (imm & 0x1f);
            break;
        case 0x6: // ORI (OR Imm)
            *rd_reg = *rs1_reg | imm;
            break;
        case 0x7: // ANDI (AND Imm)
            *rd_reg = *rs1_reg & imm;
            break;
        default:
            rv_cpu->exception_code = LOAD_ACCESS_FAULT;
        }
        
        return;
    } 

    if (opcode == 0x67) {
        switch (func3) {
        case 0x0: // JALR
            *rd_reg = *pc + 4;
            *pc = (*rs1_reg + imm);
            rv_cpu->branch_enable = 1;
            break;
        default:
            rv_cpu->exception_code = LOAD_ACCESS_FAULT;
        }

        return;
    }

    /* SYSTEM Instruction */
    uint32_t tmp;
    if (opcode == 0x73) {
        switch (func3) {
        case 0x0:
            if (imm == 0x302) { // MRET
                /* TODO */
                rv_cpu->pc = rv_cpu->csr[MEPC].value - 4;
                /* When kernel completed the Interrupt, Enable MIE */
                rv_cpu->csr[MSTATUS].value = (rv_cpu->csr[MSTATUS].value & ~MSTATUS_MIE) | 
                                             ((rv_emu->rv_cpu.csr[MSTATUS].value & MSTATUS_MPIE) >> 4);
                rv_emu->rv_cpu.csr[MSTATUS].value |= MSTATUS_MPIE;
            }
            break;
        case 0x1: // CSRRW (Read & Write)
            *rd_reg = rv_cpu->csr[imm & 0xfff].value;
            rv_cpu->csr[imm &0xfff].value = *rs1_reg;
            break;
        case 0x2: // CSRRS (Read & Set)
            tmp = rv_cpu->csr[imm & 0xfff].value;
            *rs1_reg = *rs1_reg | tmp;
            *rd_reg = tmp;
            break;
        case 0x3: // CSRRC (Read & Clean)
            *rd_reg = rv_cpu->csr[imm & 0xfff].value;
            *rs1_reg &= ~(*rd_reg);
            break;
#if 0
        case 0x4: // CSRRWI
            break;
        case 0x5: // CSRRSI
            break;
        case 0x6: // CSRRCI
            break;
#endif
        default:
            rv_cpu->exception_code = LOAD_ACCESS_FAULT;
        }

        return;
    }

}

static void execution_s_type_instr(rv_cpu_t *rv_cpu)
{
    rv_emu_t *rv_emu = container_of(rv_cpu, rv_emu_t, rv_cpu);
    rv_bus_t *rv_bus = &rv_emu->rv_bus;
    /* S-type
     *
     *
     */
    uint8_t func3 = rv_cpu->func3;
    uint32_t imm = rv_cpu->imm; 
    uint32_t *rs1_reg = &rv_cpu->xreg[rv_cpu->rs1];
    uint32_t *rs2_reg = &rv_cpu->xreg[rv_cpu->rs2];   

    switch (func3) {
    case 0x0: // SB
        rv_cpu->rv_write_bus(rv_bus, *rs1_reg + imm, *rs2_reg, 1);
        break;
    case 0x1: // SH
        rv_cpu->rv_write_bus(rv_bus, *rs1_reg + imm, *rs2_reg, 2);
        break;
    case 0x2: // SW
        rv_cpu->rv_write_bus(rv_bus, *rs1_reg + imm, *rs2_reg, 4);
        break;
    } 

}

static void execution_b_type_instr(rv_cpu_t *rv_cpu)
{
    uint8_t func3 = rv_cpu->func3;
    uint32_t imm = rv_cpu->imm; 
    uint32_t *rs1_reg = &rv_cpu->xreg[rv_cpu->rs1];
    uint32_t *rs2_reg = &rv_cpu->xreg[rv_cpu->rs2];   
    uint32_t *pc = &rv_cpu->pc;
    
    switch (func3) {
    case 0x0: // BEQ
        if (*rs1_reg == *rs2_reg) {
            *pc += imm; goto pc_sel;
        }
        break;
    case 0x1: // BNE
        if (*rs1_reg != *rs2_reg) {
            *pc += imm; goto pc_sel;
        }
        break;
    case 0x4: // BLT
        if ((int32_t)*rs1_reg < (int32_t)*rs2_reg) {
            *pc += imm; goto pc_sel;
        }
        break;
    case 0x5: // BGE
        if ((int32_t)*rs1_reg >= (int32_t)*rs2_reg) {
            *pc += imm; goto pc_sel;
        }
        break;
    case 0x6: // BLTU
        if ((int32_t)*rs1_reg < (int32_t)*rs2_reg) {
            *pc += imm; goto pc_sel;
        }
        break;
    case 0x7: // BGEU
        if ((int32_t)*rs1_reg >= (int32_t)*rs2_reg) {
            *pc += imm; goto pc_sel;
        }
        break;
    }

    return;
pc_sel:
    rv_cpu->branch_enable = 1;
}

static void execution_u_type_instr(rv_cpu_t *rv_cpu)
{
    uint8_t opcode = rv_cpu->opcode;
    uint32_t *rd_reg = &rv_cpu->xreg[rv_cpu->rd];
    uint32_t imm = rv_cpu->imm;

    switch (opcode) {
    case 0x17: // AUIPC
        *rd_reg = rv_cpu->pc + imm;
        break;
    case 0x37: // LUI
        *rd_reg = imm; 
        break;
    default:
        rv_cpu->exception_code = LOAD_ACCESS_FAULT;
    };
}

static void execution_j_type_instr(rv_cpu_t *rv_cpu)
{
    uint8_t opcode = rv_cpu->opcode;
    uint32_t *rd_reg = &rv_cpu->xreg[rv_cpu->rd];
    uint32_t imm = rv_cpu->imm; 
    uint32_t *pc = &rv_cpu->pc;

    /* PC Sel */
    rv_cpu->branch_enable = 1;

    switch (opcode) {
    case 0x6f: // JAL
        *rd_reg = *pc + 4;
        *pc += imm;
        break;
    default:
        rv_cpu->exception_code = LOAD_ACCESS_FAULT;
    };
}

uint32_t execution_rv_instr(rv_cpu_t *rv_cpu)
{
    switch (rv_cpu->opcode) {
    /* TODO : FENCE */
    case 0x0f:
        break; 
    case 0x33:
        execution_r_type_instr(rv_cpu);
        break;
    case 0x03:
    case 0x13:
    case 0x67:
    case 0x73:
        execution_i_type_instr(rv_cpu);
        break;
    case 0x23:
        execution_s_type_instr(rv_cpu);
        break;
    case 0x63:
        execution_b_type_instr(rv_cpu);
        break;
    case 0x17:
    case 0x37:
        execution_u_type_instr(rv_cpu);
        break;
    case 0x6f:
        execution_j_type_instr(rv_cpu);
        break;
    /* RV32A Atomic Extension */
    case 0x2f:
        execution_rv32a_type_instr(rv_cpu);
        break;
    default:
        rv_cpu->exception_code = LOAD_ACCESS_FAULT;
    }    
}

