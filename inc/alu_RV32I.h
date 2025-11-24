/************************************************************
 * Author: Mustafa Ergün
 * Project: CANON MCU
 * Submodule: ALU (RV32I)
 *
 * Description:
 *   The ALU performs arithmetic and logic operations on the
 *   CPU's data inputs and produces the appropriate outputs.
 ************************************************************/

#ifndef ALU_RV32I_H
#define ALU_RV32I_H

#include <systemc.h>
#include "alu_defs.h"


SC_MODULE(alu_RV32I) {
    // Inputs
    sc_in<sc_uint<32>> data_a_in;   // operand A (from register file)
    sc_in<sc_uint<32>> data_b_in;   // operand B (from register file)

    sc_in<sc_uint<4>>  alu_func_in; // ALU function from Decoder
    sc_in<sc_uint<1>> alu_src_in;  // ALU source select (0=rs2, 1=imm) from Decoder
    sc_in<sc_uint<32>>  imm_in;      // immediate from Decoder

    // Outputs
    sc_out<sc_uint<32>> result_out;   // ALU computation result
    sc_out<sc_uint<3>>  br_flags_out; // {eq, lt_s, lt_u} for branch decisions
    sc_out<sc_uint<32>> target_out;   // ALU result for PC target (branch/jump)

    void alu_process(void);

    SC_CTOR(alu_RV32I) {
        SC_METHOD(alu_process);
        sensitive << data_a_in << data_b_in << imm_in << alu_func_in << alu_src_in;
        dont_initialize();
    }
};

#endif // ALU_RV32I_H