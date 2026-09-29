#include <SDL.h>
#pragma once
class memory {
private:
	uint8_t gb_memory[65535];
public:
	void test();
	void output_memory(uint16_t location, uint16_t length);
	void write_buffer(char * buffer, size_t size, uint8_t location);
	uint8_t read_byte(uint16_t location);
	void write_byte(uint16_t location, uint8_t byte);
	//char * read_buffer(uint16_t location);
};