/************************************************************
 * Author: Mustafa Ergün
 * Project: CANON MCU
 * Submodule: Register File Unit
 ************************************************************/

 #include "register_unit.h"

void register_unit::comb_read() {
    sc_uint<5> a1 = rs1_addr_in.read();
    sc_uint<5> a2 = rs2_addr_in.read();

    // x0 must always read as zero
    sc_uint<32> v1 = (a1 == 0) ? 0u : regs[a1];
    sc_uint<32> v2 = (a2 == 0) ? 0u : regs[a2];

    data_a_out.write(v1);
    data_b_out.write(v2);
}