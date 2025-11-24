/************************************************************
 * Author: Mustafa Ergün
 * Project: CANON MCU
 * Submodule: Decoder
 *
 * Description:
 *   The Decoder interprets the data instruction and generates
 *   control signals for the CPU's various functional units.
 ************************************************************/

#ifndef DECODER_RV32I_H
#define DECODER_RV32I_H

#include <systemc.h>
#include "control_unit.h"
#include "alu_defs.h"

// 7-bit base opcodes (RV32I)
enum : uint8_t {
    OPCODE_LUI    = 0b0110111, // 0x37
    OPCODE_AUIPC  = 0b0010111, // 0x17
    OPCODE_JAL    = 0b1101111, // 0x6F
    OPCODE_JALR   = 0b1100111, // 0x67
    OPCODE_BRANCH = 0b1100011, // 0x63
    OPCODE_LOAD   = 0b0000011, // 0x03
    OPCODE_STORE  = 0b0100011, // 0x23
    OPCODE_OPIMM  = 0b0010011, // 0x13 (I-type ALU)
    OPCODE_OP     = 0b0110011, // 0x33 (R-type ALU)
    OPCODE_FENCE  = 0b0001111, // 0x0F
    OPCODE_SYSTEM = 0b1110011  // 0x73
};

SC_MODULE(decoder_RV32I) {
    // Input
    sc_in<sc_uint<32>> instr_in;  // Data instruction

    // Outputs
    sc_out<sc_uint<5>> rs1;    // Source register 1
    sc_out<sc_uint<5>> rs2;    // Source register 2
    sc_out<sc_uint<5>> rd;     // Destination register

    sc_out<sc_uint<6>> op_class;    // Operation class
    sc_out<sc_uint<3>> memMode;     // Memory mode

    sc_out<sc_uint<4>> alu_func;     // ALU function
    sc_out<sc_uint<1>> alu_src;      // ALU source
    sc_out<sc_uint<32>> imm_out;     // Immediate value

    void decode_proc(void);

        SC_CTOR(decoder_RV32I) {
            SC_METHOD(decode_proc);
            sensitive << instr_in;
            dont_initialize();
    }
};

#endif // DECODER_RV32I_H