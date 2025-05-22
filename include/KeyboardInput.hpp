#pragma once

#include <SDL3/SDL.h>

#include <unordered_map>

namespace ks {
	class KeyboardInput {
	public:
		KeyboardInput() = default;
		~KeyboardInput() = default;

		auto pre_event() -> void;
		auto on_event(SDL_Event& event) -> void;

		auto is_key_pressed(const SDL_Keycode key) const -> bool {
			if (!m_keyState.contains(key)) return 0;
			return m_keyState.at(key).state == KeyState::PRESSED;
		}
		auto is_key_pressed_once(const SDL_Keycode key) const -> bool {
			if (!m_keyState.contains(key)) return 0;
			return m_keyState.at(key).state == KeyState::PRESSED_ONCE;
		}
		auto is_key_held(const SDL_Keycode key) const -> bool {
			if (!m_keyState.contains(key)) return 0;
			return m_keyState.at(key).held;
		}
		auto is_key_released(const SDL_Keycode key) const -> bool {
			if (!m_keyState.contains(key)) return 0;
			return m_keyState.at(key).state == KeyState::RELEASED;
		}

		auto is_any_key_pressed() const -> bool { return m_anyKeyPressed; }
		auto is_any_key_released() const -> bool { return m_anyKeyReleased; }
		auto is_any_key_held() const -> bool { return m_anyKeyHeld; }

		auto get_pressed_key() const -> SDL_Keycode {
			return m_pressedKey;
		}
		auto get_held_key() const -> SDL_Keycode {
			return m_heldKey;
		}
		auto get_released_key() const -> SDL_Keycode {
			return m_releasedKey;
		}

	private:
		struct KeyState {
			enum State : uint8_t {
				DEFAULT = 0,
				PRESSED_ONCE,
				WAIT,
				PRESSED,
				RELEASED,
			} state{};

			bool held{};
		};

	private:

		std::unordered_map<SDL_Keycode, KeyState> m_keyState;

		bool m_anyKeyPressed{};
		bool m_anyKeyReleased{};
		bool m_anyKeyHeld{};

		SDL_Keycode m_pressedKey{ SDLK_UNKNOWN };
		SDL_Keycode m_heldKey{ SDLK_UNKNOWN };
		SDL_Keycode m_releasedKey{ SDLK_UNKNOWN };
	};
}