#include "cpu.hpp"

cpu::cpu(memory* gb_mem_location, bool* process_quit) {
	gb_memory = gb_mem_location;
	gb_cpu_regs.pc = 0x00;
	quit = process_quit;
	
	//*quit = true;
	load_bios();
}

void cpu::debug_printBinary(uint8_t num) {
    for (int i = sizeof(int) * 8 - 1; i >= 0; i--) {
        SDL_Log("%d", (num >> i) & 1);
    }
    SDL_Log("\n");
}

void cpu::load_bios() {
	std::ifstream bios(bios_file, std::fstream::binary);
	if (bios.is_open() == false)
		SDL_Log("Cannot open bios\n");
	else {
		SDL_Log("Opened Bios File Successfully\n");
		bios.seekg(0, bios.end);
		int size = bios.tellg();
		bios.seekg(0, bios.beg);
		SDL_Log("Bios File size : %i\n", size);
		
		if (size != 256) {
			SDL_Log("Bios file size is not 256 bytes long. Please enter a valid bios file\n");
		} else {
			char * buffer = new char [size];
			bios.read(buffer, size);
			bios.close();
			
			gb_memory->write_buffer(buffer, size, 0x00);
			gb_memory->output_memory(0x0000, 0x00FF);
			//SDL_Log("guy : %x\n", gb_memory->read_byte(0x0000));
			//*quit = true;
		}
		
	}
}

void cpu::tick() {
	if (gb_cpu_regs.pc > 0xFF) {
		SDL_Log("We survived?");
		*quit = true;
	}
	switch(gb_cpu_state) {
		case CPU_STATE::FETCH_OPCODE:
			SDL_Log("Current Insc: %02X (PC = %02X, SP = %02X)", gb_memory->read_byte(gb_cpu_regs.pc), gb_cpu_regs.pc.lo, gb_cpu_regs.sp.lo);	
			gb_cpu_current_instruction = get_instruction_data();
			gb_cpu_current_instruction.cycles -= 1;
			gb_cpu_regs.pc = gb_cpu_regs.pc + 1;
			gb_cpu_state = CPU_STATE::EXECUTE;
			break;
		case CPU_STATE::EXECUTE:
			instruction_call();
			break;
		default:
			SDL_Log("No CPU State?");
			*quit = true;
	}
}

void cpu::unimplemented_instruction() {
	//SDL_Log("Program Counter %X, Stack Pointer %X\n", gb_cpu_regs.pc, gb_cpu_regs.sp);
	*quit = true;
}

