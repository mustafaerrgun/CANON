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
const sc_uint<4>    ALU_ADD = 0;
const sc_uint<4>    ALU_SUB = 1;
const sc_uint<4>    ALU_AND = 2;
const sc_uint<4>    ALU_OR  = 3;
const sc_uint<4>    ALU_XOR = 4;
const sc_uint<4>    ALU_SLT = 5;     // signed less-than
const sc_uint<4>    ALU_SLTU = 6;    // unsigned less-than
const sc_uint<4>    ALU_SLL = 7;     // shift left logical
const sc_uint<4>    ALU_SRL = 8;     // shift right logical
const sc_uint<4>    ALU_SRA = 9;     // shift right arithmetic
const sc_uint<4>    ALU_INVALID = 15;


// Branch flag bit positions: {eq, lt_s, lt_u}
// Matches control_unit expectation: br_flags_in[0]=eq, [1]=lt_s, [2]=lt_u
enum BRFlagIdx : int {
    BR_EQ  = 0,
    BR_LT_S = 1,
    BR_LT_U = 2
};

#endif // ALU_DEFS_H
