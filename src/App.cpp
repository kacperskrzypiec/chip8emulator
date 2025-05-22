#include "App.hpp"

#include <sstream>
#include <iostream>

namespace ks {
	constexpr static int AUDIO_STREAM_FREQUENCY = 8000;
	constexpr static int PULSE_FREQUENCY = 1000;

	App::App(const std::string_view title, const int width, const int height)
		:	m_window("Chip8Emulator", 640, 480, SDL_WINDOW_RESIZABLE) 
	{
		m_gameDisplay = SDL_CreateTexture(m_window, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, Chip8::DISPLAY_X, Chip8::DISPLAY_Y);
		SDL_SetTextureScaleMode(m_gameDisplay, SDL_SCALEMODE_NEAREST);
		SDL_SetRenderDrawBlendMode(m_window, SDL_BLENDMODE_BLEND);

		SDL_SetEventEnabled(SDL_EVENT_DROP_FILE, 1);

		m_display.resize(Chip8::DISPLAY_X);
		for (int x = 0; x < Chip8::DISPLAY_X; x++) {
			m_display[x].resize(Chip8::DISPLAY_Y);
			for (int y = 0; y < Chip8::DISPLAY_Y; y++) {
				m_display[x][y].decay(20.0f);
			}
		}

		SDL_AudioSpec spec;
		spec.format = SDL_AUDIO_F32;
		spec.channels = 1;
		spec.freq = AUDIO_STREAM_FREQUENCY;
		m_audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
		SDL_ResumeAudioStreamDevice(m_audioStream);
		
		m_textEngine = TTF_CreateRendererTextEngine(m_window);
		m_font = TTF_OpenFont(DATA_PATH "arial.ttf", 18);
		m_text = TTF_CreateText(m_textEngine, m_font, "", 0);
	}
	App::~App() {
		TTF_CloseFont(m_font);
		TTF_DestroyRendererTextEngine(m_textEngine);
		SDL_DestroyTexture(m_gameDisplay);
		SDL_PauseAudioStreamDevice(m_audioStream);
		SDL_DestroyAudioStream(m_audioStream);
	}

