/************************************************************
 * Author: Mustafa Ergün
 * Project: CANON MCU
 * Submodule: Decoder (RV32I)
 ************************************************************/

#include "../../../inc/cpu/decoder_RV32I.h"

// --------- small helpers ---------
static inline sc_int<32> sext(sc_int<32> x, int from_bits) {
    const int sh = 32 - from_bits;
    return (x << sh) >> sh;
}

void decoder_RV32I::decode_proc() {
    const sc_uint<32> inst = instr_in.read();

    // Common fields
    const sc_uint<7>  opcode = inst.range(6,0);
    const sc_uint<5>  rd_f   = inst.range(11,7);
    const sc_uint<3>  f3     = inst.range(14,12);
    const sc_uint<5>  rs1_f  = inst.range(19,15);
    const sc_uint<5>  rs2_f  = inst.range(24,20);
    const sc_uint<7>  f7     = inst.range(31,25);

    // Defaults (NOP)
    sc_uint<6>  opcls    = OP_ALU;       
    sc_uint<3>  mem_mode = 0;
    sc_uint<4>  alu      = ALU_ADD;
    sc_uint<1>  asrc     = 0;            // 0=rs2, 1=imm
    sc_int<32>  imm      = 0;

    sc_uint<5>  rd_w     = 0;
    sc_uint<5>  rs1_w    = 0;
    sc_uint<5>  rs2_w    = 0;

    if(opcode == OPCODE_LUI)
    {
        opcls = OP_LUI;
        rd_w  = rd_f;
        // imm[31:12] = inst[31:12], low 12 are zero in datapath; sign irrelevant
        imm   = (sc_int<32>)((inst & 0xFFFFF000u));  // already aligned
        alu   = ALU_ADD;  // datapath does mux for LUI path; ALU add with zero is fine
        asrc  = 1;
    }
    else if(opcode == OPCODE_AUIPC)
    {
        opcls = OP_AUIPC;
        rd_w  = rd_f;
        imm   = (sc_int<32>)((inst & 0xFFFFF000u));
        alu   = ALU_ADD;  // PC + imm handled in datapath
        asrc  = 1;
    }
    else if(opcode == OPCODE_JAL)
    {
        opcls = OP_JAL;
        rd_w  = rd_f;
        // J imm: [20|10:1|11|19:12] << 1
        sc_int<32> raw =
            ( (sc_int<32>)(inst[31])       << 20 ) |
            ( (sc_int<32>)(inst.range(19,12)) << 12 ) |
            ( (sc_int<32>)(inst[20])       << 11 ) |
            ( (sc_int<32>)(inst.range(30,21)) << 1 );
        imm  = sext(raw, 21); // 21-bit signed, then <<1 already encoded above
        alu  = ALU_ADD; // not used for target calc; WB uses PC+4 via control unit
        asrc = 1;
    }
    else if(opcode == OPCODE_JALR)
    {
        opcls = OP_JALR;
        rd_w  = rd_f;
        rs1_w = rs1_f;
        // I imm: [31:20]
        imm   = sext((sc_int<32>)inst.range(31,20), 12);
        alu   = ALU_ADD; // base + imm, target computed in datapath
        asrc  = 1;
    }
    else if(opcode == OPCODE_LOAD)
    {
        opcls    = OP_LOAD;
        rd_w     = rd_f;
        rs1_w    = rs1_f;
        imm      = sext((sc_int<32>)inst.range(31,20), 12);
        mem_mode = f3;  // size/unsigned info for control/memory
        alu      = ALU_ADD; // address = rs1 + imm
        asrc     = 1;
    }
    else if(opcode == OPCODE_OPIMM) // ALU with immediate
    {
        opcls = OP_ALU;
        rd_w  = rd_f;
        rs1_w = rs1_f;
        imm   = sext((sc_int<32>)inst.range(31,20), 12);
        asrc  = 1;

        switch (f3) {
            case 0b000: alu = ALU_ADD;  break;                 // ADDI
            case 0b010: alu = ALU_SLT;  break;                 // SLTI
            case 0b011: alu = ALU_SLTU; break;                 // SLTIU
            case 0b100: alu = ALU_XOR;  break;                 // XORI
            case 0b110: alu = ALU_OR;   break;                 // ORI
            case 0b111: alu = ALU_AND;  break;                 // ANDI
            case 0b001: alu = ALU_SLL;  imm = inst.range(24,20); break; // SLLI (shamt)
            case 0b101:
                if (f7 == 0b0000000)      { alu = ALU_SRL; imm = inst.range(24,20); } // SRLI
                else /*0100000*/          { alu = ALU_SRA; imm = inst.range(24,20); } // SRAI
                break;
            default:   alu = ALU_INVALID; break;
        }
    }
    else if(opcode == OPCODE_BRANCH)
    {
        opcls = OP_BRANCH;
        rs1_w = rs1_f;
        rs2_w = rs2_f;
        // B imm: [12|10:5|4:1|11] << 1
        sc_int<32> raw =
            ( (sc_int<32>)(inst[31])        << 12 ) |
            ( (sc_int<32>)(inst.range(30,25)) << 5 ) |
            ( (sc_int<32>)(inst.range(11,8))  << 1 ) |
            ( (sc_int<32>)(inst[7])         << 11 );
        imm      = sext(raw, 13);
        mem_mode = f3; // pass to control for branch subtype
        alu      = ALU_SUB; // many datapaths use subtract/compare;
        asrc     = 1;
    }
    else if(opcode == OPCODE_STORE)
    {
        opcls    = OP_STORE;
        rs1_w    = rs1_f;
        rs2_w    = rs2_f;
        // S imm: [31:25|11:7]
        sc_int<32> raw =
            ( (sc_int<32>)(inst.range(31,25)) << 5 ) |
            ( (sc_int<32>)(inst.range(11,7)) );
        imm      = sext(raw, 12);
        mem_mode = (f3 & 0b111); // 000 SB,001 SH,010 SW
        alu      = ALU_ADD;      // address = rs1 + imm
        asrc     = 1;
    }
    else if(opcode == OPCODE_OP)
    {
        opcls = OP_ALU;
        rd_w  = rd_f;
        rs1_w = rs1_f;
        rs2_w = rs2_f;
        asrc  = 0;  // rs2

        switch (f3) {
            case 0b000: alu = (f7 == 0b0100000) ? ALU_SUB : ALU_ADD; break; // SUB/ADD
            case 0b001: alu = ALU_SLL;  break;
            case 0b010: alu = ALU_SLT;  break;
            case 0b011: alu = ALU_SLTU; break;
            case 0b100: alu = ALU_XOR;  break;
            case 0b101: alu = (f7 == 0b0100000) ? ALU_SRA : ALU_SRL; break;
            case 0b110: alu = ALU_OR;   break;
            case 0b111: alu = ALU_AND;  break;
            default:    alu = ALU_INVALID; break;
        }
    }
    else if(opcode == OPCODE_SYSTEM || opcode == OPCODE_FENCE)
    {
        // Keep defaults: no reg write, no mem op, imm=0
        opcls = OP_ALU;
        alu   = ALU_ADD;
        asrc  = 0;
    }
    else
    {
        // Unknown opcode - treat as NOP for safety
        opcls = OP_ALU;
        alu   = ALU_ADD;
    }

    // Drive outputs
    rs1.write(rs1_w);
    rs2.write(rs2_w);
    rd.write(rd_w);

    op_class.write(opcls);
    memMode.write(mem_mode);

    alu_func.write(alu);
    alu_src.write(asrc);
    imm_out.write(imm);
}
