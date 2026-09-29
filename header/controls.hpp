#include <SDL.h>
#include "memory.hpp"
class controller {
private:
	memory* gb_memory;
	bool* quit;
public:
	controller(memory* gb_mem_location, bool* process_quit);
	void read_input();
};