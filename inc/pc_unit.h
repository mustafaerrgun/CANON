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

enum PCOp : uint8_t { PC_PLUS4=0, PC_BRANCH=1, PC_JAL=2, PC_JALR=3 };

SC_MODULE(pc_unit) {
    // Inputs
    sc_in<sc_uint<2>>   pc_op_in;
    sc_in<sc_uint<32>>  boot_addr_in;      // used when reset is asserted
    sc_in<sc_uint<32>>  curr_pc_in;        // PC from top-level
    sc_in<sc_uint<32>>  branch_target_in;
    sc_in<sc_uint<32>>  jal_target_in;
    sc_in<sc_uint<32>>  jalr_target_in;
    sc_in<bool>         reset_n;

    // Outputs
    sc_out<sc_uint<32>> pc_out;            // next PC
    sc_out<sc_uint<32>> pc_plus4_out;      // curr_pc + 4

    void comb() {
        sc_uint<32> pc4 = curr_pc_in.read() + 4;
        sc_uint<32> npc;

        if (!reset_n.read()) {
            npc = boot_addr_in.read();
        } else {
            switch (pc_op_in.read()) {
                case PC_PLUS4:  npc = pc4;                            break;
                case PC_BRANCH: npc = branch_target_in.read();        break;
                case PC_JAL:    npc = jal_target_in.read();           break;
                case PC_JALR:   npc = (jalr_target_in.read() & 0xFFFFFFFEu); break;
                default:        npc = pc4;                            break;
            }
        }

        pc_out.write(npc);
        pc_plus4_out.write(pc4);
    }

    SC_CTOR(pc_unit) {
        SC_METHOD(comb);
        sensitive << reset_n
                  << boot_addr_in
                  << curr_pc_in
                  << pc_op_in
                  << branch_target_in
                  << jal_target_in
                  << jalr_target_in;
    }
};

#endif