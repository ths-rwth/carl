/**
 * This file is for short testing - not intended for distribution.
 * @file FloatExample.cpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-16
 * @version	2013-12-16
 */

#include <carl-arith/numbers/numbers.h>

#ifdef USE_MPFR_FLOAT

void compare() {
	mpfr_t a;
	mpfr_init2(a,53);
	mpfr_set_ui(a,53, mpfr_rnd_t::MPFR_RNDN);

	mpfr_t b;
	mpfr_init2(b,14);

	mpfr_set_prec(b, mpfr_get_prec(a));
	mpfr_set(b, a, MPFR_RNDN);

	assert(b->_mpfr_prec == a->_mpfr_prec);
	assert(b->_mpfr_sign = a->_mpfr_sign);
	assert(b->_mpfr_exp = a->_mpfr_exp);

	std::size_t limbs = (std::size_t)std::ceil(double(a->_mpfr_prec)/double(mp_bits_per_limb));
	while( limbs > 0 ){
		assert(b->_mpfr_d[limbs-1] == a->_mpfr_d[limbs-1]);
		--limbs;
	}
}


void to_int(mpz_t intRep, mpfr_t a) {
	std::cout << "Bits per limb " << mp_bits_per_limb << std::endl;
	std::cout << "Number limbs " << std::ceil(double(a->_mpfr_prec)/double(mp_bits_per_limb)) << std::endl;
	std::cout << "Precision is " << a->_mpfr_prec << std::endl;
	std::cout << "Sign is " << a->_mpfr_sign << std::endl;
	//std::cout << "Exponent is " << carl::binary(a->_mpfr_exp) << std::endl;
	std::cout << "Exponent is " << a->_mpfr_exp << std::endl;
	std::cout << "Min Exponent is " << mpfr_get_emin() << std::endl;
	//std::cout << "Min Exponent is " << carl::binary(mpfr_get_emin()) << std::endl;
	std::cout << "Scaled exponent: " << (a->_mpfr_exp + std::abs(mpfr_get_emin())) << std::endl;
	//std::cout << "Scaled exponent: " << carl::binary((a->_mpfr_exp + std::abs(mpfr_get_emin()))) << std::endl;

	// mpfr mantissa is stored in limbs (usually 64-bit words) - the number of those depends on the precision.
	int limbs = std::ceil(double(a->_mpfr_prec)/double(mp_bits_per_limb));
	/*
	std::cout << "Mantissa is ";
	while( limbs > 0 ){
		std::cout << carl::binary(h->_mpfr_d[limbs-1]) << " " << std::endl;
		--limbs;
	}
	std::cout << std::endl;
	limbs = std::ceil(double(h->_mpfr_prec)/double(mp_bits_per_limb));
	*/
	mpz_t mant;
	mpz_t tmp;
	mpz_init(mant);
	mpz_init(tmp);
	mpz_set_ui(mant,0);
	mpz_set_ui(tmp,0);
	// as mpfr uses whole limbs (64-bit) we can cut away the additional zeroes (offset), if there are any
	unsigned offset = mp_bits_per_limb - (a->_mpfr_prec % mp_bits_per_limb);
	//std::cout << "Offset is " << offset << " bits" << std::endl;
	//std::cout << "Mantissa is ";
	char outStr[1024];

	// assemble the integer representation of the mantissa. The limbs are stored in reversed order, least significant first.
	while( limbs > 0 ){
		mpz_set_ui(tmp, a->_mpfr_d[limbs-1]);
		//std::cout << "Shift: " << (mp_bits_per_limb*(limbs-1)) << " bits" << std::endl;
 		mpz_mul_2exp(tmp, tmp, (mp_bits_per_limb*(limbs-1)));
		mpz_add(mant, mant, tmp);
		--limbs;
	}
	// cut away unnecessary zeroes at the end (divide by 2^offset -> left-shift).
	mpz_cdiv_q_2exp(mant, mant, offset);

	mpz_get_str(outStr, 2,mant);
	std::cout << "Mantissa: " << std::string(outStr) << std::endl;

	unsigned correction = (a->_mpfr_exp - a->_mpfr_prec);

	std::cout << "correction: " << correction << std::endl;

	mpz_mul_2exp(mant, mant, correction);

	/*
	mpz_get_str(outStr, 2,mant);
	std::cout << std::string(outStr) << std::endl;
	mpz_get_str(outStr, 10,mant);
	std::cout << std::string(outStr) << std::endl;
	*/

	// cleanup.
	mpz_clear(mant);
	mpz_clear(tmp);
}

#endif

int main (int argc, char** argv)
{
	/*
	char outStr[1024];

	mpfr_t a;
	mpfr_init(a);
	mpfr_set_ui(a,10, mpfr_rnd_t::MPFR_RNDN);

	mpfr_sprintf(outStr, "%.20RDe", a);
	std::cout << std::string(outStr) << std::endl;

	mpz_t b;
	mpz_init(b);
	mpz_set_ui(b,0);

	to_int(b,a);

	mpz_get_str(outStr, 10,b);
	std::cout << std::string(outStr) << std::endl;
	*/
#ifdef USE_MPFR_FLOAT
	compare();
#endif
	return 0;
}