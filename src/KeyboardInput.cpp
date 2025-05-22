#include "KeyboardInput.hpp"

namespace ks {
	auto KeyboardInput::pre_event() -> void {
		m_anyKeyPressed = 0;
		m_anyKeyHeld = 0;
		m_anyKeyReleased = 0;
		m_pressedKey = SDLK_UNKNOWN;
		m_heldKey = SDLK_UNKNOWN;
		m_releasedKey = SDLK_UNKNOWN;

		for (auto& [key, keyState] : m_keyState) {
			if (keyState.state != KeyState::DEFAULT && 
				keyState.state != KeyState::RELEASED && 
				m_anyKeyHeld == 0) 
			{
				m_anyKeyHeld = 1;
				m_heldKey = key;
			}

			switch (keyState.state) {
			case KeyState::State::PRESSED_ONCE:
				keyState.state = KeyState::WAIT;
				break;
			case KeyState::State::RELEASED:
				keyState.state = KeyState::DEFAULT;
				break;
			default:
				break;
			}

			// release the key if being pressed during unfocusing the window
			const bool* rawState = SDL_GetKeyboardState(nullptr);
			if (!rawState) continue;
			if (rawState[SDL_GetScancodeFromKey(key, nullptr)]) continue;

			keyState.held = 0;

			if (keyState.state == KeyState::PRESSED_ONCE ||
				keyState.state == KeyState::WAIT ||
				keyState.state == KeyState::PRESSED
				) {
				keyState.state = KeyState::RELEASED;
			}
		}
	}

	auto KeyboardInput::on_event(SDL_Event& event) -> void {
		if (event.type == SDL_EVENT_KEY_DOWN) {
			m_anyKeyPressed = 1;
			m_pressedKey = event.key.key;
			m_keyState[event.key.key].held = 1;

			switch (m_keyState[event.key.key].state) {
			case KeyState::State::DEFAULT:
				m_keyState[event.key.key].state = KeyState::State::PRESSED_ONCE;
				break;
			case KeyState::State::WAIT:
				m_keyState[event.key.key].state = KeyState::State::PRESSED;
				break;
			default:
				break;
			}
		}
		else if (event.type == SDL_EVENT_KEY_UP) {
			m_anyKeyReleased = 1;
			m_releasedKey = event.key.key;
			m_keyState[event.key.key].held = 0;

			if (m_keyState[event.key.key].state != KeyState::State::DEFAULT) {
				m_keyState[event.key.key].state = KeyState::State::RELEASED;
			}
		}
	}
}