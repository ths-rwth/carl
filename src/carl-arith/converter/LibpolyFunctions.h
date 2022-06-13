#pragma once

#include <carl-common/config.h>

#if defined(USE_LIBPOLY) || defined(RAN_USE_LIBPOLY)

#include "LibpolyConverter.h"
#include "LibpolyVariableMapper.h"

#include <poly/polyxx.h>

namespace carl {

class LibpolyFunctions {

public:
	LibpolyFunctions(){};


	template<typename Coeff>
	carl::UnivariatePolynomial<carl::MultivariatePolynomial<Coeff>> libpoly_resultant(const carl::UnivariatePolynomial<carl::MultivariatePolynomial<Coeff>>& p1, const carl::UnivariatePolynomial<carl::MultivariatePolynomial<Coeff>>& p2) {

		carl::Variable mainVar = p1.main_var();
		assert(p1.main_var() == p2.main_var()); //todo needed?


		//Only changes the resultant by a constant factor... is is that really needed? TODO
		mpz_class denom1;
		mpz_class denom2;
		uint deg1 = p1.degree();
		uint deg2 = p2.degree();

		poly::Polynomial poly_p1 = LibpolyConverter::getInstance().toLibpolyPolynomial(p1, denom1);
		poly::Polynomial poly_p2 = LibpolyConverter::getInstance().toLibpolyPolynomial(p2, denom2);

		//Write the mainVar back to libpoly
		VariableMapper::getInstance().setLibpolyMainVariable(mainVar);

		//Reorder libpoly polynomials according to the global variable ordering
		lp_polynomial_ensure_order(poly_p1.get_internal());
		lp_polynomial_ensure_order(poly_p2.get_internal());

		if (poly::main_variable(poly_p1) != VariableMapper::getInstance().getLibpolyVariable(mainVar)) {
			//mainVar is not present in p1, thus resultant is p1
			CARL_LOG_DEBUG("carl.converter", "MainVar is not in p1, returning p2");
			return p1;
		} else if (poly::main_variable(poly_p2) != VariableMapper::getInstance().getLibpolyVariable(mainVar)) {

			CARL_LOG_DEBUG("carl.converter", "MainVar is not in p2, returning p2");
			return p2;
		}

		//actual calculation
		poly::Polynomial res = poly::resultant(poly_p1, poly_p2);

		//Transform according to https://en.wikipedia.org/wiki/Resultant#Invariance_under_change_of_polynomials
		mpz_class factor = carl::pow(denom1, deg2) * carl::pow(denom2, deg1) ;

		//Turn back into carl Poly
		carl::UnivariatePolynomial<carl::MultivariatePolynomial<mpq_class>> res_carl = LibpolyConverter::getInstance().toCarlUnivariatePolynomial(res, factor, mainVar) ;

		return res_carl;
	}
};

} // namespace carl

#endif