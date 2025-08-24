/************************************************************
 * Author: Mustafa Ergün
 * Project: CANON MCU
 * Submodule: Register File Unit
 *
 * Description:
 *   The Register File Unit is responsible for managing the CPU's
 *   register file, including read and write operations to the
 *   general-purpose registers.
 ************************************************************/

 #include <systemc.h>
 
 SC_MODULE(register_unit) {
    // Inputs 
    sc_in<sc_uint<5>> rs1_addr_in; // From Decoder
    sc_in<sc_uint<5>> rs2_addr_in; // From Decoder
    sc_in<sc_uint<5>> rd_addr_in;  // From Decoder

    sc_in<bool> we_in; // write enable from Control Unit

    sc_in<sc_uint<32>> wd_in; // write data from WB Mux

    // Outputs to ALU 
    sc_out<sc_uint<32>> data_a_out; // x[rs1]
    sc_out<sc_uint<32>> data_b_out; // x[rs2]

    private:
    std::array<sc_uint<32>, 32> regs{}; // 32 x 32-bit storage

    void comb_read(); // process: address → data

    void comb_write(); // process: write operation

    public:

    SC_CTOR(register_unit) {
        // Combinational read
        SC_METHOD(comb_read);
        sensitive << rs1_addr_in << rs2_addr_in;

        SC_METHOD(comb_write);
        sensitive << we_in << rd_addr_in << wd_in;

        data_a_out.initialize(0);
        data_b_out.initialize(0);

        // Ensure x0 is 0 
        regs[0] = 0;
    }
};