void cpu::instruction_call() {
	switch (gb_cpu_current_instruction.operation) {
		case op::LD_B_B: LD_x8_r8(gb_cpu_regs.bc.hi, gb_cpu_regs.bc.hi, OPERAND_TYPE::r8); break;
		case op::LD_B_C: LD_x8_r8(gb_cpu_regs.bc.hi, gb_cpu_regs.bc.lo, OPERAND_TYPE::r8); break;
		case op::LD_B_D: LD_x8_r8(gb_cpu_regs.bc.hi, gb_cpu_regs.de.hi, OPERAND_TYPE::r8); break;
		case op::LD_B_E: LD_x8_r8(gb_cpu_regs.bc.hi, gb_cpu_regs.de.lo, OPERAND_TYPE::r8); break;
		case op::LD_B_H: LD_x8_r8(gb_cpu_regs.bc.hi, gb_cpu_regs.hl.hi, OPERAND_TYPE::r8); break;
		case op::LD_B_L: LD_x8_r8(gb_cpu_regs.bc.hi, gb_cpu_regs.hl.lo, OPERAND_TYPE::r8); break;
		case op::LD_B_aHL: LD_x16_r8(gb_cpu_regs.bc.hi, gb_cpu_regs.hl, OPERAND_TYPE::r16mem); break;
		case op::LD_B_A: LD_x8_r8(gb_cpu_regs.bc.hi, gb_cpu_regs.af.hi, OPERAND_TYPE::r8); break;
		case op::LD_B_d8: LD_x8_r8(gb_cpu_regs.bc.hi, gb_cpu_regs.af.hi, OPERAND_TYPE::imm8); break; //operand doesnt matter
		
		
		case op::LD_C_B: LD_x8_r8(gb_cpu_regs.bc.lo, gb_cpu_regs.bc.hi, OPERAND_TYPE::r8); break;
		case op::LD_C_C: LD_x8_r8(gb_cpu_regs.bc.lo, gb_cpu_regs.bc.lo, OPERAND_TYPE::r8); break;
		case op::LD_C_D: LD_x8_r8(gb_cpu_regs.bc.lo, gb_cpu_regs.de.hi, OPERAND_TYPE::r8); break;
		case op::LD_C_E: LD_x8_r8(gb_cpu_regs.bc.lo, gb_cpu_regs.de.lo, OPERAND_TYPE::r8); break;
		case op::LD_C_H: LD_x8_r8(gb_cpu_regs.bc.lo, gb_cpu_regs.hl.hi, OPERAND_TYPE::r8); break;
		case op::LD_C_L: LD_x8_r8(gb_cpu_regs.bc.lo, gb_cpu_regs.hl.lo, OPERAND_TYPE::r8); break;
		case op::LD_C_aHL: LD_x16_r8(gb_cpu_regs.bc.lo, gb_cpu_regs.hl, OPERAND_TYPE::r16mem); break;
		case op::LD_C_A: LD_x8_r8(gb_cpu_regs.bc.lo, gb_cpu_regs.af.hi, OPERAND_TYPE::r8); break;
		case op::LD_C_d8: LD_x8_r8(gb_cpu_regs.bc.lo, gb_cpu_regs.af.hi, OPERAND_TYPE::imm8); break; //operand doesnt matter
		
		case op::LD_D_B: LD_x8_r8(gb_cpu_regs.de.hi, gb_cpu_regs.bc.hi, OPERAND_TYPE::r8); break;
		case op::LD_D_C: LD_x8_r8(gb_cpu_regs.de.hi, gb_cpu_regs.bc.lo, OPERAND_TYPE::r8); break;
		case op::LD_D_D: LD_x8_r8(gb_cpu_regs.de.hi, gb_cpu_regs.de.hi, OPERAND_TYPE::r8); break;
		case op::LD_D_E: LD_x8_r8(gb_cpu_regs.de.hi, gb_cpu_regs.de.lo, OPERAND_TYPE::r8); break;
		case op::LD_D_H: LD_x8_r8(gb_cpu_regs.de.hi, gb_cpu_regs.hl.hi, OPERAND_TYPE::r8); break;
		case op::LD_D_L: LD_x8_r8(gb_cpu_regs.de.hi, gb_cpu_regs.hl.lo, OPERAND_TYPE::r8); break;
		case op::LD_D_aHL: LD_x16_r8(gb_cpu_regs.de.hi, gb_cpu_regs.hl, OPERAND_TYPE::r16mem); break;
		case op::LD_D_A: LD_x8_r8(gb_cpu_regs.de.hi, gb_cpu_regs.af.hi, OPERAND_TYPE::r8); break;
		case op::LD_D_d8: LD_x8_r8(gb_cpu_regs.de.hi, gb_cpu_regs.af.hi, OPERAND_TYPE::imm8); break; //operand doesnt matter
		
		case op::LD_E_B: LD_x8_r8(gb_cpu_regs.de.lo, gb_cpu_regs.bc.hi, OPERAND_TYPE::r8); break;
		case op::LD_E_C: LD_x8_r8(gb_cpu_regs.de.lo, gb_cpu_regs.bc.lo, OPERAND_TYPE::r8); break;
		case op::LD_E_D: LD_x8_r8(gb_cpu_regs.de.lo, gb_cpu_regs.de.hi, OPERAND_TYPE::r8); break;
		case op::LD_E_E: LD_x8_r8(gb_cpu_regs.de.lo, gb_cpu_regs.de.lo, OPERAND_TYPE::r8); break;
		case op::LD_E_H: LD_x8_r8(gb_cpu_regs.de.lo, gb_cpu_regs.hl.hi, OPERAND_TYPE::r8); break;
		case op::LD_E_L: LD_x8_r8(gb_cpu_regs.de.lo, gb_cpu_regs.hl.lo, OPERAND_TYPE::r8); break;
		case op::LD_E_aHL: LD_x16_r8(gb_cpu_regs.de.lo, gb_cpu_regs.hl, OPERAND_TYPE::r16mem); break;
		case op::LD_E_A: LD_x8_r8(gb_cpu_regs.de.lo, gb_cpu_regs.af.hi, OPERAND_TYPE::r8); break;
		case op::LD_E_d8: LD_x8_r8(gb_cpu_regs.de.lo, gb_cpu_regs.af.hi, OPERAND_TYPE::imm8); break; //operand doesnt matter
		
		case op::LD_H_B: LD_x8_r8(gb_cpu_regs.hl.hi, gb_cpu_regs.bc.hi, OPERAND_TYPE::r8); break;
		case op::LD_H_C: LD_x8_r8(gb_cpu_regs.hl.hi, gb_cpu_regs.bc.lo, OPERAND_TYPE::r8); break;
		case op::LD_H_D: LD_x8_r8(gb_cpu_regs.hl.hi, gb_cpu_regs.de.hi, OPERAND_TYPE::r8); break;
		case op::LD_H_E: LD_x8_r8(gb_cpu_regs.hl.hi, gb_cpu_regs.de.lo, OPERAND_TYPE::r8); break;
		case op::LD_H_H: LD_x8_r8(gb_cpu_regs.hl.hi, gb_cpu_regs.hl.hi, OPERAND_TYPE::r8); break;
		case op::LD_H_L: LD_x8_r8(gb_cpu_regs.hl.hi, gb_cpu_regs.hl.lo, OPERAND_TYPE::r8); break;
		case op::LD_H_aHL: LD_x16_r8(gb_cpu_regs.hl.hi, gb_cpu_regs.hl, OPERAND_TYPE::r16mem); break;
		case op::LD_H_A: LD_x8_r8(gb_cpu_regs.hl.hi, gb_cpu_regs.af.hi, OPERAND_TYPE::r8); break;
		case op::LD_H_d8: LD_x8_r8(gb_cpu_regs.hl.hi, gb_cpu_regs.af.hi, OPERAND_TYPE::imm8); break; //operand doesnt matter
		
		case op::LD_L_B: LD_x8_r8(gb_cpu_regs.hl.lo, gb_cpu_regs.bc.hi, OPERAND_TYPE::r8); break;
		case op::LD_L_C: LD_x8_r8(gb_cpu_regs.hl.lo, gb_cpu_regs.bc.lo, OPERAND_TYPE::r8); break;
		case op::LD_L_D: LD_x8_r8(gb_cpu_regs.hl.lo, gb_cpu_regs.de.hi, OPERAND_TYPE::r8); break;
		case op::LD_L_E: LD_x8_r8(gb_cpu_regs.hl.lo, gb_cpu_regs.de.lo, OPERAND_TYPE::r8); break;
		case op::LD_L_H: LD_x8_r8(gb_cpu_regs.hl.lo, gb_cpu_regs.hl.hi, OPERAND_TYPE::r8); break;
		case op::LD_L_L: LD_x8_r8(gb_cpu_regs.hl.lo, gb_cpu_regs.hl.lo, OPERAND_TYPE::r8); break;
		case op::LD_L_aHL: LD_x16_r8(gb_cpu_regs.hl.lo, gb_cpu_regs.hl, OPERAND_TYPE::r16mem); break;
		case op::LD_L_A: LD_x8_r8(gb_cpu_regs.hl.lo, gb_cpu_regs.af.hi, OPERAND_TYPE::r8); break;
		case op::LD_L_d8: LD_x8_r8(gb_cpu_regs.hl.lo, gb_cpu_regs.af.hi, OPERAND_TYPE::imm8); break; //operand doesnt matter
		
		case op::LD_A_B: LD_x8_r8(gb_cpu_regs.af.hi, gb_cpu_regs.bc.hi, OPERAND_TYPE::r8); break;
		case op::LD_A_C: LD_x8_r8(gb_cpu_regs.af.hi, gb_cpu_regs.bc.lo, OPERAND_TYPE::r8); break;
		case op::LD_A_D: LD_x8_r8(gb_cpu_regs.af.hi, gb_cpu_regs.de.hi, OPERAND_TYPE::r8); break;
		case op::LD_A_E: LD_x8_r8(gb_cpu_regs.af.hi, gb_cpu_regs.de.lo, OPERAND_TYPE::r8); break;
		case op::LD_A_H: LD_x8_r8(gb_cpu_regs.af.hi, gb_cpu_regs.hl.hi, OPERAND_TYPE::r8); break;
		case op::LD_A_L: LD_x8_r8(gb_cpu_regs.af.hi, gb_cpu_regs.hl.lo, OPERAND_TYPE::r8); break;
		case op::LD_A_aBC: LD_x16_r8(gb_cpu_regs.af.hi, gb_cpu_regs.bc, OPERAND_TYPE::r16mem); break;
		case op::LD_A_aDE: LD_x16_r8(gb_cpu_regs.af.hi, gb_cpu_regs.de, OPERAND_TYPE::r16mem); break;
		case op::LD_A_aHL: LD_x16_r8(gb_cpu_regs.af.hi, gb_cpu_regs.hl, OPERAND_TYPE::r16mem); break;
		case op::LD_A_aHLD: LD_x16_r8(gb_cpu_regs.af.hi, gb_cpu_regs.hl, OPERAND_TYPE::r16memdec); break;
		case op::LD_A_aHLI: LD_x16_r8(gb_cpu_regs.af.hi, gb_cpu_regs.hl, OPERAND_TYPE::r16meminc); break;
		case op::LD_A_A: LD_x8_r8(gb_cpu_regs.af.hi, gb_cpu_regs.af.hi, OPERAND_TYPE::r8); break;
		case op::LD_A_d8: LD_x8_r8(gb_cpu_regs.af.hi, gb_cpu_regs.af.hi, OPERAND_TYPE::imm8); break; //operand doesnt matter
		
		//operand doesnt matter here because no memory is 16 bits of data
		case op::LD_BC_d16: LD_x16_r16(gb_cpu_regs.bc, 0x00, OPERAND_TYPE::imm16); break;
		case op::LD_DE_d16: LD_x16_r16(gb_cpu_regs.de, 0x00, OPERAND_TYPE::imm16); break;
		case op::LD_HL_d16: LD_x16_r16(gb_cpu_regs.hl, 0x00, OPERAND_TYPE::imm16); break;
		case op::LD_SP_d16: LD_x16_r16(gb_cpu_regs.sp, 0x00, OPERAND_TYPE::imm16); break;
		
		case op::LD_aHL_B: LD_r8_x16(gb_cpu_regs.bc.hi, gb_cpu_regs.hl, OPERAND_TYPE::r8); break;
		case op::LD_aHL_C: LD_r8_x16(gb_cpu_regs.bc.lo, gb_cpu_regs.hl, OPERAND_TYPE::r8); break;
		case op::LD_aHL_D: LD_r8_x16(gb_cpu_regs.de.hi, gb_cpu_regs.hl, OPERAND_TYPE::r8); break;
		case op::LD_aHL_E: LD_r8_x16(gb_cpu_regs.de.lo, gb_cpu_regs.hl, OPERAND_TYPE::r8); break;
		case op::LD_aHL_H: LD_r8_x16(gb_cpu_regs.hl.hi, gb_cpu_regs.hl, OPERAND_TYPE::r8); break;
		case op::LD_aHL_L: LD_r8_x16(gb_cpu_regs.hl.lo, gb_cpu_regs.hl, OPERAND_TYPE::r8); break;
		case op::LD_aHL_A: LD_r8_x16(gb_cpu_regs.af.hi, gb_cpu_regs.hl, OPERAND_TYPE::r8); break;
		
		case op::LD_aHL_d8: LD_r8_x16(gb_cpu_regs.wz.lo, gb_cpu_regs.hl, OPERAND_TYPE::imm8); break; // dummy reg passed in
		
		case op::LD_aBC_A: LD_r8_x16(gb_cpu_regs.af.hi, gb_cpu_regs.bc, OPERAND_TYPE::r8); break;
		case op::LD_aDE_A: LD_r8_x16(gb_cpu_regs.af.hi, gb_cpu_regs.de, OPERAND_TYPE::r8); break;
		case op::LD_aHLI_A: LD_r8_x16(gb_cpu_regs.af.hi, gb_cpu_regs.hl, OPERAND_TYPE::r8inc); break;
		case op::LD_aHLD_A: LD_r8_x16(gb_cpu_regs.af.hi, gb_cpu_regs.hl, OPERAND_TYPE::r8dec); break;
		
		case op::BIT_7_H: BIT_x_r(7, gb_cpu_regs.hl.hi); break;
		
		case op::JR_NZ_s8: JR(gb_cpu_regs.af.z() == 0); break;
		
		case op::INC_B: INC_r8(gb_cpu_regs.bc.hi); break;
		case op::INC_D: INC_r8(gb_cpu_regs.de.hi); break;
		case op::INC_H: INC_r8(gb_cpu_regs.de.hi); break;
		case op::INC_C: INC_r8(gb_cpu_regs.bc.lo); break;
		case op::INC_E: INC_r8(gb_cpu_regs.de.lo); break;
		case op::INC_L: INC_r8(gb_cpu_regs.hl.lo); break;
		case op::INC_A: INC_r8(gb_cpu_regs.af.hi); break;
		
		case op::DEC_B: DEC_r8(gb_cpu_regs.bc.hi); break;
		case op::DEC_D: DEC_r8(gb_cpu_regs.de.hi); break;
		case op::DEC_H: DEC_r8(gb_cpu_regs.de.hi); break;
		case op::DEC_C: DEC_r8(gb_cpu_regs.bc.lo); break;
		case op::DEC_E: DEC_r8(gb_cpu_regs.de.lo); break;
		case op::DEC_L: DEC_r8(gb_cpu_regs.hl.lo); break;
		case op::DEC_A: DEC_r8(gb_cpu_regs.af.hi); break;
		
		case op::LD_aC_A: LD_x8_r8(gb_cpu_regs.bc.lo, gb_cpu_regs.af.hi, OPERAND_TYPE::r8mem); break;
		case op::LD_aA8_A: LD_x8_r8(gb_cpu_regs.bc.lo, 0x00, OPERAND_TYPE::imm8mem); break;
		
		case op::CALL_a16: CALL(); break;
		
		case op::XOR_A :XOR(gb_cpu_regs.af.hi); break;
		
		case op::PUSH_BC: PUSH(gb_cpu_regs.bc); break;
		case op::PUSH_DE: PUSH(gb_cpu_regs.de); break;
		case op::PUSH_HL: PUSH(gb_cpu_regs.hl); break;
		case op::PUSH_AF: PUSH(gb_cpu_regs.af); break;
		
		case op::RL_A: ROTATE_LEFT_THROUGH_CARRY_r8(gb_cpu_regs.af.hi, OPERAND_TYPE::r8); break;
		case op::RL_B: ROTATE_LEFT_THROUGH_CARRY_r8(gb_cpu_regs.bc.hi, OPERAND_TYPE::r8); break;
		case op::RL_C: ROTATE_LEFT_THROUGH_CARRY_r8(gb_cpu_regs.bc.lo, OPERAND_TYPE::r8); break;
		case op::RL_D: ROTATE_LEFT_THROUGH_CARRY_r8(gb_cpu_regs.de.hi, OPERAND_TYPE::r8); break;
		case op::RL_E: ROTATE_LEFT_THROUGH_CARRY_r8(gb_cpu_regs.de.lo, OPERAND_TYPE::r8); break;
		case op::RL_H: ROTATE_LEFT_THROUGH_CARRY_r8(gb_cpu_regs.hl.hi, OPERAND_TYPE::r8); break;
		case op::RL_L: ROTATE_LEFT_THROUGH_CARRY_r8(gb_cpu_regs.hl.lo, OPERAND_TYPE::r8); break;
		
		case op::POP_BC: POP(gb_cpu_regs.bc); break;
		case op::POP_DE: POP(gb_cpu_regs.de); break;
		case op::POP_HL: POP(gb_cpu_regs.hl); break;
		case op::POP_AF: POP(gb_cpu_regs.af); break;
		
		case op::INC_BC: INC_r16(gb_cpu_regs.bc, OPERAND_TYPE::r16); break;
		case op::INC_DE: INC_r16(gb_cpu_regs.de, OPERAND_TYPE::r16); break;
		case op::INC_HL: INC_r16(gb_cpu_regs.hl, OPERAND_TYPE::r16); break;
		case op::INC_SP: INC_r16(gb_cpu_regs.sp, OPERAND_TYPE::r16); break;
		
		case op::g_RET: RET(); break;
		
		case op::CP_B: CMP_x8(gb_cpu_regs.bc.hi, OPERAND_TYPE::r8); break;
		case op::CP_C: CMP_x8(gb_cpu_regs.bc.lo, OPERAND_TYPE::r8); break;
		case op::CP_D: CMP_x8(gb_cpu_regs.de.hi, OPERAND_TYPE::r8); break;
		case op::CP_E: CMP_x8(gb_cpu_regs.de.lo, OPERAND_TYPE::r8); break;
		case op::CP_H: CMP_x8(gb_cpu_regs.hl.hi, OPERAND_TYPE::r8); break;
		case op::CP_L: CMP_x8(gb_cpu_regs.hl.lo, OPERAND_TYPE::r8); break;
		case op::CP_A: CMP_x8(gb_cpu_regs.af.hi, OPERAND_TYPE::r8); break;
		
		case op::CP_d8: CMP_x8(gb_cpu_regs.af.lo, OPERAND_TYPE::imm8); break; // reg passed in doesn't matter lol
		case op::LD_aA16_A: LD_r8_x16(gb_cpu_regs.af.hi, 0x00, OPERAND_TYPE::imm16); break;
		
		case op::JR_NC_s8: JR(gb_cpu_regs.af.c() == 0); break;
		case op::JR_Z_s8: JR(gb_cpu_regs.af.z()); break;
		case op::JR_CY_s8: JR(gb_cpu_regs.af.c()); break;
		case op::JR_s8: JR(0); break;
		
		case op::LD_A_aA8: LD_x16_r8(gb_cpu_regs.af.hi, 0x00, OPERAND_TYPE::imm8mem); break; // operand doesnt matter...
		
		default: unimplemented_instruction();
	}
}

