/*
 * test_integration_1.cpp
 *
 * Integration tests for the CANON RISC-V CPU project.
 * Using Decoder, Register File, ALU and Control Unit modules.

 * Author: Mustafa Ergün
 * Date: October 3, 2025
 */

#include <systemc.h>
#include "decoder_RV32I.h"
#include "register_unit.h"
#include "alu_RV32I.h"
#include "alu_defs.h"
#include "control_unit.h"

static void REQUIRE(bool c, const char* msg){ if(!c){ SC_REPORT_ERROR("TEST", msg); }}

int sc_main(int, char**)
{
    // Signals of Decoder
    sc_signal<sc_uint<32>> instruction;
    sc_signal<sc_uint<5>> rs1, rs2, rd;
    sc_signal<sc_uint<6>> op_class;
    sc_signal<sc_uint<3>> funct3;
    sc_signal<sc_uint<4>> alu_func;
    sc_signal<sc_uint<1>> alu_src;
    sc_signal<sc_uint<32>> imm;

    // Signals of Register File
    sc_signal<bool> we;
    sc_signal<sc_uint<32>> wd;
    sc_signal<sc_uint<32>> data_a, data_b;
    
    // Signals of Control Unit
    sc_signal<sc_uint<2>> pc_op;
    sc_signal<bool> reg_we;
    sc_signal<sc_uint<2>> wb_sel, mem_op;
    sc_signal<sc_uint<3>> mem_mode;

    // Signals of ALU
    sc_signal<sc_uint<32>> result, target;
    sc_signal<sc_uint<3>> br_flags;

    // Modules
    decoder_RV32I decoder("Decoder");
    register_unit reg_file("RegisterFile");
    alu_RV32I alu("ALU");
    control_unit ctrl_unit("ControlUnit");

    // Connections
    decoder.instr_in(instruction);
    decoder.rs1(rs1);
    decoder.rs2(rs2);
    decoder.rd(rd);
    decoder.op_class(op_class);
    decoder.memMode(funct3);
    decoder.alu_func(alu_func);
    decoder.alu_src(alu_src);
    decoder.imm_out(imm);

    reg_file.rs1_addr_in(rs1);
    reg_file.rs2_addr_in(rs2);
    reg_file.rd_addr_in(rd);
    reg_file.we_in(reg_we);
    reg_file.wd_in(wd);
    reg_file.data_a_out(data_a);
    reg_file.data_b_out(data_b);

    ctrl_unit.alu_op_in(op_class);
    ctrl_unit.funct3_in(funct3);
    ctrl_unit.br_flags_in(br_flags);
    ctrl_unit.pc_op_out(pc_op);
    ctrl_unit.reg_we_out(reg_we);
    ctrl_unit.wb_sel_out(wb_sel);
    ctrl_unit.mem_op_out(mem_op);
    ctrl_unit.mem_mode_out(mem_mode);

    alu.data_a_in(data_a);
    alu.data_b_in(data_b);
    alu.alu_func_in(alu_func);
    alu.alu_src_in(alu_src);
    alu.imm_in(imm);
    alu.result_out(result);
    alu.br_flags_out(br_flags);
    alu.target_out(target);


    // Test Case: ADD operation (R-type)
    // Initialization Register File
    reg_we.write(true); // Enable write for testing
    rd.write(1); // Write to register x1
    wd.write(15); // Write value 15 to x1
    sc_start(1, SC_NS); // Run simulation for 1 ns to write value
    instruction.write(0b00000000000100000000000010110011); // ADD x1, x0, x1
    sc_start(1, SC_NS); // Run simulation for 1 ns
    REQUIRE(result.read() == 15, "ALU result should be 15 for ADD operation");

    cout << "All test cases passed." << endl;

    return 0;
}