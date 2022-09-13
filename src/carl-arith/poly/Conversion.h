#pragma once

#include <carl-common/config.h>

#include <carl-arith/poly/ctxpoly/ContextPolynomial.h>
#include <carl-arith/poly/libpoly/LPPolynomial.h>
#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>

namespace carl {

namespace convert_poly {
template<typename T, typename S>
struct ConvertHelper {};

template<typename A, typename B, typename C>
struct ConvertHelper<ContextPolynomial<A, B, C>, MultivariatePolynomial<A, B, C>> {
	static ContextPolynomial<A, B, C> convert(const Context& context, const MultivariatePolynomial<A, B, C>& p) {
		return ContextPolynomial<A, B, C>(context, p);
	}
};

template<typename A, typename B, typename C>
struct ConvertHelper<MultivariatePolynomial<A, B, C>, ContextPolynomial<A, B, C>> {
	static MultivariatePolynomial<A, B, C> convert(const ContextPolynomial<A, B, C>& p) {
		return p.as_multivariate();
	}
};

#ifdef USE_LIBPOLY
template<typename A, typename B, typename C>
struct ConvertHelper<LPPolynomial, MultivariatePolynomial<A, B, C>> {
	static LPPolynomial convert(const LPContext& context, const MultivariatePolynomial<A, B, C>& p) {
		CARL_LOG_DEBUG("carl.converter", "Converting Carl Multivariate Poly " << p << " with LPContext " << context);

		// Problem: LPPolynomials must have mpz_class as coefficient type -> might loose information when converting mpq_class to mpz_class

		if (carl::is_constant(p)) {
			CARL_LOG_DEBUG("carl.converter", "Poly is constant");
			return LPPolynomial(context, carl::get_num(p.constant_part()));
		}

		auto denominator = p.coprime_factor();
		if (denominator < 0) {
			denominator *= -1;
		}
		/*
		// LPPolynomial can only have integer coefficients -> so we have to store the lcm of the coefficients of every term
		auto coprimeFactor = p.main_denom();
		mpz_class denominator;
		if (carl::get_denom(coprimeFactor) != 1) {
			// if coprime factor is not an integer
			denominator = coprimeFactor > 0 ? mpz_class(1) : mpz_class(-1);
		} else {
			denominator = mpz_class(coprimeFactor);
		}
		*/
		CARL_LOG_DEBUG("carl.converter", "Coprime Factor/ Denominator: " << denominator);
		LPPolynomial res(context);
		// iterate over terms
		for (const auto& term : p) {
			// Multiply by denominator to make the coefficient an integer
			assert(carl::get_denom(term.coeff() * denominator) == 1);
			LPPolynomial t(context, mpz_class(term.coeff() * denominator));
			// iterate over monomial
			if (term.monomial()) {
				// A monomial is a vector of pairs <variable, power>
				for (const std::pair<carl::Variable, std::size_t>& var_pow : (*term.monomial()).exponents()) {
					// multiply 1*var^pow
					t *=LPPolynomial(context, var_pow.first, mpz_class(1), (unsigned)var_pow.second);
				}
			}
			CARL_LOG_TRACE("carl.converter", "converted term: " << term << " -> " << t);
			res += t;
		}
		CARL_LOG_DEBUG("carl.converter", "Got Polynomial: " << res);
		return res;
	}
};

namespace conversion_private {
template<typename T>
struct collectTermData {
	std::vector<carl::Term<T>> terms;
};

template<typename T>
void collectTermDataFunction(const lp_polynomial_context_t* /*ctx*/, lp_monomial_t* m, void* d) {
	collectTermData<T>* data = static_cast<collectTermData<T>*>(d);
	carl::Term<T> term(mpz_class(&m->a));															   // m->a is the integer coefficient
	for (size_t i = 0; i < m->n; i++) {																   // m->n is the capacity of the power array
		term *= carl::Term<T>(1, VariableMapper::getInstance().getCarlVariable(m->p[i].x), m->p[i].d); // p[i].x is the variable, p[i].d is the power
	}
	data->terms.emplace_back(term);
}
} // namespace conversion_private

template<typename A, typename B, typename C>
struct ConvertHelper<MultivariatePolynomial<A, B, C>, LPPolynomial> {
	static MultivariatePolynomial<A, B, C> convert(const LPPolynomial& p) {
		conversion_private::collectTermData<A> termdata;
		CARL_LOG_DEBUG("carl.converter", "Converting LPPolynomial " << p);
		lp_polynomial_traverse(p.get_internal(), conversion_private::collectTermDataFunction<A>, &termdata);

		if (termdata.terms.empty()) {
			return MultivariatePolynomial<A, B, C>();
		}
		return MultivariatePolynomial<A, B, C>(termdata.terms);
	}
};
#endif

}

template<typename T, typename S, std::enable_if_t<is_polynomial_type<T>::value && is_polynomial_type<S>::value && !needs_context_type<T>::value, int> = 0>
inline T convert(const S& r) {
    return convert_poly::ConvertHelper<T,S>::convert(r);
}

template<typename T, typename S, std::enable_if_t<is_polynomial_type<T>::value && is_polynomial_type<S>::value && needs_context_type<T>::value, int> = 0>
inline T convert(const typename T::ContextType& c, const S& r) {
    return convert_poly::ConvertHelper<T,S>::convert(c, r);
}

} // namespace carl