#include "memory.hpp"

void memory::write_buffer(char * buffer, size_t size, uint8_t location) {
	uint8_t *mem_ptr = &gb_memory[location];
	SDL_memcpy(mem_ptr, buffer, size);
}

void memory::test() {
	SDL_Log("Hello Test\n");
}

void memory::output_memory(uint16_t location, uint16_t length) {
	int i;
	for (i = location; i < length; ++i)
    {
        SDL_Log(" %02x", gb_memory[i]);
    }
}

uint8_t memory::read_byte(uint16_t location) {
	return gb_memory[location];
}

void memory::write_byte(uint16_t location, uint8_t byte) {
	gb_memory[location] = byte;
}