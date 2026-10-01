#include <SDL.h>
#include <fstream>

#include "memory.hpp"

enum class CPU_STATE : uint8_t {
	FETCH_OPCODE,
	EXECUTE
};

enum class OPERAND_TYPE : uint8_t {
	r8,
	r8inc,
	r8dec,
	r8mem, //this shit funky, its 0xFFr8 for opcode E2
	r16,
	r16stk,
	r16mem,
	r16meminc,
	r16memdec,
	cond,
	b3,
	tgt3,
	imm8,
	imm16,
	imm8mem
};

enum class op {
	unimplemented,
	CALL_a16,
	INC_B,
	INC_D,
	INC_H,
	INC_C,
	INC_E,
	INC_L,
	INC_A,
	BIT_7_H,
	LD_aC_A,
	LD_BC_d16,
	LD_DE_d16,
	LD_HL_d16,
	LD_SP_d16,
	LD_B_d8,
	LD_C_d8,
	LD_D_d8,
	LD_E_d8,
	LD_H_d8,
	LD_L_d8,
	LD_A_d8,
	LD_B_B,
	LD_B_C,
	LD_B_D,
	LD_B_E,
	LD_B_H,
	LD_B_L,
	LD_B_aHL,
	LD_B_A,
	LD_C_B,
	LD_C_C,
	LD_C_D,
	LD_C_E,
	LD_C_H,
	LD_C_L,
	LD_C_aHL,
	LD_C_A,
	LD_D_B,
	LD_D_C,
	LD_D_D,
	LD_D_E,
	LD_D_H,
	LD_D_L,
	LD_D_aHL,
	LD_D_A,
	LD_E_B,
	LD_E_C,
	LD_E_D,
	LD_E_E,
	LD_E_H,
	LD_E_L,
	LD_E_aHL,
	LD_E_A,
	LD_H_B,
	LD_H_C,
	LD_H_D,
	LD_H_E,
	LD_H_H,
	LD_H_L,
	LD_H_aHL,
	LD_H_A,
	LD_L_B,
	LD_L_C,
	LD_L_D,
	LD_L_E,
	LD_L_H,
	LD_L_L,
	LD_L_aHL,
	LD_L_A,
	LD_A_B,
	LD_A_C,
	LD_A_D,
	LD_A_E,
	LD_A_H,
	LD_A_L,
	LD_A_aHL,
	LD_A_aBC,
	LD_A_aDE,
	LD_A_A,
	LD_A_aHLD,
	LD_A_aHLI,
	XOR_A,
	LD_aHL_B,
	LD_aHL_C,
	LD_aHL_D,
	LD_aHL_E,
	LD_aHL_H,
	LD_aHL_L,
	LD_aHL_A,
	LD_aBC_A,
	LD_aDE_A,
	LD_aHLI_A,
	LD_aHLD_A,
	LD_aHL_d8,
	JR_NZ_s8,
	LD_aA8_A,
	PUSH_BC,
	PUSH_DE,
	PUSH_HL,
	PUSH_AF,
	RL_A,
	RL_B,
	RL_C,
	RL_D,
	RL_E,
	RL_H,
	RL_L,
	POP_BC,
	POP_DE,
	POP_HL,
	POP_AF,
	DEC_A,
	DEC_F,
	DEC_B,
	DEC_C,
	DEC_D,
	DEC_E,
	DEC_H,
	DEC_L,
	INC_BC,
	INC_DE,
	INC_HL,
	INC_SP,
	g_RET,
	CP_B,
	CP_C,
	CP_D,
	CP_E,
	CP_H,
	CP_L,
	CP_A,
	CP_d8,
	LD_aA16_A,
	JR_NC_s8,
	JR_Z_s8,
	JR_CY_s8,
	JR_s8,
	LD_A_aA8
};



struct Instruction {
	op operation;
	OPERAND_TYPE operand_type;
	int operand_length;
	int cycles;
};


struct RegisterPair
{
    uint8_t hi = 0;
    uint8_t lo = 0;

    static constexpr uint8_t FLAG_C = 1 << 4;
    static constexpr uint8_t FLAG_H = 1 << 5;
    static constexpr uint8_t FLAG_N = 1 << 6;
    static constexpr uint8_t FLAG_Z = 1 << 7;

    operator uint16_t() const
    {
        return (uint16_t(hi) << 8) | lo;
    }
	
	uint16_t value() const
	{
		return (uint16_t(hi) << 8) | lo;
	}

    RegisterPair& operator=(uint16_t value)
    {
        hi = value >> 8;
        lo = value & 0xFF;
        return *this;
    }

    bool c() const { return lo & FLAG_C; }
    bool h() const { return lo & FLAG_H; }
    bool n() const { return lo & FLAG_N; }
    bool z() const { return lo & FLAG_Z; }

    void c(bool value) { lo = value ? lo | FLAG_C : lo & ~FLAG_C; }
    void h(bool value) { lo = value ? lo | FLAG_H : lo & ~FLAG_H; }
    void n(bool value) { lo = value ? lo | FLAG_N : lo & ~FLAG_N; }
    void z(bool value) { lo = value ? lo | FLAG_Z : lo & ~FLAG_Z; }
};

struct Registers
{
    RegisterPair af;
    RegisterPair bc;
    RegisterPair de;
    RegisterPair hl;

    RegisterPair sp;
    RegisterPair pc;
	RegisterPair wz;
};

class cpu {
private:
	const char* bios_file = "/Users/kevin/Code/testcpp/resource/gb_bios.bin";
	bool* quit;
	CPU_STATE gb_cpu_state;
	Instruction gb_cpu_current_instruction;
	Registers gb_cpu_regs;
	memory* gb_memory;
	void load_bios();
	
	void instruction_call();
	void unimplemented_instruction();
	//instructions...
	
	void LD_x8_r8(uint8_t& reg, uint8_t operand, OPERAND_TYPE op_type);
	void LD_x16_r8(uint8_t& reg, uint16_t operand, OPERAND_TYPE op_type);
	void LD_x16_r16(RegisterPair& reg, uint16_t operand, OPERAND_TYPE op_type);
	void LD_r8_x16(uint8_t& reg, uint16_t location, OPERAND_TYPE op_type);
	void BIT_x_r(int bit, uint8_t reg);
	void JR(bool flag);
	void INC_r8(uint8_t& reg); // only used to enumerate a single reg, i.e no need for op type
	void INC_r16(RegisterPair& reg, OPERAND_TYPE op_type);
	void DEC_r8(uint8_t& reg);
	void debug_printBinary(uint8_t num);
	void XOR(uint8_t operand); // XOR ALWAYS stores to reg A so fantastic, only need the operand
	void CALL();
	void PUSH(RegisterPair& reg);
	void POP(RegisterPair& reg);
	void ROTATE_LEFT_THROUGH_CARRY_r8(uint8_t& reg, OPERAND_TYPE op_type);
	void RET();
	void CMP_x8(uint8_t& reg, OPERAND_TYPE op_type);
public:
	cpu(memory* gb_mem_location, bool* process_quit);
	Instruction get_instruction_data();
	Instruction get_cb_instruction_data();
	void tick();
};
	// haiiii tilde heloooo tilde :3  wgere is tilde...