void cpu::JR(bool flag) {
	//SDL_Log("flag : %d", flag);
	gb_cpu_current_instruction.cycles -= 1;
	if (flag || gb_cpu_current_instruction.operation == op::JR_s8) {
		if (gb_cpu_current_instruction.cycles == 0) {
			int8_t jmp = gb_memory->read_byte(gb_cpu_regs.pc);
			gb_cpu_regs.pc = gb_cpu_regs.pc + jmp;
			gb_cpu_regs.pc = gb_cpu_regs.pc + 1;
			gb_cpu_state = CPU_STATE::FETCH_OPCODE;
			//SDL_Log("Program Counter %02x, HL %02X%02X : %02X\n", gb_cpu_regs.pc, gb_cpu_regs.hl.hi, gb_cpu_regs.hl.lo, gb_memory->read_byte(gb_cpu_regs.hl));
		}
	} else {
		gb_cpu_regs.pc = gb_cpu_regs.pc + 1;
		//SDL_Log("Done with Clearing ram, HL is, %02X, We are at : %02X", gb_cpu_regs.hl.value(), gb_cpu_regs.pc);
		gb_cpu_state = CPU_STATE::FETCH_OPCODE;
		//*quit = true;
	}
}

Instruction cpu::get_cb_instruction_data() {
	Instruction insc = {};
	uint8_t opcode = gb_memory->read_byte(gb_cpu_regs.pc);
	switch (opcode) {
		case 0x7C:
			insc.operation = op::BIT_7_H;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x10:
			insc.operation = op::RL_B;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x11:
			insc.operation = op::RL_C;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x12:
			insc.operation = op::RL_D;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x13:
			insc.operation = op::RL_E;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x14:
			insc.operation = op::RL_H;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x15:
			insc.operation = op::RL_L;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		default:
			insc.operation = op::unimplemented;
			SDL_Log("CB Instruction unimplemented, opcode : %X\n", opcode);
			//SDL_Log("CB Program Counter %02x, Stack Pointer %02X%02X\n", gb_cpu_regs.pc, gb_cpu_regs.sp.hi, gb_cpu_regs.sp.lo);
			*quit = true;
	}
	return insc;
}

