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

enum : uint8_t {
  ALU_ADD = 0,
  ALU_SUB = 1,
  ALU_AND = 2,
  ALU_OR  = 3,
  ALU_XOR = 4,
  ALU_SLT = 5,
  ALU_SLTU= 6,
  ALU_SLL = 7,
  ALU_SRL = 8,
  ALU_SRA = 9,
  ALU_INVALID = 15
};

#endif // ALU_DEFS_H
