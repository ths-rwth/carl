/**
 * This file is for short testing - not intended for distribution.
 * @file FloatExample.cpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-16
 * @version	2013-12-16
 */
#include <carl/interval/rounding.h>
#include <carl/numbers/numbers.h>

#ifdef USE_MPFR_FLOAT
void toInt(mpz_t intRep, mpfr_t a) {
	//std::cout << "Bits per limb " << mp_bits_per_limb << std::endl;
	//std::cout << "Number limbs " << std::ceil(double(a->_mpfr_prec)/double(mp_bits_per_limb)) << std::endl;
	//std::cout << "Precision is " << a->_mpfr_prec << std::endl;
	//std::cout << "Sign is " << a->_mpfr_sign << std::endl;
	//std::cout << "Exponent is " << carl::binary(a->_mpfr_exp) << std::endl;
	//std::cout << "Exponent is " << a->_mpfr_exp << std::endl;
	//std::cout << "Min Exponent is " << mpfr_get_emin() << std::endl;
	//std::cout << "Min Exponent is " << carl::binary(mpfr_get_emin()) << std::endl;
	//std::cout << "Scaled exponent: " << (a->_mpfr_exp + std::abs(mpfr_get_emin())) << std::endl;
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
	//char outStr[1024];

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

	//mpz_get_str(outStr, 2,mant);
	//std::cout << "Mantissa: " << std::string(outStr) << std::endl;

	// set the exponent (8-bit), as it is in 2s complement, subtract the minimum to shift the exponent and get exponents ordered,
	// right shift to put it before the mantissa later.
	mpz_set_ui(tmp, (a->_mpfr_exp + std::abs(mpfr_get_emin())));
	//std::cout << "Shift by " << (std::ceil(double(h->_mpfr_prec)/double(mp_bits_per_limb))*64+64-offset) << " bits" << std::endl;
	mpz_mul_2exp(tmp,tmp,a->_mpfr_prec);

	// assemble the whole representation by addition and finally add sign.
	mpz_add(mant,mant,tmp);
	mpz_mul_si(mant,mant,a->_mpfr_sign);
	mpz_set(intRep, mant);
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

unsigned distance(mpfr_t a, mpfr_t b) {
	// initialize variables
	mpz_t intRepA;
	mpz_t intRepB;
	mpz_init(intRepA);
	mpz_init(intRepB);
	mpz_t distance;
	mpz_init(distance);

	// the offset is used to cope with the exponent differences
	long offset = a->_mpfr_exp - b->_mpfr_exp;
	//std::cout << "Offset " << offset << std::endl;

	// get integer representations, we use absolute values for simplicity.
	toInt(intRepA, a);
	toInt(intRepB, b);
	mpz_abs(intRepA, intRepA);
	mpz_abs(intRepB, intRepB);



	// case distinction to cope with zero.
	if(mpfr_zero_p(a) != 0) { // a is zero
		if(mpfr_zero_p(b) != 0){ // b is also zero
			mpz_clear(distance);
			mpz_clear(intRepA);
			mpz_clear(intRepB);
			return 0;
		}

		// b is not zero -> we compute the distance from close to zero to b and add 1.
		mpfr_t zero;
		mpz_t intRepZero;
		mpfr_init2(zero,mpfr_get_prec(a));
		mpz_init(intRepZero);

		mpfr_set_ui(zero,0, MPFR_RNDZ);
		if(b->_mpfr_sign > 0) {
			mpfr_nextabove(zero);
		}else{
			mpfr_nextbelow(zero);
		}

		toInt(intRepZero, zero);
		mpz_abs(intRepZero, intRepZero);
		mpz_sub(distance, intRepB,intRepZero);
		mpz_add_ui(distance,distance, 1);

		mpfr_clear(zero);
		mpz_clear(intRepZero);
	} else if(mpfr_zero_p(b) != 0) { // a is not zero, b is zero
		mpfr_t zero;
		mpz_t intRepZero;
		mpfr_init2(zero,mpfr_get_prec(a));
		mpz_init(intRepZero);

		mpfr_set_ui(zero,0, MPFR_RNDZ);
		if(a->_mpfr_sign > 0) {
			mpfr_nextabove(zero);
		}else{
			mpfr_nextbelow(zero);
		}


		toInt(intRepZero, zero);
		mpz_abs(intRepZero, intRepZero);
		mpz_sub(distance, intRepA,intRepZero);
		mpz_add_ui(distance,distance, 1);

		mpfr_clear(zero);
		mpz_clear(intRepZero);
	} else if(a->_mpfr_sign == b->_mpfr_sign) { // both are not zero and at the same side
		mpz_sub(distance, intRepA, intRepB);
		mpz_abs(distance,distance);
	} else { // both are not zero and one is larger, the other one is less zero, compute both distances to zero and add 2.
		mpfr_t zeroA;
		mpfr_init2(zeroA,mpfr_get_prec(a));
		mpfr_t zeroB;
		mpfr_init2(zeroB,mpfr_get_prec(a));

		mpfr_set_ui(zeroA,0, MPFR_RNDZ);
		mpfr_set_ui(zeroB,0, MPFR_RNDZ);

		if(a->_mpfr_sign > 0) {
			mpfr_nextabove(zeroA);
			mpfr_nextbelow(zeroB);
		}else{
			mpfr_nextbelow(zeroA);
			mpfr_nextabove(zeroB);
		}
		mpz_t intRepZeroA;
		mpz_init(intRepZeroA);
		mpz_t intRepZeroB;
		mpz_init(intRepZeroB);
		mpz_t d2;
		mpz_init(d2);

		toInt(intRepZeroA, zeroA);
		mpz_abs(intRepZeroA, intRepZeroA);
		toInt(intRepZeroB, zeroB);
		mpz_abs(intRepZeroB, intRepZeroB);

		mpz_sub(distance, intRepA,intRepZeroA);
		mpz_sub(d2, intRepB,intRepZeroB);
		mpz_add(distance, distance, d2);
		mpz_add_ui(distance,distance, 2);

		mpfr_clear(zeroA);
		mpfr_clear(zeroB);
		mpz_clear(intRepZeroA);
		mpz_clear(intRepZeroB);
		mpz_clear(d2);
	}
	//std::cout << "Modify by " << 2*std::abs(offset)*a->_mpfr_prec << std::endl;

	// shift by offset (exponent differences).
	unsigned result = mpz_get_ui(distance) - 2*std::abs(offset)*a->_mpfr_prec;

	// cleanup.
	mpz_clear(distance);
	mpz_clear(intRepA);
	mpz_clear(intRepB);
	return result;
}
#endif

int main (int argc, char** argv)
{
#ifdef USE_MPFR_FLOAT
	//carl::FLOAT_T<mpfr_t> eps = std::numeric_limits<carl::FLOAT_T<mpfr_t>>::epsilon();
	carl::FLOAT_T<mpfr_t> eps = carl::FLOAT_T<mpfr_t>::maxval();
	char out[1000];
	std::string fString = "%." + std::to_string(eps.precision()+5) + "RNf";
	//char format[fString.length()];
	char* format = (char*)fString.c_str();
	std::cout << "Precision is " << eps.precision() << std::endl;
 	mpfr_sprintf(out, "%.RDe", eps.value());
	std::cout << std::string(out) << std::endl;


	/*
#ifdef USE_MPFR_FLOAT
	/*
	mpfr_ptr numberA = new mpfr_t();
	mpfr_init(numberA);
	mpfr_set_ui(numberA, 5, MPFR_RNDN);

	mpfr_ptr numberB = new mpfr_t;
	mpfr_init(numberB);
	mpfr_set_ui(numberB, 5, MPFR_RNDN);

	mpfr_ptr result;
	mpfr_init(result);

	//carl::convRnd<mpfr_ptr> r;
	//mpfr_rnd_t tmp =  r.operator() (carl::CARL_RND::CARL_RNDD);

	//result = carl::add_down(numberA, numberB);

	mpfr_clears(numberA, numberB, result);
	*/
	/*
	mpfr_t f;
	mpfr_init2(f,100);
	mpfr_set_d(f, 0, MPFR_RNDN);
	//mpfr_nextabove(f);
	//mpfr_nextbelow(f);
	//mpfr_nextbelow(f);
	//mpfr_nextbelow(f);
	//mpfr_nextbelow(f);


	char out1[120];
	char out2[120];
	for(unsigned target = 0; target < 400; ++target){
		int targetDistance = target;
		mpfr_t g;
		mpfr_init2(g,100);
		mpfr_set_d(g, 0, MPFR_RNDN);
		while(targetDistance > 0){
			mpfr_nextbelow(g);
			--targetDistance;
		}

		mpfr_sprintf(out1, "%.64RDe", f);
		mpfr_sprintf(out2, "%.64RDe", g);
		std::cout << "Distance (" << std::string(out1) << ", " << std::string(out2) << ") =" << distance(f,g) << std::endl;
		mpfr_clear(g);
		std::cout << "##############################" << std::endl;
	}

	mpfr_clear(f);
	*/
	/*
	carl::FLOAT_T<mpfr_t> a(1);
	carl::FLOAT_T<mpfr_t> b(2);

	carl::AlmostEqual2sComplement(a,b);
	*/
	/*
	std::cout << "F" << std::endl;
	char out[120];
	mpfr_sprintf(out, "%.100RDe", f);
	std::cout << std::string(out) << std::endl;
	mpfr_sprintf(out, "%.100RDb", f);
	std::cout << std::string(out) << std::endl;

	std::cout << "G" << std::endl;
	mpfr_sprintf(out, "%.100RDe", g);
	std::cout << std::string(out) << std::endl;
	mpfr_sprintf(out, "%.100RDb", g);
	std::cout << std::string(out) << std::endl;

	mpz_t significantA;
	mpz_init(significantA);
	mpz_t significantB;
	mpz_init(significantB);

	mpfr_exp_t expA = mpfr_get_z_exp(significantA,f);
	std::cout << "ExpA: " << expA << std::endl;
	mpfr_exp_t expB = mpfr_get_z_exp(significantB,g);
	std::cout << "ExpB: " << expB << std::endl;
	//std::cout << "Exp converted: " << carl::binary(expA) << std::endl;
	std::cout << __func__ << ": 2^" << expA << " * " << mpz_get_si(significantA) << " and 2^" << expB << " * " << mpz_get_si(significantB) << std::endl;

	if(expA == expB) {
		std::cout << "Equal exponents, compare mantissa" << std::endl;
		mpz_t diff;
		mpz_init(diff);
		mpz_sub(diff, significantA, significantB);
		mpz_abs(diff,diff);
		std::cout << "difference: " << mpz_get_si(diff) << std::endl;
	}else if (expA > expB) {
		std::cout << "expA > expB" << std::endl;
		mpz_mul_2exp(significantB,significantB,expA-expB);
		std::cout << "Shifted: " << mpz_get_si(significantB) << std::endl;
		mpz_t diff;
		mpz_init(diff);
		mpz_sub(diff, significantA, significantB);
		mpz_abs(diff,diff);
		std::cout << "difference: " << mpz_get_si(diff) << std::endl;
	}else{
		std::cout << "expB > expA" << std::endl;
		mpz_mul_2exp(significantA,significantA,expB-expA);
		std::cout << "Shifted: " << mpz_get_si(significantA) << std::endl;
		mpz_t diff;
		mpz_init(diff);
		mpz_sub(diff, significantA, significantB);
		mpz_abs(diff,diff);
		std::cout << "difference: " << mpz_get_si(diff) << std::endl;
	}
	*/
	/*
	std::cout << "DECONSTRUCT mpfr_t" << std::endl;
	mpfr_t h;
	mpfr_init2(h,65);
	mpfr_set_d(h, 0, MPFR_RNDN);

	targetDistance = 2;
	while(targetDistance > 0){
		mpfr_nextbelow(h);
		--targetDistance;
	}

	std::cout << "Bits per limb " << mp_bits_per_limb << std::endl;
	std::cout << "Number limbs " << std::ceil(double(h->_mpfr_prec)/double(mp_bits_per_limb)) << std::endl;
	std::cout << "Precision is " << h->_mpfr_prec << std::endl;
	std::cout << "Sign is " << h->_mpfr_sign << std::endl;
	std::cout << "Exponent is " << carl::binary(h->_mpfr_exp) << std::endl;
	std::cout << "Exponent is " << h->_mpfr_exp << std::endl;
	std::cout << "Min Exponent is " << mpfr_get_emin() << std::endl;
	std::cout << "Min Exponent is " << carl::binary(mpfr_get_emin()) << std::endl;

	std::cout << "Scaled exponent: " << (h->_mpfr_exp + std::abs(mpfr_get_emin())) << std::endl;
	std::cout << "Scaled exponent: " << carl::binary((h->_mpfr_exp + std::abs(mpfr_get_emin()))) << std::endl;


	int limbs = std::ceil(double(h->_mpfr_prec)/double(mp_bits_per_limb));
	std::cout << "Mantissa is ";
	while( limbs > 0 ){
		std::cout << carl::binary(h->_mpfr_d[limbs-1]) << " " << std::endl;
		--limbs;
	}
	std::cout << std::endl;
	limbs = std::ceil(double(h->_mpfr_prec)/double(mp_bits_per_limb));

	mpz_t mant;
	mpz_t tmp;
	mpz_init(mant);
	mpz_init(tmp);
	unsigned offset = mp_bits_per_limb - (h->_mpfr_prec % mp_bits_per_limb);
	std::cout << "Offset is " << offset << " bits" << std::endl;
	std::cout << "Mantissa is ";
	char outStr[1024];
	while( limbs > 0 ){
		mpz_set_ui(tmp, h->_mpfr_d[limbs-1]);
		//std::cout << "Shift: " << (mp_bits_per_limb*(limbs-1)) << " bits" << std::endl;
 		mpz_mul_2exp(tmp, tmp, (mp_bits_per_limb*(limbs-1)));
		mpz_add(mant, mant, tmp);
		//mant = (h->_mpfr_d[limbs-1] << (mp_bits_per_limb*(limbs)));
		--limbs;
	}
	mpz_cdiv_q_2exp(mant, mant, offset);
	mpz_get_str(outStr, 2,mant);
	std::cout << std::string(outStr) << std::endl;

	mpz_set_ui(tmp, (h->_mpfr_exp + std::abs(mpfr_get_emin())));
	std::cout << "Shift by " << (std::ceil(double(h->_mpfr_prec)/double(mp_bits_per_limb))*64+64-offset) << " bits" << std::endl;
	mpz_mul_2exp(tmp,tmp,h->_mpfr_prec);
	mpz_add(mant,mant,tmp);

	mpz_get_str(outStr, 2,mant);
	std::cout << std::string(outStr) << std::endl;
	mpz_get_str(outStr, 10,mant);
	std::cout << std::string(outStr) << std::endl;
	*/

	//mpfr_sprintf(out, "%.100RDb", h);
	//std::cout << std::string(out) << std::endl;
	//std::cout << carl::binary(h->_mpfr_exp) << std::endl;
	/*
	mpfr_t res;
	mpfr_init2(res,100);

	mpfr_t mantissa;
	mpfr_init2(mantissa,100);
	mpfr_set_z(mantissa,significant, MPFR_RNDN);

	mpfr_t exponent;
	mpfr_init2(exponent,100);
	mpfr_set_si(exponent, expA, MPFR_RNDN);

	mpfr_mul_2si(res, mantissa, expA, MPFR_RNDN);
	mpfr_sprintf(out, "%.100RDb", res);
	std::cout << "RES: " << std::endl << std::string(out) << std::endl;

	mpfr_clear(f);

	#endif
	*/
#endif
	return 0;
}