Instruction cpu::get_instruction_data() {
	Instruction insc = {};
	uint8_t opcode = gb_memory->read_byte(gb_cpu_regs.pc);
	switch (opcode) {
		case 0x01:
			insc.operation = op::LD_BC_d16;
			insc.cycles = 3;
			insc.operand_type = OPERAND_TYPE::imm16;
			break;
		case 0x02:
			insc.operation = op::LD_aBC_A;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x03:
			insc.operation = op::INC_BC;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r16;
			break;
		case 0x04:
			insc.operation = op::INC_B;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x05:
			insc.operation = op::DEC_B;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x06:
			insc.operation = op::LD_B_d8;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::imm8;
			break;
		case 0x0C:
			insc.operation = op::INC_C;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x0D:
			insc.operation = op::DEC_C;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x0E:
			insc.operation = op::LD_C_d8;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::imm8;
			break;
		case 0x11:
			insc.operation = op::LD_DE_d16;
			insc.cycles = 3;
			insc.operand_type = OPERAND_TYPE::imm16;
			break;
		case 0x12:
			insc.operation = op::LD_aDE_A;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x13:
			insc.operation = op::INC_DE;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r16;
			break;
		case 0x14:
			insc.operation = op::INC_D;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x15:
			insc.operation = op::DEC_D;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x16:
			insc.operation = op::LD_D_d8;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::imm8;
			break;
		case 0x17:
			insc.operation = op::RL_A;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::imm8;
			break;
		case 0x18:
			insc.operation = op::JR_s8;
			insc.cycles = 3;
			insc.operand_type = OPERAND_TYPE::imm8;
			break;
		case 0x1A:
			insc.operation = op::LD_A_aDE;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r16mem;
			break;
		case 0x1C:
			insc.operation = op::INC_E;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x1D:
			insc.operation = op::DEC_E;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x1E:
			insc.operation = op::LD_E_d8;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::imm8;
			break;
		case 0x20:
			insc.operation = op::JR_NZ_s8;
			insc.cycles = 3;
			insc.operand_type = OPERAND_TYPE::imm8;
			break;
		case 0x21:
			insc.operation = op::LD_HL_d16;
			insc.cycles = 3;
			insc.operand_type = OPERAND_TYPE::imm16;
			break;
		case 0x22:
			insc.operation = op::LD_aHLI_A;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8inc;
			break;
		case 0x23:
			insc.operation = op::INC_HL;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r16;
			break;
		case 0x24:
			insc.operation = op::INC_H;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x25:
			insc.operation = op::DEC_H;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x26:
			insc.operation = op::LD_H_d8;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::imm8;
			break;
		case 0x28:
			insc.operation = op::JR_Z_s8;
			insc.cycles = 3;
			insc.operand_type = OPERAND_TYPE::imm8;
			break;
		case 0x2C:
			insc.operation = op::INC_L;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x2D:
			insc.operation = op::DEC_L;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x2E:
			insc.operation = op::LD_L_d8;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::imm8;
			break;
		case 0x30:
			insc.operation = op::JR_NC_s8;
			insc.cycles = 3;
			insc.operand_type = OPERAND_TYPE::imm8;
			break;
		case 0x31:
			insc.operation = op::LD_SP_d16;
			insc.cycles = 3;
			insc.operand_type = OPERAND_TYPE::imm16;
			break;
		case 0x32:
			insc.operation = op::LD_aHLD_A;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8dec;
			break;
		case 0x33:
			insc.operation = op::INC_SP;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r16;
			break;
		case 0x36:
			insc.operation = op::LD_aHL_d8;
			insc.cycles = 3;
			insc.operand_type = OPERAND_TYPE::imm8;
			break;
		case 0x38:
			insc.operation = op::JR_CY_s8;
			insc.cycles = 3;
			insc.operand_type = OPERAND_TYPE::imm8;
			break;
		case 0x3C:
			insc.operation = op::INC_A;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x3D:
			insc.operation = op::DEC_A;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x3E:
			insc.operation = op::LD_A_d8;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::imm8;
			break;
		case 0x40:
			insc.operation = op::LD_B_B;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x41:
			insc.operation = op::LD_B_C;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x42:
			insc.operation = op::LD_B_D;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x43:
			insc.operation = op::LD_B_E;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x44:
			insc.operation = op::LD_B_H;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x45:
			insc.operation = op::LD_B_L;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x46:
			insc.operation = op::LD_B_aHL;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x47:
			insc.operation = op::LD_B_A;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x48:
			insc.operation = op::LD_C_B;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x49:
			insc.operation = op::LD_C_C;
			insc.cycles = 1;
			break;
			insc.operand_type = OPERAND_TYPE::r8;
		case 0x4A:
			insc.operation = op::LD_C_D;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x4B:
			insc.operation = op::LD_C_E;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x4C:
			insc.operation = op::LD_C_H;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x4D:
			insc.operation = op::LD_C_L;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x4E:
			insc.operation = op::LD_C_aHL;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x4F:
			insc.operation = op::LD_C_A;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x50:
			insc.operation = op::LD_D_B;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x51:
			insc.operation = op::LD_D_C;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x52:
			insc.operation = op::LD_D_D;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x53:
			insc.operation = op::LD_D_E;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x54:
			insc.operation = op::LD_D_H;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x55:
			insc.operation = op::LD_D_L;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x56:
			insc.operation = op::LD_D_aHL;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x57:
			insc.operation = op::LD_D_A;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x58:
			insc.operation = op::LD_E_B;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x59:
			insc.operation = op::LD_E_C;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x5A:
			insc.operation = op::LD_E_D;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x5B:
			insc.operation = op::LD_E_E;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x5C:
			insc.operation = op::LD_E_H;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x5D:
			insc.operation = op::LD_E_L;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x5E:
			insc.operation = op::LD_E_aHL;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x5F:
			insc.operation = op::LD_E_A;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x60:
			insc.operation = op::LD_H_B;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x61:
			insc.operation = op::LD_H_C;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x62:
			insc.operation = op::LD_H_D;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x63:
			insc.operation = op::LD_H_E;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x64:
			insc.operation = op::LD_H_H;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x65:
			insc.operation = op::LD_H_L;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x66:
			insc.operation = op::LD_H_aHL;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x67:
			insc.operation = op::LD_H_A;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x68:
			insc.operation = op::LD_L_B;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x69:
			insc.operation = op::LD_L_C;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x6A:
			insc.operation = op::LD_L_D;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x6B:
			insc.operation = op::LD_L_E;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x6C:
			insc.operation = op::LD_L_H;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x6D:
			insc.operation = op::LD_L_L;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x6E:
			insc.operation = op::LD_L_aHL;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x6F:
			insc.operation = op::LD_L_A;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x70:
			insc.operation = op::LD_aHL_B;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x71:
			insc.operation = op::LD_aHL_C;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x72:
			insc.operation = op::LD_aHL_D;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x73:
			insc.operation = op::LD_aHL_E;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x74:
			insc.operation = op::LD_aHL_H;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x75:
			insc.operation = op::LD_aHL_L;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x77:
			insc.operation = op::LD_aHL_A;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x78:
			insc.operation = op::LD_A_B;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x79:
			insc.operation = op::LD_A_C;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x7A:
			insc.operation = op::LD_A_D;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x7B:
			insc.operation = op::LD_A_E;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x7C:
			insc.operation = op::LD_A_H;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x7D:
			insc.operation = op::LD_A_L;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x7E:
			insc.operation = op::LD_A_aHL;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0x7F:
			insc.operation = op::LD_A_A;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0xAF:
			insc.operation = op::XOR_A;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0xB8:
			insc.operation = op::CP_B;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0xB9:
			insc.operation = op::CP_C;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0xBA:
			insc.operation = op::CP_D;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0xBB:
			insc.operation = op::CP_E;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0xBC:
			insc.operation = op::CP_H;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0xBD:
			insc.operation = op::CP_L;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0xBF:
			insc.operation = op::CP_A;
			insc.cycles = 1;
			insc.operand_type = OPERAND_TYPE::r8;
			break;
		case 0xC1:
			insc.operation = op::POP_BC;
			insc.cycles = 3;
			insc.operand_type = OPERAND_TYPE::r16;
			break;
		case 0xC5:
			insc.operation = op::PUSH_BC;
			insc.cycles = 4;
			insc.operand_type = OPERAND_TYPE::r16;
			break;
		case 0xC9:
			insc.operation = op::g_RET;
			insc.cycles = 4;
			insc.operand_type = OPERAND_TYPE::r8; //doesn't matter...
			break;
		case 0xCB:
			gb_cpu_regs.pc = gb_cpu_regs.pc + 1;
			insc = get_cb_instruction_data();
			break;
		case 0xCD:
			insc.operation = op::CALL_a16;
			insc.cycles = 6;
			insc.operand_type = OPERAND_TYPE::r8; //doesnt matter
			break;
		case 0xD1:
			insc.operation = op::POP_DE;
			insc.cycles = 3;
			insc.operand_type = OPERAND_TYPE::r16;
			break;
		case 0xD5:
			insc.operation = op::PUSH_DE;
			insc.cycles = 4;
			insc.operand_type = OPERAND_TYPE::r16;
			break;
		case 0xE0:
			insc.operation = op::LD_aA8_A;
			insc.cycles = 3,
			insc.operand_type = OPERAND_TYPE::imm8mem;
			break;
		case 0xE1:
			insc.operation = op::POP_HL;
			insc.cycles = 3;
			insc.operand_type = OPERAND_TYPE::r16;
			break;
		case 0xE2:
			insc.operation = op::LD_aC_A;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::r8mem;
			break;
		case 0xE5:
			insc.operation = op::PUSH_HL;
			insc.cycles = 4;
			insc.operand_type = OPERAND_TYPE::r16;
			break;
		case 0xEA:
			insc.operation = op::LD_aA16_A;
			insc.cycles = 4;
			insc.operand_type = OPERAND_TYPE::imm16;
			break;
		case 0xF0:
			insc.operation = op::LD_A_aA8;
			insc.cycles = 3;
			insc.operand_type = OPERAND_TYPE::imm8mem;
			break;
		case 0xF1:
			insc.operation = op::POP_AF;
			insc.cycles = 3;
			insc.operand_type = OPERAND_TYPE::r16;
			break;
		case 0xF5:
			insc.operation = op::PUSH_AF;
			insc.cycles = 4;
			insc.operand_type = OPERAND_TYPE::r16;
			break;
		case 0xFE:
			insc.operation = op::CP_d8;
			insc.cycles = 2;
			insc.operand_type = OPERAND_TYPE::imm8;
			break;
		default:
			insc.operation = op::unimplemented;
			//SDL_Log("A : %02X C : %02X HL : %02X%02X", gb_cpu_regs.af.hi, gb_cpu_regs.bc.lo, gb_cpu_regs.hl.hi, gb_cpu_regs.hl.lo);
			SDL_Log("A : %02X C : %02X HL : %02X", gb_cpu_regs.af.hi, gb_cpu_regs.bc.lo, gb_cpu_regs.hl.value());
			SDL_Log("Instruction unimplemented, opcode : %X\n", opcode);
			//SDL_Log("Program Counter %02x, Stack Pointer %02X%02X\n", gb_cpu_regs.pc, gb_cpu_regs.sp.hi, gb_cpu_regs.sp.lo);
			*quit = true;
			
	}
	
	return insc;
}

