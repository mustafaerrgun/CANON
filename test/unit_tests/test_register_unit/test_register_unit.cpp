/*
 * test_register_unit.cpp
 *
 * Unit tests for the register unit of the CANON RISC-V CPU project.

 * Author: Mustafa Ergün
 * Date: October 1, 2025
 */

#include <systemc.h>
#include "../../../inc/cpu/register_unit.h"

static void REQUIRE(bool c, const char* msg){ if(!c){ SC_REPORT_ERROR("TEST", msg); }}

int sc_main(int, char**)
{
    sc_signal<sc_uint<5>> rs1_addr, rs2_addr, rd_addr;
    sc_signal<bool> we_in;
    sc_signal<sc_uint<32>> wd_in;

    sc_signal<sc_uint<32>> data_a_out, data_b_out;

    register_unit reg_unit("Register_Unit");
    reg_unit.rs1_addr_in(rs1_addr);
    reg_unit.rs2_addr_in(rs2_addr);
    reg_unit.rd_addr_in(rd_addr);
    reg_unit.we_in(we_in);
    reg_unit.wd_in(wd_in);
    reg_unit.data_a_out(data_a_out);
    reg_unit.data_b_out(data_b_out);

    // Test case 1: Initial read from x0 should be 0
    rs1_addr.write(0);
    rs2_addr.write(0);
    we_in.write(true);
    rd_addr.write(0);
    wd_in.write(1234); // Attempt to write to x0
    sc_start(1, SC_NS);
    REQUIRE(data_a_out.read() == 0, "x0 should always read as 0");
    REQUIRE(data_b_out.read() == 0, "x0 should always read as 0");

    // Test case 2: Write to x1 and read back
    we_in.write(true);
    rd_addr.write(1);
    wd_in.write(1234);
    sc_start(1, SC_NS);

    rs1_addr.write(1);
    rs2_addr.write(0);
    we_in.write(false); // Disable write
    sc_start(1, SC_NS);
    REQUIRE(data_a_out.read() == 1234, "x1 should read back the written value");
    REQUIRE(data_b_out.read() == 0, "x0 should always read as 0");

    // Test Case 3: Write Enable Signal
    we_in.write(false); // Disable write
    rd_addr.write(2);
    wd_in.write(5678);
    sc_start(1, SC_NS);

    rs1_addr.write(2);
    rs2_addr.write(0);
    sc_start(1, SC_NS);
    REQUIRE(data_a_out.read() == 0, "x2 should not change when write enable is false");
    REQUIRE(data_b_out.read() == 0, "x0 should always read as 0");

    // Test Case 4: Multiple Writes and Reads
    we_in.write(true);
    rd_addr.write(3);
    wd_in.write(42);
    sc_start(1, SC_NS);
    rd_addr.write(4);
    wd_in.write(84);
    sc_start(1, SC_NS);
    we_in.write(false);
    rs1_addr.write(3);
    rs2_addr.write(4);
    sc_start(1, SC_NS);
    REQUIRE(data_a_out.read() == 42, "x3 should read back the written value 42");
    REQUIRE(data_b_out.read() == 84, "x4 should read back the written value 84");

    // Test Case 5: Overwrite Register
    we_in.write(true);
    rd_addr.write(3);
    wd_in.write(100);
    sc_start(1, SC_NS);
    rd_addr.write(3);
    wd_in.write(200);
    sc_start(1, SC_NS);
    we_in.write(false);
    rs1_addr.write(3);
    rs2_addr.write(0);
    sc_start(1, SC_NS);
    REQUIRE(data_a_out.read() == 200, "x3 should read back the last written value 200");

    cout << "All tests passed." << endl;

    return 0;
}