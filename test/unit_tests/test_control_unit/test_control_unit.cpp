/*
 * test_control_unit.cpp
 *
 * Unit tests for the control_unit module of the CANON RISC-V CPU project.
 * Uses SystemC to instantiate the control_unit and verify its outputs
 * for various instruction types (ALU, LOAD, STORE, BRANCH, JAL/JALR, LUI/AUIPC).
 * Each test sets input signals and checks the correctness of control signals
 * such as writeback selection, memory operation, and program counter operation.
 *
 * Author: Mustafa Ergün
 * Date: August 28, 2025
 */
#include <systemc.h>
#include "../../../inc/cpu/control_unit.h"


static void REQUIRE(bool c, const char* msg){ if(!c){ SC_REPORT_ERROR("TEST", msg); }}

int sc_main(int, char**) {
    // Signals
    sc_signal<sc_uint<5>>  alu_op_s;
    sc_signal<sc_uint<3>>  br_flags_s;   // {eq, lt_s, lt_u}
    sc_signal<sc_uint<3>>  funct3_s;

    sc_signal<sc_uint<2>> pc_op_s, mem_op_s, wb_sel_s;
    sc_signal<sc_uint<3>> mem_mode_s;
    sc_signal<bool>       reg_we_s;

    // DUT
    control_unit dut("control_unit");
    dut.alu_op_in(alu_op_s);
    dut.br_flags_in(br_flags_s);
    dut.funct3_in(funct3_s);        // after you add it
    dut.pc_op_out(pc_op_s);
    dut.mem_op_out(mem_op_s);
    dut.mem_mode_out(mem_mode_s);
    dut.reg_we_out(reg_we_s);
    dut.wb_sel_out(wb_sel_s);

    // // ---- Test 1: ALU class ----
    alu_op_s.write(OP_ALU); 
    br_flags_s.write(0); 
    funct3_s.write(0);
    sc_start(1, SC_NS);
    REQUIRE(reg_we_s.read() == true,  "ALU should write rd");
    REQUIRE(wb_sel_s.read() == WB_ALU,"ALU wb source");

    // ---- Test 2: LOAD ----
    alu_op_s.write(OP_LOAD); 
    br_flags_s.write(0); 
    funct3_s.write(0b010); 
    sc_start(1, SC_NS);
    REQUIRE(mem_op_s.read() == MEM_LOAD, "LOAD mem_op");
    REQUIRE(mem_mode_s.read() == 0b010,  "LOAD size LW"); 
    REQUIRE(wb_sel_s.read() == WB_LOAD,  "LOAD wb source");

    // ---- Test 3: STORE ----
    alu_op_s.write(OP_STORE); 
    funct3_s.write(0b010); // SH
    sc_start(1, SC_NS);
    REQUIRE(mem_op_s.read() == MEM_STORE,"STORE mem_op");
    REQUIRE(reg_we_s.read() == false,     "STORE no rd write");

    // ---- Test 4: BRANCH (BEQ taken) ----
    alu_op_s.write(OP_BRANCH); 
    funct3_s.write(0b000); // BEQ
    br_flags_s.write(0b001);  // eq=1, lt_s=0, lt_u=0
    sc_start(1, SC_NS);
    REQUIRE(pc_op_s.read() == PC_BRANCH,  "BEQ taken");

    // ---- Test 5: JAL/JALR ----
    alu_op_s.write(OP_JAL); 
    sc_start(1, SC_NS);
    REQUIRE(pc_op_s.read() == PC_JAL, "JAL pc_op"); 
    REQUIRE(wb_sel_s.read() == WB_PC4,"JAL wb=PC+4");

    alu_op_s.write(OP_JALR); 
    sc_start(1, SC_NS);
    REQUIRE(pc_op_s.read() == PC_JALR, "JALR pc_op");

    // ---- Test 6: LUI/AUIPC (writeback via ALU path) ----
    alu_op_s.write(OP_LUI); 
    sc_start(1, SC_NS);
    REQUIRE(reg_we_s.read() == true, "LUI writes rd");

    alu_op_s.write(OP_AUIPC); 
    sc_start(1, SC_NS);
    REQUIRE(reg_we_s.read() == true, "AUIPC writes rd");

    std::cout << "control_unit: ALL TESTS PASSED\n";
    return 0;
}

