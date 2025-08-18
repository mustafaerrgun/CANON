/************************************************************
 * Author: Mustafa Ergün
 * Project: CANON MCU
 * Submodule: Control Unit
 *
 * Description:
 *   The Control Unit generates control signals for the CPU
 *   based on instruction decode info and ALU status flags.
 ************************************************************/

#ifndef CONTROL_H
#define CONTROL_H

#include <systemc.h>

SC_MODULE(control_unit) {
    // Inputs
    sc_in<sc_uint<5>>  alu_op_in;    // decoded ALU op (room for future ops)
    sc_in<sc_uint<3>>  br_flags_in;  // {eq, lt_s, lt_u} from ALU

    // Outputs
    sc_out<sc_uint<2>> pc_op_out;    // PC+4, BRANCH, JAL, JALR
    sc_out<sc_uint<2>> mem_op_out;   // 00=NONE, 01=LOAD, 10=STORE
    sc_out<sc_uint<2>> mem_size_out; // 00=B, 01=H, 10=W
    sc_out<bool>       mem_sign_out; // 1=signed load, 0=unsigned
    sc_out<bool>       reg_we_out;   // write enable for rd
    sc_out<sc_uint<2>> wb_sel_out;   // 00=ALU, 01=LOAD, 10=PC+4

    SC_CTOR(control_unit) { /* add SC_METHOD later */ }
};

#endif // CONTROL_H
