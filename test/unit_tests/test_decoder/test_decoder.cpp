/*
 * test_control_unit.cpp
 *
 * Unit tests for the decoder module of the CANON RISC-V CPU project.

 * Author: Mustafa Ergün
 * Date: October 1, 2025
 */
#include <systemc.h>
#include "../../../inc/cpu/decoder_RV32I.h"
#include "../../../inc/cpu/alu_defs.h"

static void REQUIRE(bool c, const char* msg){ if(!c){ SC_REPORT_ERROR("TEST", msg); }}

int sc_main(int, char**) 
{
    sc_signal<sc_uint<32>> instr_in;
    sc_signal<sc_uint<5>> rs1, rs2, rd;
    sc_signal<sc_uint<6>> op_class;
    sc_signal<sc_uint<3>> memMode;
    sc_signal<sc_uint<4>> alu_func;
    sc_signal<sc_uint<1>> alu_src;
    sc_signal<sc_int<32>> imm_out;

    decoder_RV32I decoder("Decoder");
    decoder.instr_in(instr_in);
    decoder.rs1(rs1);
    decoder.rs2(rs2);
    decoder.rd(rd);
    decoder.op_class(op_class);
    decoder.memMode(memMode);
    decoder.alu_func(alu_func);
    decoder.alu_src(alu_src);
    decoder.imm_out(imm_out);

    // Test case 1: ADD instruction
    instr_in.write(0x00000033);  // ADD x0, x0, x0
    sc_start(1, SC_NS);
    REQUIRE(op_class.read() == OP_ALU, "ADD sets op_class");
    REQUIRE(memMode.read() == 0, "ADD sets memMode");
    REQUIRE(rd.read() == 0, "ADD writes rd");
    REQUIRE(alu_func.read() == 0b0000, "ADD sets ALU func");
    REQUIRE(alu_src.read() == 0, "ADD uses rs2");
    REQUIRE(imm_out.read() == 0, "ADD has no immediate");

    // Test case 2: SUB instruction
    instr_in.write(0x40000033);  // SUB x0, x0, x0
    sc_start(1, SC_NS);
    REQUIRE(op_class.read() == OP_ALU, "SUB sets op_class");
    REQUIRE(memMode.read() == 0, "SUB sets memMode");
    REQUIRE(rd.read() == 0, "SUB writes rd");
    REQUIRE(alu_func.read() == 0b0001, "SUB sets ALU func");
    REQUIRE(alu_src.read() == 0, "SUB uses rs2");
    REQUIRE(imm_out.read() == 0, "SUB has no immediate");

    // Test case 3: LW instruction
    instr_in.write(0x00002083);  // LW x1, 0(x0)
    sc_start(1, SC_NS);
    REQUIRE(rd.read() == 1, "LW writes rd");
    REQUIRE(op_class.read() == OP_LOAD, "LW sets op_class");
    REQUIRE(memMode.read() == 0b010, "LW sets memMode");
    REQUIRE(alu_func.read() == 0b0000, "LW sets ALU func");
    REQUIRE(alu_src.read() == 1, "LW uses immediate");
    REQUIRE(imm_out.read() == 0, "LW immediate is 0");

    // Test case 4: JAL instruction
    instr_in.write(0x0000006F);  // JAL x1, 0
    sc_start(1, SC_NS);
    REQUIRE(op_class.read() == OP_JAL, "JAL sets op_class");
    REQUIRE(memMode.read() == MEM_NONE, "JAL sets memMode");
    REQUIRE(rd.read() == 0, "JAL writes rd");
    REQUIRE(alu_func.read() == OP_ALU, "JAL sets ALU func");
    REQUIRE(alu_src.read() == 1, "JAL uses immediate");
    REQUIRE(imm_out.read() == 0, "JAL immediate is 0");

    // Test case 5: BEQ instruction
    instr_in.write(0x00000063);  // BEQ x0, x0, 0
    sc_start(1, SC_NS);
    REQUIRE(op_class.read() == OP_BRANCH, "BEQ sets op_class");
    REQUIRE(memMode.read() == MEM_NONE, "BEQ sets memMode");
    REQUIRE(rd.read() == 0, "BEQ writes rd");
    REQUIRE(alu_func.read() == ALU_SUB, "BEQ sets ALU func");
    REQUIRE(alu_src.read() == 1, "BEQ uses immediate");
    REQUIRE(imm_out.read() == 0, "BEQ immediate is 0");

    std::cout << "control_unit: ALL TESTS PASSED\n";
    return 0;
}