void cpu::XOR(uint8_t operand) {
	if (gb_cpu_current_instruction.cycles == 0) {
		gb_cpu_regs.af.hi = gb_cpu_regs.af.hi | operand;
		gb_cpu_state = CPU_STATE::FETCH_OPCODE;
	}
	//SDL_Log("A has been XORRED, A is %02X", gb_cpu_regs.af.hi);
}
//ld x8 into r8
void cpu::LD_x8_r8(uint8_t& reg, uint8_t operand, OPERAND_TYPE op_type) {
	gb_cpu_current_instruction.cycles -= 1;
	switch(op_type) {
		case OPERAND_TYPE::r8:
				reg = operand;
				gb_cpu_state = CPU_STATE::FETCH_OPCODE;
			break;
		case OPERAND_TYPE::r8mem:
			//SDL_Log("Hello! %u", gb_cpu_current_instruction.cycles);
			
			if (gb_cpu_current_instruction.cycles == 0) {
				uint16_t x = 0xFF00 + operand;
				gb_memory->write_byte(x, reg);
				//should be called for LD (C) A, so lets check C,
				//gb_cpu_regs.pc -=1; // IGNORE I REFACTORED : weird it takes 1 byte but 2 cycles so to stop it from clicking on i need to -- the pc
				SDL_Log("should write %02X to %02X, but it is %02X", reg, x, gb_memory->read_byte(x));
				gb_cpu_state = CPU_STATE::FETCH_OPCODE;
			}
			break;
		case OPERAND_TYPE::imm8:
			if (gb_cpu_current_instruction.cycles == 0) {
				reg = gb_memory->read_byte(gb_cpu_regs.pc);
				gb_cpu_state = CPU_STATE::FETCH_OPCODE;
				gb_cpu_regs.pc = gb_cpu_regs.pc + 1;
			}
			break;
		case OPERAND_TYPE::imm8mem: 
			if (gb_cpu_current_instruction.cycles == 0) {
				//cycle = 3 fetches opcode
				// cycle = 2 fetches next byte
				// cycle 3 = writes next byte. we only execute on cycle 3 so it needs to go back to do it, i think!
				uint16_t x = 0xFF00 + gb_memory->read_byte(gb_cpu_regs.pc);
				gb_memory->write_byte(x, reg);
				SDL_Log("!!should write %02X to %02X, but it is %02X ", reg, x, gb_memory->read_byte(x));
				gb_cpu_state = CPU_STATE::FETCH_OPCODE;
				gb_cpu_regs.pc = gb_cpu_regs.pc + 1;
			}
			break;
		default:
			SDL_Log("Operand type for LD_stk not implemented yet!");
			*quit = true;
	}
	
}


