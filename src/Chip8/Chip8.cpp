#include "Chip8/Chip8.hpp"

#include <iostream>
#include <format>
#include <fstream>
#include <string_view>
#include <random>

namespace {
	static auto random(int minValue, int maxValue) -> int {
		static std::random_device rd;
		static std::mt19937_64 mt(rd());

		std::uniform_int_distribution<int> dist(minValue, maxValue);
		return dist(mt);
	}
}

namespace ks {
	Chip8::Chip8() {
		m_cpu.halted = 1;
	}

	auto Chip8::load_program(const fs::path& path) -> bool {
		std::memset(m_RAM.data(), 0, m_RAM.size());
		m_displayMemory = {};

		if (!fs::exists(path)) {
			return 0;
		}

		if (fs::file_size(path) > m_RAM.size()) {
			return 0;
		}

		std::ifstream file(path, std::ios::binary);
		if (!file) {
			return 0;
		}

		file.read(reinterpret_cast<char*>(m_RAM.data() + 0x200), 3584);
		file.close();

		static constexpr std::array<uint8_t, 80> font{
			0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
			0x20, 0x60, 0x20, 0x20, 0x70, // 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
			0x90, 0x90, 0xF0, 0x10, 0x10, // 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
			0xF0, 0x10, 0x20, 0x40, 0x40, // 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
			0xF0, 0x90, 0xF0, 0x90, 0x90, // A
			0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
			0xF0, 0x80, 0x80, 0x80, 0xF0, // C
			0xE0, 0x90, 0x90, 0x90, 0xE0, // D
			0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
			0xF0, 0x80, 0xF0, 0x80, 0x80  // F
		};
		std::memcpy(m_RAM.data() + 0x50, font.data(), font.size());

		reset();
		return 1;
	}
	auto Chip8::reset() -> void {
		m_cpu = {};
		m_cpu.registers.PC = 0x200;
	}
	auto Chip8::update(const ks::KeyboardInput& keyboard) -> void {
		if (!m_releaseIt) {
			if (!m_settings.changeKeypad) {
				switch (keyboard.get_held_key()) {
				case SDLK_X: m_cpu.key = 0; break;
				case SDLK_1: m_cpu.key = 1; break;
				case SDLK_2: m_cpu.key = 2; break;
				case SDLK_3: m_cpu.key = 3; break;
				case SDLK_Q: m_cpu.key = 4; break;
				case SDLK_W: m_cpu.key = 5; break;
				case SDLK_E: m_cpu.key = 6; break;
				case SDLK_A: m_cpu.key = 7; break;
				case SDLK_S: m_cpu.key = 8; break;
				case SDLK_D: m_cpu.key = 9; break;
				case SDLK_Z: m_cpu.key = 10; break;
				case SDLK_C: m_cpu.key = 11; break;
				case SDLK_4: m_cpu.key = 12; break;
				case SDLK_R: m_cpu.key = 13; break;
				case SDLK_F: m_cpu.key = 14; break;
				case SDLK_V: m_cpu.key = 15; break;
				default: m_cpu.key = -1; break;
				}
			}
			else {
				switch (keyboard.get_held_key()) {
				case SDLK_1: m_cpu.key = 0; break;
				case SDLK_2: m_cpu.key = 1; break;
				case SDLK_3: m_cpu.key = 2; break;
				case SDLK_4: m_cpu.key = 3; break;
				case SDLK_Q: m_cpu.key = 4; break;
				case SDLK_W: m_cpu.key = 5; break;
				case SDLK_E: m_cpu.key = 6; break;
				case SDLK_R: m_cpu.key = 7; break;
				case SDLK_A: m_cpu.key = 8; break;
				case SDLK_S: m_cpu.key = 9; break;
				case SDLK_D: m_cpu.key = 10; break;
				case SDLK_F: m_cpu.key = 11; break;
				case SDLK_Z: m_cpu.key = 12; break;
				case SDLK_X: m_cpu.key = 13; break;
				case SDLK_C: m_cpu.key = 14; break;
				case SDLK_V: m_cpu.key = 15; break;
				default: m_cpu.key = -1; break;
				}
			}
		}
		else {
			if (keyboard.get_held_key() == -1) {
				m_released = 1;
				m_releaseIt = 0;
			}
		}

		if (!m_settings.changeKeypad) {
			m_cpu.keys[0] = keyboard.is_key_held(SDLK_X);
			m_cpu.keys[1] = keyboard.is_key_held(SDLK_1);
			m_cpu.keys[2] = keyboard.is_key_held(SDLK_2);
			m_cpu.keys[3] = keyboard.is_key_held(SDLK_3);
			m_cpu.keys[4] = keyboard.is_key_held(SDLK_Q);
			m_cpu.keys[5] = keyboard.is_key_held(SDLK_W);
			m_cpu.keys[6] = keyboard.is_key_held(SDLK_E);
			m_cpu.keys[7] = keyboard.is_key_held(SDLK_A);
			m_cpu.keys[8] = keyboard.is_key_held(SDLK_S);
			m_cpu.keys[9] = keyboard.is_key_held(SDLK_D);
			m_cpu.keys[10] = keyboard.is_key_held(SDLK_Z);
			m_cpu.keys[11] = keyboard.is_key_held(SDLK_C);
			m_cpu.keys[12] = keyboard.is_key_held(SDLK_4);
			m_cpu.keys[13] = keyboard.is_key_held(SDLK_R);
			m_cpu.keys[14] = keyboard.is_key_held(SDLK_F);
			m_cpu.keys[15] = keyboard.is_key_held(SDLK_V);
		}
		else {
			m_cpu.keys[0] = keyboard.is_key_held(SDLK_1);
			m_cpu.keys[1] = keyboard.is_key_held(SDLK_2);
			m_cpu.keys[2] = keyboard.is_key_held(SDLK_3);
			m_cpu.keys[3] = keyboard.is_key_held(SDLK_4);
			m_cpu.keys[4] = keyboard.is_key_held(SDLK_Q);
			m_cpu.keys[5] = keyboard.is_key_held(SDLK_W);
			m_cpu.keys[6] = keyboard.is_key_held(SDLK_E);
			m_cpu.keys[7] = keyboard.is_key_held(SDLK_R);
			m_cpu.keys[8] = keyboard.is_key_held(SDLK_A);
			m_cpu.keys[9] = keyboard.is_key_held(SDLK_S);
			m_cpu.keys[10] = keyboard.is_key_held(SDLK_D);
			m_cpu.keys[11] = keyboard.is_key_held(SDLK_F);
			m_cpu.keys[12] = keyboard.is_key_held(SDLK_Z);
			m_cpu.keys[13] = keyboard.is_key_held(SDLK_X);
			m_cpu.keys[14] = keyboard.is_key_held(SDLK_C);
			m_cpu.keys[15] = keyboard.is_key_held(SDLK_V);

		}

		if (m_cpu.halted) return;

		const uint16_t opcode = fetch();
		const Instruction instruction = decode(opcode);
		execute(instruction);

		m_tick++;
		if (m_tick >= 9) {
			m_tick = 0;

			if (m_cpu.registers.delay > 0) m_cpu.registers.delay--;
			if (m_cpu.registers.sound > 0) m_cpu.registers.sound--;

			if (m_cpu.registers.sound != 0) {
				m_playSound = 1;
			}
			else {
				m_playSound = 0;
			}
		}
	}
	auto Chip8::fetch() -> uint16_t {
		const uint16_t instruction = m_RAM[m_cpu.registers.PC] << 8 | m_RAM[m_cpu.registers.PC + 1];
		m_cpu.registers.PC = (m_cpu.registers.PC + 2) & 0xFFF;
		return instruction;
	}
	auto Chip8::decode(const uint16_t opcode) -> Instruction {
		const uint8_t n1 = (opcode >> 12) & 0xF;
		const uint8_t n2 = (opcode >> 8) & 0xF;
		const uint8_t n3 = (opcode >> 4) & 0xF;
		const uint8_t n4 = (opcode) & 0xF;
		const uint8_t n34 = opcode & 0xFF;

		Instruction instruction;
		instruction.type = static_cast<InstructionType>(n1);
		instruction.vx = n2;
		instruction.vy = n3;
		instruction.address = opcode & 0x0FFF;

		if (instruction.type == InstructionType::ZERO && n2 == 0) {
			instruction.zeroType = static_cast<ZeroType>(n34);
		}
		else if (instruction.type == InstructionType::ARITHMETIC) {
			instruction.arithmeticType = static_cast<ArithmeticType>(n4);
		}
		else if (instruction.type == InstructionType::MISC) {
			instruction.miscType = static_cast<MiscType>(n34);
		}
		else if (instruction.type == InstructionType::KEY) {
			instruction.keyType = static_cast<KeyType>(n34);
		}
		else {
			instruction.literal = n34;
		}

		return instruction;
	}
	auto Chip8::execute(const Instruction instruction) -> void {
		auto print_warning = [](const std::string_view type, const uint8_t value) -> void {
			std::cerr << std::format("[{}] Unknown instruction {:#04x}.\n", type, value);
		};

		auto execute_zero = [&](const Instruction instruction) -> void {
			switch (instruction.zeroType) {
				using enum ZeroType;
			case NOT_IMPORTANT: break;
			case CLEAR:
				m_displayMemory = {};
				break;
			case RET:
				m_cpu.registers.PC = m_cpu.stack.pop();
				break;
			default:
				// ignore 0NNN
				break;
			}
			};
		auto execute_arithmetic = [&](const Instruction instruction) -> void {
			const uint8_t vxValue = m_cpu.registers.get_register(instruction.vx);
			const uint8_t vyValue = m_cpu.registers.get_register(instruction.vy);
			switch (instruction.arithmeticType) {
				using enum ArithmeticType;
			case SET_VX_TO_VY:
				m_cpu.registers.set_register(instruction.vx, vyValue);
				break;
			case OR_VX_WITH_VY:
				m_cpu.registers.set_register(instruction.vx, vxValue | vyValue);
				break;
			case AND_VX_WITH_VY:
				m_cpu.registers.set_register(instruction.vx, vxValue & vyValue);
				break;
			case XOR_VX_WITH_VY:
				m_cpu.registers.set_register(instruction.vx, vxValue ^ vyValue);
				break;
			case ADD_VY_TO_VX: {
				const uint16_t sum = static_cast<uint16_t>(vxValue) + static_cast<uint16_t>(vyValue);
				m_cpu.registers.set_register(instruction.vx, sum & 0xFF);
				if (sum > 255) {
					m_cpu.registers.V[0xF] = 1;
				}
				else {
					m_cpu.registers.V[0xF] = 0;
				}
			}	break;
			case SUB_VY_FROM_VX: {
				m_cpu.registers.set_register(instruction.vx, vxValue - vyValue);
				if (vxValue >= vyValue) {
					m_cpu.registers.V[0xF] = 1;
				}
				else {
					m_cpu.registers.V[0xF] = 0;
				}
			}	break;
			case SR_VX_BY_VY:
				if (m_settings.putVYintoVXbeforeShift) {
					m_cpu.registers.set_register(instruction.vx, vyValue >> 1);
					m_cpu.registers.V[0xF] = vyValue & 0x1;
				}
				else {
					m_cpu.registers.set_register(instruction.vx, vxValue >> 1);
					m_cpu.registers.V[0xF] = vxValue & 0x1;
				}
				break;
			case SUB_VX_FROM_VY:
				m_cpu.registers.set_register(instruction.vx, vyValue - vxValue);
				if (vyValue >= vxValue) {
					m_cpu.registers.V[0xF] = 1;
				}
				else {
					m_cpu.registers.V[0xF] = 0;
				}
				break;
			case SL_VX_BY_VY:
				if (m_settings.putVYintoVXbeforeShift) {
					m_cpu.registers.set_register(instruction.vx, vyValue << 1);
					m_cpu.registers.V[0xF] = (vyValue & 0x80) >> 7;
				}
				else {
					m_cpu.registers.set_register(instruction.vx, vxValue << 1);
					m_cpu.registers.V[0xF] = (vxValue & 0x80) >> 7;
				}
				break;
			default:
				print_warning("ARITHMETIC", static_cast<uint8_t>(instruction.arithmeticType));
				break;
			}
			};
		auto execute_key = [&](const Instruction instruction) -> void {
			const uint8_t vxValue = m_cpu.registers.get_register(instruction.vx) & 0xF;
			switch (instruction.keyType) {
				using enum KeyType;
			case KEY_PRESSED:
				if (m_cpu.keys[vxValue]) m_cpu.registers.PC += 2;
				break;
			case KEY_NOT_PRESSED:
				if (!m_cpu.keys[vxValue]) m_cpu.registers.PC += 2;
				break;
			default:
				print_warning("KEY", static_cast<uint8_t>(instruction.keyType));
				break;
			}
			};
		auto execute_misc = [&](const Instruction instruction) -> void {
			switch (instruction.miscType) {
				using enum MiscType;
			case SET_VX_TO_DELAY:
				m_cpu.registers.set_register(instruction.vx, m_cpu.registers.delay);
				break;
			case WAIT_FOR_KEYPRESS:
				if (m_cpu.key == -1) m_cpu.registers.PC -= 2;
				else {
					if (m_released) {
						m_cpu.registers.set_register(instruction.vx, m_cpu.key);
						m_released = 0;
					}
					else {
						m_releaseIt = 1;
						m_cpu.registers.PC -= 2;
					}
				}
				break;
			case SET_DELAY_TO_VX:
				m_cpu.registers.delay = m_cpu.registers.get_register(instruction.vx);
				break;
			case SET_SOUND_TO_VX:
				m_cpu.registers.sound = m_cpu.registers.get_register(instruction.vx);
				break;
			case ADD_VX_TO_I:
				m_cpu.registers.I += m_cpu.registers.get_register(instruction.vx);
				if (m_cpu.registers.I > 0xFFF) {
					// Spacefight 2091! relies on this behavior.
					m_cpu.registers.V[0xF] = 1;
				}
				break;
			case SET_I_TO_HEX_CHARACTER:
				m_cpu.registers.I = 0x50 + (m_cpu.registers.get_register(instruction.vx) & 0xF) * 5;
				break;
			case BCD_VX:
				m_RAM[m_cpu.registers.I] = m_cpu.registers.get_register(instruction.vx) / 100;
				m_RAM[m_cpu.registers.I + 1] = (m_cpu.registers.get_register(instruction.vx) / 10) % 10;
				m_RAM[m_cpu.registers.I + 2] = m_cpu.registers.get_register(instruction.vx) % 10;
				break;
			case SAVE_VX:
				for (int i = 0; i <= instruction.vx; i++) {
					m_RAM[m_cpu.registers.I + i] = m_cpu.registers.V[i];
				}
				if (m_settings.changeValueOfI) {
					m_cpu.registers.I += instruction.vx + 1;
				}
				break;
			case LOAD_VX:
				for (int i = 0; i <= instruction.vx; i++) {
					m_cpu.registers.V[i] = m_RAM[m_cpu.registers.I + i];
				}
				if (m_settings.changeValueOfI) {
					m_cpu.registers.I += instruction.vx + 1;
				}
				break;
			default:
				print_warning("MISC", static_cast<uint8_t>(instruction.miscType));
				break;
			}
			};

		switch (instruction.type) {
			using enum InstructionType;
		case ZERO:
			execute_zero(instruction);
			break;
		case JP:
			m_cpu.registers.PC = instruction.address;
			break;
		case CALL:
			m_cpu.stack.push(m_cpu.registers.PC);
			m_cpu.registers.PC = instruction.address;
			break;
		case SKIP_VX_EQ_NN:
			if (m_cpu.registers.get_register(instruction.vx) == instruction.literal) {
				m_cpu.registers.PC += 2;
			}
			break;
		case SKIP_VX_NEQ_NN:
			if (m_cpu.registers.get_register(instruction.vx) != instruction.literal) {
				m_cpu.registers.PC += 2;
			}
			break;
		case SKIP_VX_NEQ_VY:
			if (m_cpu.registers.get_register(instruction.vx) == m_cpu.registers.get_register(instruction.vy)) {
				m_cpu.registers.PC += 2;
			}
			break;
		case VX_SET:
			m_cpu.registers.set_register(instruction.vx, instruction.literal);
			break;
		case VX_ADD:
			m_cpu.registers.set_register(
				instruction.vx,
				m_cpu.registers.get_register(instruction.vx) + instruction.literal);
			break;
		case ARITHMETIC:
			execute_arithmetic(instruction);
			break;
		case SKIP_VX_EQ_VY:
			if (m_cpu.registers.get_register(instruction.vx) != m_cpu.registers.get_register(instruction.vy)) {
				m_cpu.registers.PC += 2;
			}
			break;
		case SET_I:
			m_cpu.registers.I = instruction.address;
			break;
		case JR:
			if (m_settings.useVXinsteadOfV0) {
				m_cpu.registers.PC = instruction.address + m_cpu.registers.V[instruction.vx];
			}
			else {
				m_cpu.registers.PC = instruction.address + m_cpu.registers.V[0];
			}
			break;
		case RANDOM:
			m_cpu.registers.set_register(instruction.vx, ::random(0, 255) & instruction.literal);
			break;
		case DRAW: {
			const uint8_t x = m_cpu.registers.get_register(instruction.vx) & (DISPLAY_X - 1);
			const uint8_t y = m_cpu.registers.get_register(instruction.vy) & (DISPLAY_Y - 1);
			m_cpu.registers.V[0xF] = 0;

			for (int i = 0; i < (instruction.literal & 0xF); i++) {
				const uint8_t b = m_RAM[m_cpu.registers.I + i];
				for (int j = 0; j < 8; j++) {
					uint8_t rx = x + j;
					uint8_t ry = y + i;
					if (m_settings.clipping) {
						if (rx >= DISPLAY_X || ry >= DISPLAY_Y) break;
					}
					else {
						rx &= (DISPLAY_X - 1);
						ry &= (DISPLAY_Y - 1);
					}
					const bool bitIndex = ((0x80u >> j) & b) >> (7 - j);
					if (bitIndex) {
						if (m_displayMemory[rx][ry]) {
							m_cpu.registers.V[0xF] = 1;
						}
						m_displayMemory[rx][ry] ^= bitIndex;
					}
				}
			}
		}	break;
		case KEY:
			execute_key(instruction);
			break;
		case MISC:
			execute_misc(instruction);
			break;
		default:
			print_warning("DEFAULT", static_cast<uint8_t>(instruction.type));
			break;
		}
	}
}