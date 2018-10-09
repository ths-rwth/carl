/**
 * @file CArLConverter.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../numbers/numbers.h"

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
#ifdef USE_Z3_NUMBERS
	mpz toZ3MPZ(const cln::cl_I& n) {
		mpz res;
		std::stringstream ss;
		ss << n;
		mpzmanager().set(res, ss.str().c_str());
		return res;
	}
	mpq toZ3MPQ(const cln::cl_RA& n) {
		mpz num = toZ3MPZ(getNum(n));
		mpz den = toZ3MPZ(getDenom(n));
		mpq res;
		mpqmanager().set(res, num, den);
		return res;
	}
	rational toZ3Rational(const cln::cl_RA& n) {
		return rational(toZ3MPQ(n));
	}
#endif
#endif
};

}