//ld x16 into r8
void cpu::LD_x16_r8(uint8_t& reg, uint16_t operand, OPERAND_TYPE op_type) {
	gb_cpu_current_instruction.cycles -= 1;
	switch(op_type) {
		case OPERAND_TYPE::r16mem:
			if (gb_cpu_current_instruction.cycles == 0) {
				reg = gb_memory->read_byte(operand);
				gb_cpu_state = CPU_STATE::FETCH_OPCODE;
			}
			break;
		case OPERAND_TYPE::r16meminc:
			if (gb_cpu_current_instruction.cycles == 0) {
				reg = gb_memory->read_byte(operand);
				gb_cpu_regs.hl = gb_cpu_regs.hl + 1;
				gb_cpu_state = CPU_STATE::FETCH_OPCODE;
			}
			break;
		case OPERAND_TYPE::r16memdec:
			if (gb_cpu_current_instruction.cycles == 0) {
				reg = gb_memory->read_byte(operand);
				gb_cpu_regs.hl = gb_cpu_regs.hl - 1;
				gb_cpu_state = CPU_STATE::FETCH_OPCODE;
			}
			break;
		case OPERAND_TYPE::imm8mem:
			switch(gb_cpu_current_instruction.cycles) {
				case (2):
					gb_cpu_regs.wz = 0xFF00 + gb_memory->read_byte(gb_cpu_regs.pc);
					break;
				case (1):
					gb_cpu_regs.af.hi = gb_memory->read_byte(gb_cpu_regs.wz);
					break;
				case (0):
					gb_cpu_regs.pc = gb_cpu_regs.pc + 1;
					gb_cpu_state = CPU_STATE::FETCH_OPCODE;
					break;
				default:
					SDL_Log("Doesn't work... imm8mem has fell through");
			}
			break;
		default:
			SDL_Log("Operand type for LD_stk not implemented yet!");
			*quit = true;
	}
	//gb_cpu_regs.pc += 1;
}

