#pragma once

#include "Window.hpp"
#include "KeyboardInput.hpp"
#include "Chip8/Chip8.hpp"
#include "SmoothReal.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <vector>
#include <string_view>

namespace fs = std::filesystem;

namespace ks {
	class App {
	public:
		App(const std::string_view title, const int width, const int height);
		~App();

		auto run() -> void;

	private:
		auto handle_events() -> void;
		auto input(const float deltaTime) -> void;
		auto update(const float deltaTime) -> void;
		auto render() -> void;
	
	private:
		auto reload() -> void;
		auto play_sine_wave() -> void;

	private:
		ks::Window m_window;
		ks::KeyboardInput m_keyboard;
		ks::Chip8 m_chip8;

		SDL_AudioStream* m_audioStream{};
		SDL_Texture* m_gameDisplay{};
		SDL_Event m_event{};

		TTF_Font* m_font{};
		TTF_TextEngine* m_textEngine{};
		TTF_Text* m_text{};

		std::vector<std::vector<ks::SmoothFloat>> m_display;
		
		fs::path m_romPath{};

		float m_simulationSpeed{ 1.0f };
		float m_accumulator{};
		int m_currentSineSample{};
		bool m_paused{};
	};
}