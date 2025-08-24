/************************************************************
 * Author: Mustafa Ergün
 * Project: CANON MCU
 * File: alu_defs.h
 *
 * Purpose:
 *   Shared ALU definitions used by both the decoder and ALU.
 ************************************************************/

#ifndef ALU_DEFS_H
#define ALU_DEFS_H

#include <systemc.h>

// XLEN 
static constexpr int XLEN = 32;

// ALU function select (4-bit) — RV32I base ops
enum ALUFunc : sc_uint<4> {
    ALU_ADD = 0,
    ALU_SUB,
    ALU_AND,
    ALU_OR,
    ALU_XOR,
    ALU_SLT,     // signed less-than
    ALU_SLTU,    // unsigned less-than
    ALU_SLL,     // shift left logical
    ALU_SRL,     // shift right logical
    ALU_SRA,     // shift right arithmetic
    ALU_INVALID = 15
};

// Branch flag bit positions: {eq, lt_s, lt_u}
// Matches control_unit expectation: br_flags_in[0]=eq, [1]=lt_s, [2]=lt_u
enum BRFlagIdx : int {
    BR_EQ  = 0,
    BR_LT_S = 1,
    BR_LT_U = 2
};

#endif // ALU_DEFS_H
