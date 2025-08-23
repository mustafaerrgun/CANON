/************************************************************
 * Author: Mustafa Ergün
 * Project: CANON MCU
 * Submodule: ALU (RV32I)
 *
 * Description:
 *   Combinational ALU implementing RV32I arithmetic/logic ops
 *   and producing branch compare flags {eq, lt_s, lt_u}.
 ************************************************************/

#include "alu_RV32I.h"


void alu_RV32I::alu_process(void) {
    // Read inputs
    const sc_uint<32> a      = data_a_in.read();          // rs1
    const sc_uint<32> b_rs2  = data_b_in.read();          // rs2 (for compares)
    const sc_uint<32> b_imm  = (sc_uint<32>) imm_in.read(); // imm as unsigned bits
    const bool        use_imm = alu_src_in.read();
    const sc_uint<4>  func    = alu_func_in.read();

    // Operand B selection for ALU datapath
    const sc_uint<32> b = use_imm ? b_imm : b_rs2;

    // Shift amount: RV32 uses only lower 5 bits
    const sc_uint<5> shamt = b.range(4, 0);

    // ---- Compute branch flags from rs1 vs rs2 (not affected by alu_src) ----
    sc_uint<3> flags = 0;
    const bool eq   = (a == b_rs2);
    const bool lt_s = ( (sc_int<32>)a < (sc_int<32>)b_rs2 );
    const bool lt_u = ( a < b_rs2 );

    flags[0] = eq;    // BR_EQ
    flags[1] = lt_s;  // BR_LT_S
    flags[2] = lt_u;  // BR_LT_U

    // ---- ALU core ----
    sc_uint<32> res = 0;

    switch (func) {
        case ALU_ADD:  res = a + b; break;
        case ALU_SUB:  res = a - b; break;

        case ALU_AND:  res = a & b; break;
        case ALU_OR:   res = a | b; break;
        case ALU_XOR:  res = a ^ b; break;

        case ALU_SLT:  res = ((sc_int<32>)a < (sc_int<32>)b) ? 1u : 0u; break;
        case ALU_SLTU: res = (a < b) ? 1u : 0u; break;

        case ALU_SLL:  res = (sc_uint<32>)(a << shamt); break;
        case ALU_SRL:  res = (sc_uint<32>)(a >> shamt); break;
        case ALU_SRA:  res = (sc_uint<32>)(((sc_int<32>)a) >> shamt); break;

        default:       res = 0; break; // ALU_INVALID 
    }

    // ---- Drive outputs ----
    result_out.write(res);
    br_flags_out.write(flags);
}
