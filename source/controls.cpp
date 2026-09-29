#include "controls.hpp"

controller::controller(memory* gb_mem_location, bool* process_quit) {
	quit = process_quit;
	gb_memory = gb_mem_location;
}

void controller::read_input() {
	SDL_Event e; 
	SDL_PollEvent(&e);
 
	if( e.type == SDL_QUIT ) { 
		*quit = true;
	}
}
