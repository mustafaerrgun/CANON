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
    // Inputs
    sc_in<sc_uint<2>>  pc_op_in;        // select (PC+4, BRANCH, JAL, JALR)
    sc_in<sc_uint<32>> boot_addr_in;    // reset vector
    sc_in<sc_uint<32>> branch_target_in;
    sc_in<sc_uint<32>> jal_target_in;
    sc_in<sc_uint<32>> jalr_target_in;

    // Outputs
    sc_out<sc_uint<32>> pc_out;       // current PC (to memory)
    sc_out<sc_uint<32>> pc_plus4_out; // for WB on JAL/JALR

    // Internal PC register
    sc_signal<sc_uint<32>> pc_reg;

    // Next PC calculation
    void comb() {
        sc_uint<32> pc  = pc_reg.read();
        sc_uint<32> pc4 = pc + 4;
        sc_uint<32> npc = pc4;

        switch (pc_op_in.read()) {
            case 0: npc = pc4; break;
            case 1: npc = branch_target_in.read(); break;
            case 2: npc = jal_target_in.read(); break;
            case 3: npc = jalr_target_in.read(); break;
        }

        pc_out.write(pc);
        pc_plus4_out.write(pc4);
        next_pc = npc;
    }

    // Sequential update of PC register
    void seq() {
        if (!reset_n.read())
            pc_reg.write(boot_addr_in.read());
        else
            pc_reg.write(next_pc);
    }

    sc_uint<32> next_pc; // temp

    SC_CTOR(pc_unit) {
        SC_METHOD(comb);
        sensitive << pc_reg << pc_op_in << branch_target_in << jal_target_in << jalr_target_in;

        SC_METHOD(seq);
        sensitive << clk.pos();
    }
};
