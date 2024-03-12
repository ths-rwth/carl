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
		
		CARL_LOG_DEBUG("carl.converter", "Coprime Factor/ Denominator: " << denominator);
		lp_polynomial_t* res = lp_polynomial_new(context.lp_context());
		for (const auto& term : p) {
			assert(carl::get_denom(term.coeff() * denominator) == 1);
			lp_monomial_t t;
			lp_monomial_construct(context.lp_context(), &t);
			lp_monomial_set_coefficient(context.lp_context(), &t, (lp_integer_t*)mpz_class(term.coeff() * denominator).get_mpz_t());
			if (term.monomial()) {
				// A monomial is a vector of pairs <variable, power>
				for (const std::pair<carl::Variable, std::size_t>& var_pow : (*term.monomial()).exponents()) {
					lp_monomial_push(&t, context.lp_variable(var_pow.first), var_pow.second);
				}
			}
			lp_polynomial_add_monomial(res, &t);
			lp_monomial_destruct(&t);
		}
		return LPPolynomial(context, poly::Polynomial(res));
	}
};

namespace conversion_private {
template<typename T>
struct CollectTermData {
	std::vector<carl::Term<T>> terms;
	const LPContext& context;
	CollectTermData(const LPContext& c) : context(c) {}
};

template<typename T>
void collectTermDataFunction(const lp_polynomial_context_t* /*ctx*/, lp_monomial_t* m, void* d) {
	CollectTermData<T>* data = static_cast<CollectTermData<T>*>(d);
	carl::Term<T> term(mpz_class(&m->a));															   // m->a is the integer coefficient
	for (size_t i = 0; i < m->n; i++) {																   // m->n is the capacity of the power array
		auto carl_var = data->context.carl_variable(m->p[i].x);
		assert(carl_var.has_value());
		term *= carl::Term<T>(1, *carl_var, m->p[i].d); // p[i].x is the variable, p[i].d is the power
	}
	data->terms.emplace_back(term);
}
} // namespace conversion_private

template<typename A, typename B, typename C>
struct ConvertHelper<MultivariatePolynomial<A, B, C>, LPPolynomial> {
	static MultivariatePolynomial<A, B, C> convert(const LPPolynomial& p) {
		conversion_private::CollectTermData<A> termdata(p.context());
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