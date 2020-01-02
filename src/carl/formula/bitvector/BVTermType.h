#pragma once

#include <carl-logging/carl-logging.h>

#include <iostream>
#include <type_traits>

namespace carl {

	enum class BVTermType {
		CONSTANT,
		VARIABLE,
		CONCAT, EXTRACT,
		NOT,
		NEG,
		AND, OR, XOR, NAND, NOR, XNOR,
		ADD, SUB, MUL, DIV_U, DIV_S, MOD_U, MOD_S1, MOD_S2,
		EQ,
		LSHIFT, RSHIFT_LOGIC, RSHIFT_ARITH,
		LROTATE, RROTATE,
		EXT_U, EXT_S,
		REPEAT
	};

	inline auto typeId(BVTermType type) {
		return static_cast<std::underlying_type<BVTermType>::type>(type);
	}

	inline std::ostream& operator<<(std::ostream& os, BVTermType type) {
		switch(type) {
			case BVTermType::CONSTANT: return os << "CONSTANT";
			case BVTermType::VARIABLE: return os << "VARIABLE";
			case BVTermType::CONCAT: return os << "concat";
			case BVTermType::EXTRACT: return os << "extract";
			case BVTermType::NOT: return os << "bvnot";
			case BVTermType::NEG: return os << "bvneg";
			case BVTermType::AND: return os << "bvand";
			case BVTermType::OR: return os << "bvor";
			case BVTermType::XOR: return os << "bvxor";
			case BVTermType::NAND: return os << "bvnand";
			case BVTermType::NOR: return os << "bvnor";
			case BVTermType::XNOR: return os << "bvxnor";
			case BVTermType::ADD: return os << "bvadd";
			case BVTermType::SUB: return os << "bvsub";
			case BVTermType::MUL: return os << "bvmul";
			case BVTermType::DIV_U: return os << "bvudiv";
			case BVTermType::DIV_S: return os << "bvsdiv";
			case BVTermType::MOD_U: return os << "bvurem";
			case BVTermType::MOD_S1: return os << "bvsrem";
			case BVTermType::MOD_S2: return os << "bvsmod";
			case BVTermType::EQ: return os << "bvcomp";
			case BVTermType::LSHIFT: return os << "bvshl";
			case BVTermType::RSHIFT_LOGIC: return os << "bvlshr";
			case BVTermType::RSHIFT_ARITH: return os << "bvashr";
			case BVTermType::LROTATE: return os << "rotate_left";
			case BVTermType::RROTATE: return os << "rotate_right";
			case BVTermType::EXT_U: return os << "zero_extend";
			case BVTermType::EXT_S: return os << "sign_extend";
			case BVTermType::REPEAT: return os << "repeat";
			default:
				CARL_LOG_ERROR("carl.formula.bv", "Tried to print an unknown value for BVTermType: " << typeId(type));
				return os << "unknown";
		}
	}

	inline bool typeIsUnary(BVTermType type) {
		return (
			type == BVTermType::NOT || type == BVTermType::NEG || type == BVTermType::LROTATE
			|| type == BVTermType::RROTATE || type == BVTermType::REPEAT || type == BVTermType::EXT_U
			|| type == BVTermType::EXT_S
		);
	}

	inline bool typeIsBinary(BVTermType type) {
		return (
			type == BVTermType::CONCAT || type == BVTermType::AND || type == BVTermType::OR
			|| type == BVTermType::XOR || type == BVTermType::NAND || type == BVTermType::NOR
			|| type == BVTermType::XNOR || type == BVTermType::ADD || type == BVTermType::SUB
			|| type == BVTermType::MUL || type == BVTermType::DIV_U || type == BVTermType::DIV_S
			|| type == BVTermType::MOD_U || type == BVTermType::MOD_S1 || type == BVTermType::MOD_S2
			|| type == BVTermType::EQ || type == BVTermType::LSHIFT || type == BVTermType::RSHIFT_LOGIC
			|| type == BVTermType::RSHIFT_ARITH
		);
	}
}