//ld x16 into r16
void cpu::LD_x16_r16(RegisterPair& reg, uint16_t operand, OPERAND_TYPE op_type) {
	gb_cpu_current_instruction.cycles -= 1;
	switch(op_type) {
		case OPERAND_TYPE::imm16:
			if (gb_cpu_current_instruction.cycles == 1) {
				reg.lo = gb_memory->read_byte(gb_cpu_regs.pc);
			} else if (gb_cpu_current_instruction.cycles == 0) {
				reg.hi = gb_memory->read_byte(gb_cpu_regs.pc);
				gb_cpu_state = CPU_STATE::FETCH_OPCODE;
				//SDL_Log("Program Counter %02x, HL %02X%02X : %02X\n", gb_cpu_regs.pc, gb_cpu_regs.hl.hi, gb_cpu_regs.hl.lo, gb_memory->read_byte(gb_cpu_regs.hl));
				//quit = true;
			}
			break;
		default:
			SDL_Log("Operand type for LD_x16_r16 not implemented yet!");
			*quit = true;
	}
	gb_cpu_regs.pc = gb_cpu_regs.pc + 1;
}
//ld r8 into x16
void cpu::LD_r8_x16(uint8_t& reg, uint16_t location, OPERAND_TYPE op_type) {
	gb_cpu_current_instruction.cycles -= 1;
	switch(op_type) {
		case OPERAND_TYPE::r8:
			if (gb_cpu_current_instruction.cycles == 0) {
				gb_memory->write_byte(location, reg);
				gb_cpu_state = CPU_STATE::FETCH_OPCODE;
			}
			break;
		
		case OPERAND_TYPE::r8inc:
			if (gb_cpu_current_instruction.cycles == 0) {
				gb_memory->write_byte(location, reg);
				SDL_Log("Writing %02X to %02X", reg, location);
				gb_cpu_regs.hl = gb_cpu_regs.hl + 1;
				gb_cpu_state = CPU_STATE::FETCH_OPCODE;
			}
			break;
		case OPERAND_TYPE::r8dec:
			if (gb_cpu_current_instruction.cycles == 0) {
				gb_memory->write_byte(location, reg);
				//SDL_Log("wrote reg %u into location %02X\n", reg, location);
				//SDL_Log("Program Counter %02x, HL %02X%02X : %02X\n", gb_cpu_regs.pc, gb_cpu_regs.hl.hi, gb_cpu_regs.hl.lo, gb_memory->read_byte(gb_cpu_regs.hl));
				gb_cpu_regs.hl = gb_cpu_regs.hl - 1;
				gb_cpu_state = CPU_STATE::FETCH_OPCODE;
			}
			break;
		case OPERAND_TYPE::imm8:
			if (gb_cpu_current_instruction.cycles == 0) {
				gb_memory->write_byte(location, gb_memory->read_byte(gb_cpu_regs.pc));
				gb_cpu_state = CPU_STATE::FETCH_OPCODE;
				gb_cpu_regs.pc = gb_cpu_regs.pc + 1;
			}
			break;
		case OPERAND_TYPE::imm16:
			switch(gb_cpu_current_instruction.cycles) {
				case (2):
					gb_cpu_regs.wz.hi = gb_memory->read_byte(gb_cpu_regs.pc);
					break;
				case (1):
					gb_cpu_regs.wz.lo = gb_memory->read_byte(gb_cpu_regs.pc + 1);
					break;
				case (0):
					gb_memory->write_byte(gb_cpu_regs.wz, reg);
					gb_cpu_regs.pc = gb_cpu_regs. pc + 2;
					gb_cpu_state = CPU_STATE::FETCH_OPCODE;
					break;
			}
			break;
		default:
			SDL_Log("Operand type for LD_x16_r16 not implemented yet!");
			*quit = true;
	}
	
}
//take the complement of bit x in register and store it in a z flag
void cpu::BIT_x_r(int bit, uint8_t reg) {
    gb_cpu_current_instruction.cycles -= 1;
    if (gb_cpu_current_instruction.cycles == 0) {
		gb_cpu_regs.af.z(((gb_cpu_regs.hl.hi) >> 7) ^ 1);
        gb_cpu_state = CPU_STATE::FETCH_OPCODE;
    }
}

void cpu::INC_r8(uint8_t& reg) {
	uint8_t old = reg;
    reg++;
	
	gb_cpu_regs.af.z(reg == 0);
	gb_cpu_regs.af.n(false);
	gb_cpu_regs.af.h((old & 0x0F) == 0x0F);
	gb_cpu_state = CPU_STATE::FETCH_OPCODE;
	//*quit = true;
}

