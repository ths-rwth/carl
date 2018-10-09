/**
 * @file Z3Converter.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once
#ifdef USE_Z3_NUMBERS
#include <utility>

#include "../numbers/adaption_z3/include.h"


#include "carl/numbers/numbers.h"
#include "carl/core/Variable.h"
#include "carl/core/Monomial.h"
#include "carl/core/Term.h"
#include "carl/core/MultivariatePolynomial.h"

namespace carl {

class Z3Converter {
private:
	std::map<carl::Variable, polynomial::var> vars;
	reslimit rl;
	polynomial::numeral_manager nm;
	polynomial::manager m;
	mpz_manager<true> mpzm;
public:
	Z3Converter(): 
		m(rl, nm) {
	}
	/**
	 * Converts a polynomial pointer to a polynomial reference.
     */
	polynomial::polynomial_ref operator()(polynomial::polynomial* p) {
		return polynomial::polynomial_ref(p, this->m);
	}
	/**
	 * Converts a number.
     */
	rational operator()(const rational& n) {
		return n;
	}
    #ifdef USE_CLN_NUMBERS
	rational operator()(const cln::cl_RA& n) {
		std::stringstream ss1;
		ss1 << carl::getDenom(n);
		mpz denom;
		mpzm.set(denom, ss1.str().c_str());
		std::stringstream ss2;
		ss2 << carl::getNum(n);
		mpz num;
		mpzm.set(num, ss2.str().c_str());
		return rational(num / denom);
	}
    #endif
	rational operator()(const mpq_class& n) {
		std::stringstream ss1;
		ss1 << n.get_den();
		mpz denom;
		mpzm.set(denom, ss1.str().c_str());
		std::stringstream ss2;
		ss2 << n.get_num();
		mpz num;
		mpzm.set(num, ss2.str().c_str());
		return rational(num / denom);
	}
	/**
	 * Converts a variable.
     */
	polynomial::var operator()(const carl::Variable& v) {
		auto it = vars.find(v);
		if (it == vars.end()) {
			it = vars.insert(std::make_pair(v, m.mk_var())).first;
		}
		return it->second;
	}
	/**
	 * Converts a variable and an exponent.
     */
	polynomial::polynomial_ref operator()(const std::pair<carl::Variable, carl::exponent>& p) {
		return (*this)(this->m.mk_polynomial((*this)(p.first), p.second));
	}
	/**
	 * Converts a monomial.
     */
	polynomial::polynomial_ref operator()(const carl::Monomial& m) {
		polynomial::polynomial_ref res(this->m);
		res = this->m.mk_const(rational(1));
		for (auto it: m) {
			res = res * (*this)(it);
		}
		return res;
	}
	template<typename Coeff>
	polynomial::polynomial_ref operator()(const carl::Term<Coeff>& t) {
		polynomial::polynomial_ref res(this->m);
		res = (*this)(this->m.mk_const((*this)(t.coeff())));
		if (t.monomial()) return res * (*this)(*(t.monomial()));
		else return res;
	}
	template<typename Coeff>
	polynomial::polynomial_ref operator()(const carl::MultivariatePolynomial<Coeff>& p) {
		polynomial::polynomial_ref res(this->m);
		res = (*this)(this->m.mk_zero());
		for (auto t: p) res = res + (*this)(t);
		this->m.lex_sort(res);
		return res;
	}
	template<typename Coeff>
	polynomial::polynomial_ref operator()(const carl::UnivariatePolynomial<Coeff>& p) {
		polynomial::polynomial_ref res(this->m);
		res = (*this)(this->m.mk_zero());
		unsigned exp = 0;
		polynomial::var mainvar = (*this)(p.mainVar());
		for (auto t: p.coefficients()) {
			res = res + (*this)(this->m.mk_polynomial(mainvar, exp)) * (*this)(t);
			exp++;
		}
		this->m.lex_sort(res);
		return res;
	}
};

}
#endif
