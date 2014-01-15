#include "Sign.h"

namespace carl {

std::ostream& operator<<(std::ostream& os, const Sign& sign) {
	switch (sign) {
		case Sign::NEGATIVE: os << "NEGATIVE";
			break;
		case Sign::ZERO: os << "ZERO";
			break;
		case Sign::POSITIVE: os << "POSITIVE";
			break;
	}
	return os;
}

}