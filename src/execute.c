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

    switch (func3) {
    case 0x0: 
        if (!func7) // ADD
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
        if (!func7) // SRL (Shift Right Logical) 
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
                *rd_reg = read_bus(rv_bus, *rs1_reg + imm, 1);
                break;
            case 0x5: // LHU, Note : zero-extends
                *rd_reg = read_bus(rv_bus, *rs1_reg + imm, 2);
                break;
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
        }
        
        return;
    } 

    if (opcode == 0x67) {
        switch (func3) {
        case 0x0: // JARL
            /* TODO
             * x0 can't be modified.
             * Maybe need good access mechanism
             */
            if (!(rv_cpu->rd == 0))
                *rd_reg = *pc + 4; 
            *pc = (*rs1_reg + imm - 4); // TODO
            break;
        }

        return;
    }

    /* SYSTEM Instruction */
    uint32_t tmp;
    if (opcode == 0x73) {
        switch (func3) {
        case 0x0:
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
        case 0x4: // CSRRWI
            break;
        case 0x5: // CSRRSI
            break;
        case 0x6: // CSRRCI
            break;
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
    uint32_t imm = rv_cpu->imm - 4; // TODO 
    uint32_t *rs1_reg = &rv_cpu->xreg[rv_cpu->rs1];
    uint32_t *rs2_reg = &rv_cpu->xreg[rv_cpu->rs2];   
    uint32_t *pc = &rv_cpu->pc;
    
    switch (func3) {
    case 0x0: // BEQ
        if (*rs1_reg == *rs2_reg)
            *pc += imm;
        break;
    case 0x1: // BNE
        if (*rs1_reg != *rs2_reg)
            *pc += imm;
        break;
    case 0x4: // BLT
        if (*rs1_reg < *rs2_reg)
            *pc += imm;
        break;
    case 0x5: // BGE
        if (*rs1_reg >= *rs2_reg)
            *pc += imm;
        break;
    case 0x6: // BLTU
        if (*rs1_reg < *rs2_reg)
            *pc += imm;
        break;
    case 0x7: // BGEU
        if (*rs1_reg >= *rs2_reg)
            *pc += imm;
        break;
    }

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
    };
}

static void execution_j_type_instr(rv_cpu_t *rv_cpu)
{
    uint8_t opcode = rv_cpu->opcode;
    uint32_t *rd_reg = &rv_cpu->xreg[rv_cpu->rd];
    uint32_t imm = rv_cpu->imm; 
    uint32_t *pc = &rv_cpu->pc;

    switch (opcode) {
    case 0x6f: // JAL
        /* TODO
         * x0 can't be modified.
         * Maybe need good access mechanism
         */
        if (!(rv_cpu->rd == 0))
            *rd_reg = *pc + 4; 
        /* TODO
         * Because next fetch will cause PC 
         * be added 4, so need to sub 4 before next fetch
         */
        *pc += (imm - 4); // TODO
        break;
    };
}


uint32_t execution_rv_instr(rv_cpu_t *rv_cpu)
{
    switch (rv_cpu->opcode) {
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
    default:
        rv_cpu->exception_code = LOAD_ACCESS_FAULT;
    }    
}

