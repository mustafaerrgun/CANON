/*
 * test_pc_unit.cpp
 *
 * Unit tests for the CANON RISC-V CPU project.
 * Focusing on the Program Counter (PC) functionality.
 *
 * Author: Mustafa Ergün
 * Date: October 4, 2025
 */

#include <systemc.h>
#include "pc_unit.h"

static void REQUIRE(bool c, const char* msg){ if(!c){ SC_REPORT_ERROR("TEST", msg); }}

int sc_main(int, char**)
{
    // Signals
    sc_signal<sc_uint<2>>   pc_op;
    sc_signal<sc_uint<32>>  boot_addr;
    sc_signal<bool>         reset_n;
    sc_signal<sc_uint<32>>  curr_pc;          // <-- driven by test (top-level role)
    sc_signal<sc_uint<32>>  branch_target;
    sc_signal<sc_uint<32>>  jal_target;
    sc_signal<sc_uint<32>>  jalr_target;
    sc_signal<sc_uint<32>>  pc_next;          // next PC from unit
    sc_signal<sc_uint<32>>  pc_plus4;

    // DUT
    pc_unit pcu("PC_Unit");
    pcu.pc_op_in(pc_op);
    pcu.boot_addr_in(boot_addr);
    pcu.reset_n(reset_n);
    pcu.curr_pc_in(curr_pc);                  // <-- make sure pc_unit.h exposes this
    pcu.branch_target_in(branch_target);
    pcu.jal_target_in(jal_target);
    pcu.jalr_target_in(jalr_target);
    pcu.pc_out(pc_next);
    pcu.pc_plus4_out(pc_plus4);

    // Defaults
    pc_op.write(PC_PLUS4);
    branch_target.write(0);
    jal_target.write(0);
    jalr_target.write(0);
    curr_pc.write(0);

    // --- Test Case 1: Reset Behavior ---
    boot_addr.write(0x00000000);
    reset_n.write(false);          // assert reset
    sc_start(1, SC_NS);            // simulate 1 ns
    REQUIRE(pc_next.read() == 0x00000000, "Reset: PC should equal boot_addr (0x0)");

    boot_addr.write(0x00001000); // Change boot address to 0x00001000
    sc_start(1, SC_NS);          // Simulate 1 ns
    REQUIRE(pc_next.read() == 0x00001000, "Test Case 1 Failed: PC should update to new boot address on reset");

    reset_n.write(true);         // Deassert reset
    curr_pc.write(0x00001000);
    pc_op.write(PC_PLUS4);
    sc_start(1, SC_NS);         // Simulate one clock cycle
    REQUIRE(pc_next.read() == 0x00001004, "Test Case 1 Failed: PC should increment by 4 after reset is deasserted");

    // Test Case 2: Instruction Fetch (PC + 4)
    // Step 1
    curr_pc.write(pc_next.read());
    sc_start(1, SC_NS);         // Simulate one clock cycle
    REQUIRE(pc_next.read() == 0x00001008, "Test Case 2 Failed: PC should increment by 4");
    // Step 2
    curr_pc.write(pc_next.read());
    sc_start(1, SC_NS);         // Simulate another clock cycle
    REQUIRE(pc_next.read() == 0x0000100C, "Test Case 2 Failed: PC should increment by 4");

    // Test Case 3: Branch Taken
    branch_target.write(0x00000020);
    pc_op.write(PC_BRANCH);      // BRANCH
    sc_start(1, SC_NS);         // Simulate one clock cycle
    REQUIRE(pc_next.read() == 0x00000020, "Test Case 3 Failed: PC should update to branch target");

    // --- Test Case 4: JAL ---
    jal_target.write(0x00000100);
    pc_op.write(PC_JAL);
    sc_start(1, SC_NS); 
    REQUIRE(pc_next.read() == 0x00000100, "JAL: next PC should be jal_target");
    curr_pc.write(0x0000001C);
    sc_start(1, SC_NS); 
    REQUIRE(pc_plus4.read() == 0x00000020, "pc_plus4 should be curr_pc+4");

    // --- Test Case 5: JALR (LSB forced to 0) ---
    pc_op.write(PC_JALR);
    jalr_target.write(0x00000301); // odd address -> should clear bit0
    sc_start(1, SC_NS); 
    REQUIRE(pc_next.read() == 0x00000300, "JALR: next PC must have bit0 cleared");

    cout << "All PC Unit tests passed!" << endl;
    return 0;

}