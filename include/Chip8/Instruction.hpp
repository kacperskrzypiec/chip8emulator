#pragma once

#include <cstdint>

namespace ks {
	enum class InstructionType : uint8_t {
		ZERO = 0x0,
		JP = 0x1,
		CALL = 0x2,
		SKIP_VX_EQ_NN = 0x3,
		SKIP_VX_NEQ_NN = 0x4,
		SKIP_VX_NEQ_VY = 0x5,
		VX_SET = 0x6,
		VX_ADD = 0x7,
		ARITHMETIC = 0x8,
		SKIP_VX_EQ_VY = 0x9,
		SET_I = 0xA,
		JR = 0xB,
		RANDOM = 0xC,
		DRAW = 0xD,
		KEY = 0xE,
		MISC = 0xF,
	};

	enum class ZeroType : uint8_t {
		NOT_IMPORTANT = 0x00,
		CLEAR = 0xE0,
		RET = 0xEE,
	};

	enum class ArithmeticType : uint8_t {
		SET_VX_TO_VY = 0x0,
		OR_VX_WITH_VY = 0x1,
		AND_VX_WITH_VY = 0x2,
		XOR_VX_WITH_VY = 0x3,
		ADD_VY_TO_VX = 0x4,
		SUB_VY_FROM_VX = 0x5,
		SR_VX_BY_VY = 0x6,
		SUB_VX_FROM_VY = 0x7,
		SL_VX_BY_VY = 0xE,
	};

	enum class KeyType : uint8_t {
		KEY_PRESSED = 0x9E,
		KEY_NOT_PRESSED = 0xA1,
	};

	enum class MiscType : uint8_t {
		SET_VX_TO_DELAY = 0x07,
		WAIT_FOR_KEYPRESS = 0x0A,
		SET_DELAY_TO_VX = 0x15,
		SET_SOUND_TO_VX = 0x18,
		ADD_VX_TO_I = 0x1E,
		SET_I_TO_HEX_CHARACTER = 0x29,
		BCD_VX = 0x33,
		SAVE_VX = 0x55,
		LOAD_VX = 0x65,
	};

	struct Instruction {
		InstructionType type{};
		uint16_t address{};
		uint8_t vx{};
		uint8_t vy{};

		union {
			ZeroType zeroType;
			ArithmeticType arithmeticType;
			MiscType miscType;
			KeyType keyType;
			uint8_t literal{};
		};
	};
}