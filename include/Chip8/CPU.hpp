#pragma once

#include <array>
#include <cstdint>

namespace ks {
	struct CPU {
		std::array<bool, 16> keys{};
		int8_t key{ -1 };
		bool halted{};

		struct Stack {
			std::array<uint16_t, 16> memory{};
			uint8_t sp{};

			auto push(const uint16_t value) -> void {
				if (sp == 16) return;
				memory[sp++] = value;
			}
			auto pop() -> uint16_t {
				if (sp == 0) return 0;
				return memory[--sp];
			}
		} stack;

		struct Registers {
			std::array<uint8_t, 16> V{};
			uint16_t PC{};
			uint16_t I{};
			uint8_t delay{};
			uint8_t sound{};

			auto set_register(const uint8_t index, const uint8_t value) -> void {
				V[index] = value;
			}
			auto get_register(const uint8_t index) -> uint8_t {
				return V[index];
			}
		} registers;
	};
}