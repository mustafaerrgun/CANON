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

// ---- Operation class tags ----
const sc_uint<6>  OP_ALU    = 0x00;
const sc_uint<6>  OP_LOAD   = 0x08;
const sc_uint<6>  OP_STORE  = 0x18;
const sc_uint<6>  OP_BRANCH = 0x10;
const sc_uint<6>  OP_JAL    = 0x11;
const sc_uint<6>  OP_JALR   = 0x21;
const sc_uint<6>  OP_LUI    = 0x30;
const sc_uint<6>  OP_AUIPC  = 0x31;

// ---- PC operation select ----
const sc_uint<2> PC_PLUS4  = 0;
const sc_uint<2> PC_BRANCH = 1;
const sc_uint<2> PC_JAL    = 2;
const sc_uint<2> PC_JALR   = 3;

// ---- Memory operation ----
const sc_uint<2> MEM_NONE  = 0;
const sc_uint<2> MEM_LOAD  = 1;
const sc_uint<2> MEM_STORE = 2;

// ---- Write-back source select ----
const sc_uint<2> WB_ALU  = 0;
const sc_uint<2> WB_LOAD = 1;
const sc_uint<2> WB_PC4  = 2;

SC_MODULE(control_unit) {
    // Inputs
    sc_in<sc_uint<6>>  alu_op_in;    // decoded ALU op 
    sc_in<sc_uint<3>> funct3_in;     // Memory mode

    sc_in<sc_uint<3>>  br_flags_in;  // {eq, lt_s, lt_u} from ALU

    // Outputs
    sc_out<sc_uint<2>> pc_op_out;    // PC+4, BRANCH, JAL, JALR

    sc_out<bool>       reg_we_out;   // write enable for rd to Register Unit

    sc_out<sc_uint<2>> wb_sel_out;   // 00=ALU, 01=LOAD, 10=PC+4 to WB Mux

    sc_out<sc_uint<2>> mem_op_out;   // 00=NONE, 01=LOAD, 10=STORE to Memory
    sc_out<sc_uint<3>> mem_mode_out; // 000=LB, 001=LH, 010=LW, 100=LBU, 101=LHU to Memory

    // Combinational process
    void comb();

    SC_CTOR(control_unit) {
        SC_METHOD(comb);
        sensitive << alu_op_in;
        sensitive << br_flags_in;
        sensitive << funct3_in;
    }
};

#endif // CONTROL_H
