#include "LibpolyConverter.h"

#if defined(USE_LIBPOLY) || defined(RAN_USE_LIBPOLY)

namespace carl {

struct MonomialData {
	std::vector<carl::Term<mpq_class>> terms;

	MonomialData() {}
};

/**
 * This function is called for every monomial in a polynomial (by lp_polynomial_traverse)
 * The monomial is converted to a carl term and appended to the vector in MonomialData
 */
void collectMonomialData(const lp_polynomial_context_t* ctx,
						 lp_monomial_t* m,
						 void* d) {
	MonomialData* data = static_cast<MonomialData*>(d);

	carl::Term<mpq_class> term(mpz_class(&m->a));															   // m->a is the integer coefficient
	for (size_t i = 0; i < m->n; i++) {																		   // m->n is the capacity of the power array
		term *= carl::Term<mpq_class>(1, VariableMapper::getInstance().getCarlVariable(m->p[i].x), m->p[i].d); //p[i].x is the variable, p[i].d is the power
	}
	data->terms.emplace_back(term);
}

carl::MultivariatePolynomial<mpq_class> LibpolyConverter::toCarlMultivariatePolynomial(const poly::Polynomial& p) {
	MonomialData data;
	CARL_LOG_DEBUG("carl.converter", "Converting Libpoly Polynomial " << p);
	lp_polynomial_traverse(p.get_internal(), collectMonomialData, &data);

	if (data.terms.empty()) {
		CARL_LOG_DEBUG("carl.converter", "Empty Poly, returning 0");
		return carl::MultivariatePolynomial<mpq_class>();
	} else {
		CARL_LOG_DEBUG("carl.converter", "Found Terms: " << data.terms);
		return carl::MultivariatePolynomial<mpq_class>(data.terms);
	}
}
carl::MultivariatePolynomial<mpq_class> LibpolyConverter::toCarlMultivariatePolynomial(const poly::Polynomial& p, const mpz_class& denominator) {
	return (toCarlMultivariatePolynomial(p) / mpq_class(denominator));
}
carl::UnivariatePolynomial<mpq_class> LibpolyConverter::toCarlUnivariatePolynomial(const poly::UPolynomial& p, const carl::Variable& mainVar) {
	CARL_LOG_DEBUG("carl.converter", "Converting Libpoly UPolynomial " << p);
	CARL_LOG_DEBUG("carl.converter", "Got Carl mainVar " << mainVar);
	std::vector<mpq_class> carl_coeffs;
	for (const poly::Integer& coeff : poly::coefficients(p)) {
		carl_coeffs.emplace_back(*(poly::detail::cast_to_gmp(&coeff)));
	}
	CARL_LOG_DEBUG("carl.converter", "Coefficient List" << carl_coeffs);
	return carl::UnivariatePolynomial<mpq_class>(mainVar, carl_coeffs);
}
carl::UnivariatePolynomial<mpq_class> LibpolyConverter::toCarlUnivariatePolynomial(const poly::UPolynomial& p, const mpz_class& denominator, carl::Variable& mainVar) {
	return (toCarlUnivariatePolynomial(p, mainVar) / mpq_class(denominator));
}

template<>
mpq_class LibpolyConverter::toNumber<mpq_class>(const poly::Integer& m) {
	return *poly::detail::cast_to_gmp(&m);
}

template<>
mpq_class LibpolyConverter::toNumber<mpq_class>(const poly::Rational& m) {
	return *poly::detail::cast_to_gmp(&m);
}

template<>
mpq_class LibpolyConverter::toNumber<mpq_class>(const lp_rational_t* m) {
	return *reinterpret_cast<const mpq_class*>(m);
}

template<>
mpz_class LibpolyConverter::toNumber<mpz_class>(const poly::Integer& m) {
	return *poly::detail::cast_to_gmp(&m);
}

template<>
mpq_class LibpolyConverter::toNumber<mpq_class>(const poly::DyadicRational& m) {
	return mpq_class(toNumber<mpz_class>(poly::numerator(m)), toNumber<mpz_class>(poly::denominator(m)));
}

template<>
poly::Rational LibpolyConverter::toLibpolyRational<mpq_class>(const mpq_class& num) {
	return poly::Rational(num);
}

//Exact for whole numbers
template<>
poly::DyadicRational LibpolyConverter::getDyadicApproximation(const mpz_class& num, const unsigned int& precision) {
	return poly::DyadicRational(poly::Integer(num));
}

/**
 * Tries to convert num = a/b into a dyadic rational of the form c/2^d
 * We take d = precision * ceil(log(2,b)) and c = ceil((a * 2^d)/(b))
 * @return Approximation to num by a dyadic rational
 */
template<>
poly::DyadicRational LibpolyConverter::getDyadicApproximation(const mpq_class& num, const unsigned int& precision) {
	CARL_LOG_DEBUG("carl.converter", "Starting Dyadic Approximation with: " << num);

	mpz_class a = num.get_num();
	mpz_class b = num.get_den();

	mpz_class d = (precision)*carl::ceil(carl::log(b)); //unsigned long
	assert(d.fits_ulong_p());

	mpz_class c = carl::ceil((a * carl::pow(mpq_class(2), d.get_ui())) / (b));

	if (!c.fits_slong_p()) {
		CARL_LOG_DEBUG("carl.converter", "Dyadic Rational: Fallback for overflow of numerator");
		poly::DyadicRational tmp = getDyadicApproximation(c); //Construct with arbitrary precision
		return poly::div_2exp(tmp, d.get_ui());
	}

	CARL_LOG_DEBUG("carl.converter", "Got d: " << d << " and c " << c);
	CARL_LOG_DEBUG("carl.converter", "Got dyadic number: " << poly::DyadicRational(c.get_si(), d.get_ui()));

	return poly::DyadicRational(c.get_si(), d.get_ui());
}

/**
 * Convert a carl interval to a libpoly interval
 * This keeps the bound types
 */
template<>
poly::Interval LibpolyConverter::toInterval(const carl::Interval<mpq_class>& inter) {
	poly::Value low;
	bool low_open;
	switch (inter.lower_bound_type()) {
	case BoundType::STRICT:
		low = poly::Value(poly::Rational(inter.lower()));
		low_open = true;
		break;
	case BoundType::WEAK:
		low = poly::Value(poly::Rational(inter.lower()));
		low_open = false;
		break;
	case BoundType::INFTY:
		low = poly::Value::minus_infty();
		low_open = true;
		break;
	}
	poly::Value up;
	bool up_open;
	switch (inter.upper_bound_type()) {
	case BoundType::STRICT:
		up = poly::Value(poly::Rational(inter.upper()));
		up_open = true;
		break;
	case BoundType::WEAK:
		up = poly::Value(poly::Rational(inter.upper()));
		up_open = false;
		break;
	case BoundType::INFTY:
		up = poly::Value::plus_infty();
		up_open = false;
		break;
	}
	return poly::Interval(low, low_open, up, up_open);
}

/**
 * Converts a libpoly dyadic interval to a carl interval
 * Keeps the bound types
 */
template<typename Number>
carl::Interval<Number> LibpolyConverter::toInterval(const poly::DyadicInterval& inter) {
	BoundType upper_bound = inter.get_internal()->a_open ? BoundType::STRICT : BoundType::WEAK ;
	BoundType lower_bound = inter.get_internal()->b_open ? BoundType::STRICT : BoundType::WEAK ;

	return carl::Interval<Number>(toNumber<Number>(poly::get_lower(inter)), lower_bound, toNumber<Number>(poly::get_upper(inter)), upper_bound);
}

/**
 * Converts a libpoly interval to a carl interval
 * Keeps the bound types
 */
template<typename Number>
carl::Interval<Number> LibpolyConverter::toInterval(const poly::Interval& inter) {
	BoundType upper_bound = inter.get_internal()->a_open ? BoundType::STRICT : BoundType::WEAK ;
	BoundType lower_bound = inter.get_internal()->b_open ? BoundType::STRICT : BoundType::WEAK ;

	return carl::Interval<Number>(toNumber<Number>(poly::get_lower(inter)), lower_bound, toNumber<Number>(poly::get_upper(inter)), upper_bound);
}

} // namespace carl

#endif