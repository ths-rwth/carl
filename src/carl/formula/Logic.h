#pragma once

#include <iostream>

namespace carl {

enum class Logic {
	QF_BV, QF_LIA, QF_LIRA, QF_LRA, QF_NIA, QF_NIRA, QF_NRA, QF_PB, QF_UF, UNDEFINED
};

inline std::ostream& operator<<(std::ostream& os, const Logic& l) {
	switch (l) {
		case Logic::QF_BV:		os << "QF_BV"; break;
		case Logic::QF_LIA:		os << "QF_LIA"; break;
		case Logic::QF_LIRA:	os << "QF_LIRA"; break;
		case Logic::QF_LRA:		os << "QF_LRA"; break;
		case Logic::QF_NIA:		os << "QF_NIA"; break;
		case Logic::QF_NIRA:	os << "QF_NIRA"; break;
		case Logic::QF_NRA:		os << "QF_NRA"; break;
		case Logic::QF_PB:		os << "QF_PB"; break;
		case Logic::QF_UF:		os << "QF_UF"; break;
		case Logic::UNDEFINED:	os << "undefined"; break;
	}
	return os;
}	

}
