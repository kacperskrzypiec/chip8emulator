#include "App.hpp"

int main(int argc, char* argv[]) {
	SDL_SetAppMetadata("Chip8Emulator", "1.0.0", 0);
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	TTF_Init();

	{
		ks::App app("Chip8Emulator", 1280, 720);
		app.run();
	}

	TTF_Quit();
	SDL_Quit();
}