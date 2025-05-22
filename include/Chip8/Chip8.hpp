#pragma once

#include "Chip8/CPU.hpp"
#include "Chip8/Instruction.hpp"
#include "KeyboardInput.hpp"

#include <array>
#include <filesystem>

namespace fs = std::filesystem;

namespace ks {
	class Chip8 {
	public:
		static constexpr int RAM_SIZE = 4096;
		static constexpr int DISPLAY_X = 64;
		static constexpr int DISPLAY_Y = 32;

		using DisplayMemory = std::array<std::array<bool, DISPLAY_Y>, DISPLAY_X>;

		struct Settings {
			bool putVYintoVXbeforeShift{};
			bool useVXinsteadOfV0{};
			bool changeValueOfI{};
			bool clipping{ 1 };
			bool changeKeypad{};
		};

	public:
		Chip8();
		~Chip8() = default;

		auto load_program(const fs::path& path) -> bool;
		auto reset() -> void;
		auto update(const ks::KeyboardInput& keyboard) -> void;

		auto should_play_sound() const -> bool {
			return m_playSound;
		}

		auto get_display_memory() const -> const DisplayMemory& {
			return m_displayMemory;
		}
		auto get_settings() const -> const Settings& {
			return m_settings;
		}
		auto set_settings(const Settings& settings) -> void {
			m_settings = settings;
		}

	private:
		auto fetch() -> uint16_t;
		auto decode(const uint16_t opcode) -> Instruction;
		auto execute(const Instruction instruction) -> void;

	private:
		std::array<uint8_t, RAM_SIZE> m_RAM{};
		int32_t m_tick{};
		bool m_releaseIt{};
		bool m_released{};
		bool m_playSound{};

		DisplayMemory m_displayMemory{};

		CPU m_cpu;
		Settings m_settings;
	};
}