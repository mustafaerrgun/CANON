/************************************************************
 * Author: Mustafa Ergün
 * Project: CANON MCU
 * Submodule: PC Unit
 *
 * Description:
 *   RV32I Program Counter unit. Holds current PC and
 *   computes the next PC based on control (PC+4 / BRANCH / JAL / JALR).
 *   Purely combinational interface; no clock here (LT VP style).
 ************************************************************/

#ifndef PC_UNIT_H
#define PC_UNIT_H

#include <systemc.h>

SC_MODULE(pc_unit) {
    // ==== Inputs ====
    sc_in<bool>         reset_n;            // Active-low reset from top
    sc_in<sc_uint<32>>  boot_addr_in;       // Boot/reset address (e.g., 0x00000000)

    sc_in<sc_uint<32>>  pc_curr_in;         // Current PC from core state

    sc_in<sc_uint<2>>   pc_op_in;           // 00:PC+4, 01:BRANCH, 10:JAL, 11:JALR

    // Candidate targets computed by ALU/Branch unit
    sc_in<sc_uint<32>>  branch_target_in;   // PC + imm (conditional branches)
    sc_in<sc_uint<32>>  jal_target_in;      // PC + imm (JAL)
    sc_in<sc_uint<32>>  jalr_target_in;     // (rs1 + imm) & ~1 (JALR)

    // ==== Outputs ====
    sc_out<sc_uint<32>> pc_out;             // Current PC (to fetch/decoder)
    sc_out<sc_uint<32>> npc_out;            // Selected next PC
    sc_out<sc_uint<32>> pc_plus4_out;       // PC + 4 (for WB on JAL/JALR)

    // Constructor (behavior to be added later)
    SC_CTOR(pc_unit) { /* SC_METHOD(comb) */ }
};

#endif // PC_UNIT_H
