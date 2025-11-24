/*
 * test_alu_RV32I.cpp
 *
 * Unit tests for the ALU unit of the CANON RISC-V CPU project.

 * Author: Mustafa Ergün
 * Date: October 3, 2025
 */

#include <systemc.h>
#include "alu_RV32I.h"

static void REQUIRE(bool c, const char* msg){ if(!c){ SC_REPORT_ERROR("TEST", msg); }}


int sc_main(int, char**)
{
    // Input signals of ALU
    sc_signal<sc_uint<32>> data_a_sig;
    sc_signal<sc_uint<32>> data_b_sig;
    sc_signal<sc_uint<4>>  alu_func_sig;
    sc_signal<sc_uint<1>>  alu_src_sig;
    sc_signal<sc_uint<32>>  imm_sig;

    // Output signals of ALU
    sc_signal<sc_uint<32>> result_sig;
    sc_signal<sc_uint<3>>  br_flags_sig;
    sc_signal<sc_uint<32>> target_sig;

    alu_RV32I alu("ALU_RV32I");
    alu.data_a_in(data_a_sig);
    alu.data_b_in(data_b_sig);
    alu.alu_func_in(alu_func_sig);
    alu.alu_src_in(alu_src_sig);
    alu.imm_in(imm_sig);
    alu.result_out(result_sig);
    alu.br_flags_out(br_flags_sig);
    alu.target_out(target_sig);

    // Test Case #1: Arithmetic Operations
    data_a_sig.write(10);
    data_b_sig.write(20);
    alu_func_sig.write(ALU_ADD);
    alu_src_sig.write(0); // Use rs2
    imm_sig.write(0);
    sc_start(1, SC_NS);
    REQUIRE(result_sig.read() == 30, "10 + 20 should be 30");
    REQUIRE(br_flags_sig.read()[0] == 0, "BR_EQ should be 0");
    REQUIRE(br_flags_sig.read()[1] == 1, "BR_LT_S should be 1");
    REQUIRE(br_flags_sig.read()[2] == 1, "BR_LT_U should be 1");
    sc_start(1, SC_NS);
    data_a_sig.write(50);
    data_b_sig.write(20);
    alu_func_sig.write(ALU_SUB);
    alu_src_sig.write(0); // Use rs2
    imm_sig.write(0);
    sc_start(1, SC_NS);
    REQUIRE(result_sig.read() == 30, "50 - 20 should be 30");
    REQUIRE(br_flags_sig.read()[0] == 0, "BR_EQ should be 0");
    REQUIRE(br_flags_sig.read()[1] == 0, "BR_LT_S should be 0");
    REQUIRE(br_flags_sig.read()[2] == 0, "BR_LT_U should be 0");

    // Test Case #2: Logical Operations
    data_a_sig.write(0b1100);
    data_b_sig.write(0b1010);
    alu_func_sig.write(ALU_AND);
    alu_src_sig.write(0); // Use rs2
    imm_sig.write(0);
    sc_start(1, SC_NS);
    REQUIRE(result_sig.read() == 0b1000, "0b1100 & 0b1010 should be 0b1000");
    alu_func_sig.write(ALU_OR);
    sc_start(1, SC_NS);
    REQUIRE(result_sig.read() == 0b1110, "0b1100 | 0b1010 should be 0b1110");
    alu_func_sig.write(ALU_XOR);
    sc_start(1, SC_NS);
    REQUIRE(result_sig.read() == 0b0110, "0b1100 ^ 0b1010 should be 0b0110");

    cout << "All test cases passed." << endl;

    return 0;

}