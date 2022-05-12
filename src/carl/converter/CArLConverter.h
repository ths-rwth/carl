/**
 * @file CArLConverter.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <carl/numbers/numbers.h>

namespace carl {

class CArLConverter {
public:
#ifdef USE_CLN_NUMBERS
	mpq_class toGMP(const cln::cl_RA& n) {
		std::stringstream ss1;
		ss1 << carl::getDenom(n);
		mpz_class denom(ss1.str());
		std::stringstream ss2;
		ss2 << carl::getNum(n);
		mpz_class num(ss2.str());
		return carl::quotient(num, denom);
	}
#endif
};

}
