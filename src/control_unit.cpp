/************************************************************
 * Author: Mustafa Ergün
 * Project: CANON MCU
 * Submodule: Control Unit 
 ************************************************************/

#include "control_unit.h"

void control_unit::comb() {
    // ---- Safe defaults (NOP) ----
    sc_uint<2> pc_op    = PC_PLUS4;
    sc_uint<2> mem_op   = MEM_NONE;
    sc_uint<3> mem_mode = 0;          
    bool       reg_we   = false;
    sc_uint<2> wb_sel   = WB_ALU;


    // Unpack inputs
    sc_uint<6>  op = alu_op_in.read();
    const sc_uint<3> f3  = funct3_in.read();
    const bool eq   = br_flags_in.read()[0];
    const bool lt_s = br_flags_in.read()[1];
    const bool lt_u = br_flags_in.read()[2];

    if(op == OP_ALU)
    {
            // R/I ALU operations write result to rd
            reg_we = true;
            wb_sel = WB_ALU;
    }
    else if(op == OP_LOAD)
    {
            // Loads: mem_mode == funct3 (RV32I encoding)
            mem_op   = MEM_LOAD;
            mem_mode = f3;            // 000 LB,001 LH,010 LW,100 LBU,101 LHU
            reg_we   = true;
            wb_sel   = WB_LOAD;
    }
    else if(op == OP_STORE)
    {
            // Stores: only size matters (000 SB,001 SH,010 SW)
            mem_op   = MEM_STORE;
            mem_mode = (f3 & 0b011);  // mask to 000/001/010
            reg_we   = false;
    }
    else if(op == OP_BRANCH)
    {
            // Branch subtype via funct3
            // 000 BEQ, 001 BNE, 100 BLT, 101 BGE, 110 BLTU, 111 BGEU
            bool take = false;
            switch (f3) {
                case 0b000: take =  eq;   break; // BEQ
                case 0b001: take = !eq;   break; // BNE
                case 0b100: take =  lt_s; break; // BLT
                case 0b101: take = !lt_s; break; // BGE
                case 0b110: take =  lt_u; break; // BLTU
                case 0b111: take = !lt_u; break; // BGEU
                default:    take = false; break;
            }
            if (take) pc_op = PC_BRANCH;
            // Branches do not write rd
            reg_we = false;
    }
    else if(op == OP_JAL)
    {
            pc_op  = PC_JAL;
            reg_we = true;            // rd = PC+4
            wb_sel = WB_PC4;
    }
    else if(op == OP_JALR)
    {
            pc_op  = PC_JALR;
            reg_we = true;            // rd = PC+4
            wb_sel = WB_PC4;
    }
    else if(op == OP_LUI)
    {
            // rd = imm << 12 
            reg_we = true;
            wb_sel = WB_ALU;
    }
    else if(op == OP_AUIPC)
    {
            // rd = PC + imm 
            reg_we = true;
            wb_sel = WB_ALU;
    }

    // ---- Drive outputs ----
    pc_op_out.write(pc_op);
    mem_op_out.write(mem_op);
    mem_mode_out.write(mem_mode);
    reg_we_out.write(reg_we);
    wb_sel_out.write(wb_sel);
}