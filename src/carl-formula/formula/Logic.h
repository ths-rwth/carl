#pragma once

#include <iostream>

namespace carl {

enum class Logic {
	QF_BV, QF_IDL, QF_LIA, QF_LIRA, QF_LRA, QF_NIA, QF_NIRA, QF_NRA, QF_PB, QF_RDL, QF_UF, NRA, LRA, UNDEFINED
};

inline std::ostream& operator<<(std::ostream& os, const Logic& l) {
	switch (l) {
		case Logic::QF_BV:		os << "QF_BV"; break;
		case Logic::QF_IDL:		os << "QF_IDL"; break;
		case Logic::QF_LIA:		os << "QF_LIA"; break;
		case Logic::QF_LIRA:	os << "QF_LIRA"; break;
		case Logic::QF_LRA:		os << "QF_LRA"; break;
		case Logic::QF_NIA:		os << "QF_NIA"; break;
		case Logic::QF_NIRA:	os << "QF_NIRA"; break;
		case Logic::QF_NRA:		os << "QF_NRA"; break;
		case Logic::QF_PB:		os << "QF_PB"; break;
		case Logic::QF_RDL:		os << "QF_RDL"; break;
		case Logic::QF_UF:		os << "QF_UF"; break;
		case Logic::NRA:		os << "NRA"; break;
		case Logic::LRA:		os << "LRA"; break;
		case Logic::UNDEFINED:	os << "undefined"; break;
	}
	return os;
}	

}
