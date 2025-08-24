/************************************************************
 * Author: Mustafa Ergün
 * Project: CANON MCU
 * Submodule: Write-Back Multiplexer (WB Mux)
 *
 * Description:
 *   Selects the correct data source to write back to the register file
 *   based on wb_sel signal from the control unit.
 ************************************************************/

#ifndef WB_MUX_H
#define WB_MUX_H

#include <systemc.h>

SC_MODULE(wb_mux) {
    // Inputs
    sc_in<sc_uint<32>> alu_in;   // ALU result
    sc_in<sc_uint<32>> load_in;  // Data loaded from memory
    sc_in<sc_uint<32>> pc4_in;   // PC + 4
    sc_in<sc_uint<2>>  wb_sel_in; // Write-back select from Control Unit

    // Output
    sc_out<sc_uint<32>> wb_out;  // Data to write to Register Unit

    void mux_process() {
        switch (wb_sel_in.read()) {
            case 0: // WB_ALU
                wb_out.write(alu_in.read());
                break;
            case 1: // WB_LOAD
                wb_out.write(load_in.read());
                break;
            case 2: // WB_PC4
                wb_out.write(pc4_in.read());
                break;
            default:
                wb_out.write(0); // Safe default
                break;
        }
    }

    SC_CTOR(wb_mux) {
        SC_METHOD(mux_process);
        sensitive << alu_in << load_in << pc4_in << wb_sel_in;
        dont_initialize();
    }
};

#endif // WB_MUX_H