	auto App::run() -> void {
		int64_t last = SDL_GetTicksNS();

		while (m_window.is_open()) {
			const int64_t start = SDL_GetTicksNS();
			handle_events();

			const float deltaTime = [&]() -> float {
				const float delta = (SDL_GetTicksNS() - last) / 1'000'000'000.0f;
				return delta < 2.0f ? delta : 2.0f;
			}();
			last = SDL_GetTicksNS();

			input(deltaTime);
			update(deltaTime);
			render();

			const int64_t diff = start - SDL_GetTicksNS();
			SDL_DelayPrecise(1'000'000'000 / 60 - diff);
		}
	}
	auto App::handle_events() -> void {
		m_keyboard.pre_event();
		while (SDL_PollEvent(&m_event)) {
			m_keyboard.on_event(m_event);
			m_window.on_event(m_event);
			switch (m_event.type) {
			case SDL_EVENT_QUIT:
				m_window.close();
				break;
			case SDL_EVENT_DROP_FILE:
				m_romPath = m_event.drop.data;
				reload();
				break;
			}
		}
	}
	auto App::input(const float deltaTime) -> void {
		if (m_keyboard.is_key_pressed_once(SDLK_F6)) {
			reload();
		}
		if (m_keyboard.is_key_pressed_once(SDLK_ESCAPE)) {
			m_paused = !m_paused;
		}

		if (m_paused) {
			Chip8::Settings settings = m_chip8.get_settings();

			if (m_keyboard.is_key_pressed_once(SDLK_F1)) {
				settings.putVYintoVXbeforeShift = !settings.putVYintoVXbeforeShift;
			}
			if (m_keyboard.is_key_pressed_once(SDLK_F2)) {
				settings.useVXinsteadOfV0 = !settings.useVXinsteadOfV0;
			}
			if (m_keyboard.is_key_pressed_once(SDLK_F3)) {
				settings.changeValueOfI = !settings.changeValueOfI;
			}
			if (m_keyboard.is_key_pressed_once(SDLK_F4)) {
				settings.clipping = !settings.clipping;
			}
			if (m_keyboard.is_key_pressed_once(SDLK_F5)) {
				settings.changeKeypad = !settings.changeKeypad;
			}

			m_chip8.set_settings(settings);
		}
	}
	auto App::update(const float deltaTime) -> void {
		static constexpr float tick = 1.0f / 531.0f;

		if (!m_paused) {
			m_accumulator += deltaTime;
			while (m_accumulator >= tick / m_simulationSpeed) {
				m_chip8.update(m_keyboard);
				m_accumulator -= tick / m_simulationSpeed;
			}

			const auto& displayMemory = m_chip8.get_display_memory();
			for (int x = 0; x < Chip8::DISPLAY_X; x++) {
				for (int y = 0; y < Chip8::DISPLAY_Y; y++) {
					m_display[x][y] = displayMemory[x][y];
					m_display[x][y].update(deltaTime);
				}
			}

			if (m_chip8.should_play_sound()) {
				play_sine_wave();
			} 
			else {
				SDL_ClearAudioStream(m_audioStream);
			}
		}

		Uint32* pixels{};
		int pitch{};
		int format{};
		SDL_LockTexture(m_gameDisplay, nullptr, reinterpret_cast<void**>(&pixels), &pitch);

		for (int x = 0; x < Chip8::DISPLAY_X; x++) {
			for (int y = 0; y < Chip8::DISPLAY_Y; y++) {
				const Uint32 pixelPosition = y * (pitch / sizeof(unsigned int)) + x;
				const uint8_t r = 0;
				const uint8_t g = static_cast<uint8_t>(255.0f * m_display[x][y] + 10.0f * (1.0f - m_display[x][y]));
				const uint8_t b = static_cast<uint8_t>(51.0f * m_display[x][y] + 2.0f * (1.0f - m_display[x][y]));

				pixels[pixelPosition] = 0xFF << 24 | b << 16 | g << 8 | r;
			}
		}

		SDL_UnlockTexture(m_gameDisplay);

		if (m_paused) {
			const Chip8::Settings& settings = m_chip8.get_settings();
			std::stringstream ss;
			ss << "[F1] Put VY into VX before shift: " << (settings.putVYintoVXbeforeShift ? "ON" : "OFF")
				<< "\n[F2] Use VX instead of V0: " << (settings.useVXinsteadOfV0 ? "ON" : "OFF")
				<< "\n[F3] Change value of I: " << (settings.changeValueOfI ? "ON" : "OFF")
				<< "\n[F4] Clipping: " << (settings.clipping ? "ON" : "OFF")
				<< "\n[F5] Change keypad: " << (settings.changeKeypad ? "ON" : "OFF")
				<< "\n\n[F6] Reload ROM";

			TTF_SetTextString(m_text, ss.str().c_str(), 0);
		}
	}
	auto App::render() -> void {
		SDL_RenderClear(m_window);
		
		const float scaleX = std::floorf(m_window.get_width() / Chip8::DISPLAY_X);
		const float scaleY = std::floorf(m_window.get_height() / Chip8::DISPLAY_Y);
		const float scale = std::min(scaleX, scaleY);

		const float sizeX = Chip8::DISPLAY_X * scale;
		const float sizeY = Chip8::DISPLAY_Y * scale;
		const float offsetX = (m_window.get_width() - sizeX) / 2;
		const float offsetY = (m_window.get_height() - sizeY) / 2;

		const SDL_FRect srcRect{ 0.0f, 0.0f, static_cast<float>(Chip8::DISPLAY_X), static_cast<float>(Chip8::DISPLAY_Y) };
		const SDL_FRect dstRect{ offsetX, offsetY, sizeX, sizeY };
		SDL_RenderTexture(m_window, m_gameDisplay, &srcRect, &dstRect);

		if (m_paused) {
			SDL_SetRenderDrawColor(m_window, 0, 0, 0, 150);
			SDL_RenderFillRect(m_window, nullptr);
			TTF_DrawRendererText(m_text, 10, 10);
		}

		SDL_SetRenderDrawColor(m_window, 0, 0, 0, 255);
		SDL_RenderPresent(m_window);
	}

	auto App::play_sine_wave() -> void {
		const int minimumSize = AUDIO_STREAM_FREQUENCY * sizeof(float) / 2.0f;
		if (SDL_GetAudioStreamQueued(m_audioStream) < minimumSize) {
			std::array<float, 1024> samples;

			for (int i = 0; i < samples.size(); i++) {
				const float phase = static_cast<float>(m_currentSineSample * PULSE_FREQUENCY) / AUDIO_STREAM_FREQUENCY;
				samples[i] = std::sinf(2.0f * SDL_PI_F * phase);
				m_currentSineSample++;
			}

			m_currentSineSample %= AUDIO_STREAM_FREQUENCY;

			SDL_PutAudioStreamData(m_audioStream, samples.data(), samples.size());
		}
	}

	auto App::reload() -> void {
		const std::string filename = m_romPath.filename().string();
		if (!m_chip8.load_program(m_romPath)) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", std::format("Could not load {}", filename).c_str(), m_window);
		}
		else {
			m_paused = 0;
		}
	}
}