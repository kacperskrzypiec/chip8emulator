#pragma once

#include <SDL3/SDL.h>

namespace ks {
	class Window {
	public:
		Window(const char* title, int w, int h, SDL_WindowFlags flags) {
			m_window = SDL_CreateWindow(title, w, h, flags);
			if (!m_window) return;

			m_renderer = SDL_CreateRenderer(m_window, nullptr);
			if (!m_renderer) return;

			m_width = static_cast<float>(w);
			m_height = static_cast<float>(h);

			m_isOpen = 1;
		}
		~Window() {
			if (m_renderer) {
				SDL_DestroyRenderer(m_renderer);
			}
			if (m_window) {
				SDL_DestroyWindow(m_window);
			}
		}

		auto on_event(SDL_Event& event) -> void {
			if (event.type == SDL_EVENT_WINDOW_RESIZED) {
				m_width = static_cast<float>(event.window.data1);
				m_height = static_cast<float>(event.window.data2);
			}
		}

		auto close() -> void {
			m_isOpen = 0;
		}

		auto is_open() const -> bool {
			return m_isOpen;
		}

		auto get_width() const -> float {
			return m_width;
		}
		auto get_height() const -> float {
			return m_height;
		}

		auto get_window() -> SDL_Window* {
			return m_window;
		}
		auto get_renderer() -> SDL_Renderer* {
			return m_renderer;
		}

		operator SDL_Window*() {
			return m_window;
		}
		operator SDL_Renderer* () {
			return m_renderer;
		}

	private:
		SDL_Window* m_window{};
		SDL_Renderer* m_renderer{};
		float m_width{};
		float m_height{};
		bool m_isOpen{};
	};
}