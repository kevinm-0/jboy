#include "main.hpp"

int main( int argc, char* args[] ) {

	
	SDL_Window* window = NULL;

	SDL_Surface* screenSurface = NULL;

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		SDL_Log( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	} else {
		window = SDL_CreateWindow( "juliaboy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if (window == NULL) {
			SDL_Log( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		} else {
			bool game_quit = false;
			
			memory gb_memory;
			controller gb_controls(&gb_memory, &game_quit);
			cpu gb_cpu(&gb_memory, &game_quit);
			
			while(game_quit == false) {
				gb_controls.read_input();
				int m_cycle;
				
				for (m_cycle = 0; m_cycle < M_CYCLES_PER_FRAME; ++m_cycle) {
					gb_cpu.tick();
					if (game_quit == true) break;
				}
				
			}
		}
	}

	SDL_DestroyWindow( window );

	SDL_Quit();
	
	return 0;
}