void cpu::DEC_r8(uint8_t& reg) {
	uint8_t old = reg;
    reg--;
	
	gb_cpu_regs.af.z(reg == 0);
	gb_cpu_regs.af.n(true);
	gb_cpu_regs.af.h(old & 0x0F);
	gb_cpu_state = CPU_STATE::FETCH_OPCODE;
	//*quit = true;
}

void cpu::INC_r16(RegisterPair& reg, OPERAND_TYPE op_type) {
	gb_cpu_current_instruction.cycles -= 1;
	switch (op_type) {
		case OPERAND_TYPE::r16:
			reg = reg + 1;
			gb_cpu_state = CPU_STATE::FETCH_OPCODE;
			break;
		case OPERAND_TYPE::r16mem:
			if (gb_cpu_current_instruction.cycles == 0) {
				gb_memory->write_byte(reg, gb_memory->read_byte(reg) - 1);
				gb_cpu_state = CPU_STATE::FETCH_OPCODE;
			}
			break;
		default:
			SDL_Log("err?");
			*quit = true;
	}
}

void cpu::CALL() {
	//cycle 6 get opcode, cycle 5, get next byte, cycle 4 get last byte, cycle 3, do logic but dont enumerate pc
	
	gb_cpu_current_instruction.cycles -= 1;
	switch (gb_cpu_current_instruction.cycles) {
		case (4):
			SDL_Log("Function called!!");
			gb_cpu_regs.wz.lo = gb_memory->read_byte(gb_cpu_regs.pc);
			gb_cpu_regs.pc = gb_cpu_regs.pc + 1;
			break;
		case (3):
			//SDL_Log("PC is at %02X im looking at %02X", gb_cpu_regs.pc, gb_memory->read_byte(gb_cpu_regs.pc));
			gb_cpu_regs.wz.hi = gb_memory->read_byte(gb_cpu_regs.pc);
			break;
		case (2):
			gb_cpu_regs.pc = gb_cpu_regs.pc + 1;
			break;
		case (1):
			gb_cpu_regs.sp = gb_cpu_regs.sp - 1;
			gb_memory->write_byte(gb_cpu_regs.sp, gb_cpu_regs.pc.hi);

			gb_cpu_regs.sp = gb_cpu_regs.sp - 1;
			gb_memory->write_byte(gb_cpu_regs.sp, gb_cpu_regs.pc.lo);
			break;
		case (0):
			gb_cpu_regs.pc = gb_cpu_regs.wz;
			gb_cpu_state = CPU_STATE::FETCH_OPCODE;
			break;
		default:
			SDL_Log("cycles at %u", gb_cpu_current_instruction.cycles);
			if (gb_cpu_current_instruction.cycles == 0) {
				SDL_Log("How did we get here??");
			}
	}
	//SDL_Log("Do nothing!, PC is at: %02X\n", gb_cpu_regs.pc);
}

void cpu::PUSH(RegisterPair& reg) {
	gb_cpu_current_instruction.cycles -= 1;
	switch (gb_cpu_current_instruction.cycles) {
		case (2):
			gb_memory->write_byte(gb_cpu_regs.sp - 1, reg.hi);
			break;
		case (1):
			gb_memory->write_byte(gb_cpu_regs.sp - 2, reg.lo);
			break;
		case (0):
			gb_cpu_regs.sp = gb_cpu_regs.sp - 2;
			gb_cpu_state = CPU_STATE::FETCH_OPCODE;
			break;
		default:
			SDL_Log("How did we get here?? the cycles on the PUSH call has fallen through we are at %02X", gb_cpu_current_instruction.cycles);
			*quit = true;
	}
}

void cpu::ROTATE_LEFT_THROUGH_CARRY_r8(uint8_t& reg, OPERAND_TYPE op_type) {
	uint8_t old_carry = (gb_cpu_regs.af.lo & 0x10) ? 1 : 0;
	uint8_t new_carry = (reg & 0x80) ? 1 : 0;

	reg = (reg << 1) | old_carry;

	gb_cpu_regs.af.z(reg == 0);
	gb_cpu_regs.af.n(false);
	gb_cpu_regs.af.h(false);
	gb_cpu_regs.af.c(new_carry);
	
	//gb_cpu_regs.pc = gb_cpu_regs.pc + 1;
	gb_cpu_state = CPU_STATE::FETCH_OPCODE;
	//SDL_Log("Tes2t, pc at %02X", gb_cpu_regs.pc.lo);
}

void cpu::POP(RegisterPair& reg) {
	gb_cpu_current_instruction.cycles -= 1;
	switch (gb_cpu_current_instruction.cycles) {
		case (1):
			reg.lo = gb_memory->read_byte(gb_cpu_regs.sp);
			gb_cpu_regs.sp = gb_cpu_regs.sp + 1;
			break;
		case (0):
			reg.hi = gb_memory->read_byte(gb_cpu_regs.sp);
			gb_cpu_regs.sp = gb_cpu_regs.sp + 1;
			gb_cpu_state = CPU_STATE::FETCH_OPCODE;
			break;
		default:
			SDL_Log("Fell through POP");
			break;
	}
}

void cpu::RET() {
	gb_cpu_current_instruction.cycles -= 1;
	SDL_Log("Cycles at %02X", gb_cpu_current_instruction.cycles);
	switch (gb_cpu_current_instruction.cycles) {
		case (2):
			gb_cpu_regs.pc.lo = gb_memory->read_byte(gb_cpu_regs.sp);
			break;
		case (1):
			gb_cpu_regs.pc.hi = gb_memory->read_byte(gb_cpu_regs.sp + 1);
			break;
		case (0):
			gb_cpu_regs.sp = gb_cpu_regs.sp + 2;
			gb_cpu_state = CPU_STATE::FETCH_OPCODE;
			break;
		default:
			SDL_Log("Ret fell through?!");
			*quit = true;
	}
}

void cpu::CMP_x8(uint8_t& reg, OPERAND_TYPE operand_type) {
	switch(operand_type) {
		case OPERAND_TYPE::r8:
			gb_cpu_regs.af.z(gb_cpu_regs.af.hi - reg == 0);
			break;
		case OPERAND_TYPE::imm8:
			gb_cpu_regs.af.z(gb_cpu_regs.af.hi - gb_memory->read_byte(gb_cpu_regs.pc) == 0);
			break;
		default:
			SDL_Log("Unknown operand for CMP");
			*quit = true;
	}
	gb_cpu_regs.pc = gb_cpu_regs.pc + 1;
	gb_cpu_state = CPU_STATE::FETCH_OPCODE;
}