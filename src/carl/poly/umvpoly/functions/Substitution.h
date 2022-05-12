#pragma once

#include "Degree.h"
#include "Evaluation.h"
#include "Power.h"

#include "../Monomial.h"

namespace carl {

/**
 * Applies the given substitutions to a monomial.
 * Every variable may be substituted by some value.
 * @param m The monomial.
 * @param substitutions Maps variables to numbers.
 * @return \f$ this[<substitutions>] \f$
 */
template<typename Coeff>
Coeff substitute(const Monomial& m, const std::map<Variable, Coeff>& substitutions) {
	CARL_LOG_FUNC("carl.core.monomial", m << ", " << substitutions);
	Coeff res = carl::constant_one<Coeff>::get();
	for (const auto& ve : m) {
		auto it = substitutions.find(ve.first);
		if (it == substitutions.end()) {
			res *= carl::pow(ve.first, ve.second);
		} else {
			res *= carl::pow(it->second, ve.second);
		}
	}
	CARL_LOG_TRACE("carl.core.monomial", "Result: " << res);
	return res;
}

template<typename Coeff>
Term<Coeff> substitute(const Term<Coeff>& t, const std::map<Variable, Coeff>& substitutions) {
	if (t.monomial()) {
		Monomial::Content content;
		Coeff coeff = t.coeff();
		for (const auto& c: *t.monomial()) {
			auto it = substitutions.find(c.first);
			if (it == substitutions.end()) {
				content.push_back(c);
			} else {
				coeff *= carl::pow(it->second, c.second);
			}
		}
		if (content.empty()) return Term<Coeff>(coeff);
		return Term<Coeff>(coeff, createMonomial(std::move(content)));
	} else {
		return Term<Coeff>(t.coeff());
	}
}

template<typename Coeff>
Term<Coeff> substitute(const Term<Coeff>& t, const std::map<Variable, Term<Coeff>>& substitutions) {
	if (t.monomial()) {
		return Coeff(t.coeff()) * substitute(*t.monomial(), substitutions);
	} else {
		return Term<Coeff>(t.coeff());
	}
}

template<typename C, typename O, typename P>
void substitute_inplace(MultivariatePolynomial<C,O,P>& p, Variable var, const MultivariatePolynomial<C,O,P>& value) {
	assert(p.isConsistent());
	assert(value.isConsistent());
	if (!p.has(var)) {
		return;
	}
	typename MultivariatePolynomial<C,O,P>::TermsType newTerms;
	// If we replace a variable by zero, just eliminate all terms containing the variable.
	if(carl::isZero(value))
	{
		bool removedLast = false;
		for (const auto& term: p) {
			if (!term.has(var)) {
				newTerms.push_back(term);
				removedLast = false;
			} else removedLast = true;
		}
		p.getTerms().swap(newTerms);
		CARL_LOG_TRACE("carl.core", p << " [ " << var << " -> " << value << " ] = " << p);
		if (removedLast) {
			p.reset_ordered();
			p.template makeMinimallyOrdered<false, true>();
		}
        assert(p.isConsistent());
		return;
	}
	// Find all exponents occurring with the variable to substitute as basis.
	// expResults will finally be a mapping from every exponent e for which var^e occurs to the value^e and the number of times var^e occurs.
	// Meanwhile, we store an upper bound on the expected number of terms of the result in expectedResultSize.
	std::map<exponent, std::pair<MultivariatePolynomial<C,O,P>, size_t>> expResults;
	size_t expectedResultSize = 0;
	std::pair<MultivariatePolynomial<C,O,P>, unsigned> def( MultivariatePolynomial<C,O,P>(constant_one<C>::get()), 1 );
	for(const auto& term: p)
	{
		if(term.monomial())
		{ // This is not the constant part.
			exponent e = term.monomial()->exponentOfVariable(var);
			if(e > 1)
			{ // Variable occurs with exponent at least two. Insert into map and increase counter in map.
				auto iterBoolPair = expResults.insert(std::pair<exponent, std::pair<MultivariatePolynomial<C,O,P>, size_t>>(e, def));
				if(!iterBoolPair.second)
				{
					++(iterBoolPair.first->second.second);
				}
			}
			else if(e == 1)
			{ // Variable occurs with exponent one.
				expectedResultSize += value.nrTerms();
			}
			else
			{ // Variable does not occur in this term.
				++expectedResultSize;
			}
		}
		else
		{ // This is the constant part.
			++expectedResultSize;
		}
	}
	// Calculate the exponentiation of the multivariate polynomial to substitute the
	// variable for, reusing the already calculated exponentiations.
	if( !expResults.empty() )
	{
		// Last var^e
		auto expResultA = expResults.begin();
		// Next var^e
		auto expResultB = expResultA;
		// Calculate first one
		expResultB->second.first = carl::pow(value, expResultB->first);
		expectedResultSize += expResultB->second.second * expResultB->second.first.nrTerms();
		++expResultB;
		while(expResultB != expResults.end())
		{
			// Calculate next var^e based on the last one.
			expResultB->second.first = expResultA->second.first * carl::pow(value, expResultB->first - expResultA->first);
			expectedResultSize += expResultB->second.second * expResultB->second.first.nrTerms();
			++expResultA;
			++expResultB;
		}
	}
	// Substitute the variable.
	auto& tam = MultivariatePolynomial<C,O,P>::mTermAdditionManager;
	auto id = tam.getId(expectedResultSize);
	for (const auto& term: p)
	{
		if (term.monomial() == nullptr) {
			tam.template addTerm<false>(id, term);
		} else {
			exponent e = term.monomial()->exponentOfVariable(var);
			Monomial::Arg mon;
			if (e > 0) mon = term.monomial()->dropVariable(var);
			if (e == 1) {
				for(auto vterm : value)
				{
					if (mon == nullptr) tam.template addTerm<false>(id, Term<C>(vterm.coeff() * term.coeff(), vterm.monomial()));
					else if (vterm.monomial() == nullptr) tam.template addTerm<false>(id, Term<C>(vterm.coeff() * term.coeff(), mon));
					else tam.template addTerm<false>(id, Term<C>(vterm.coeff() * term.coeff(), vterm.monomial() * mon));
				}
			} else if(e > 1) {
				auto iter = expResults.find(e);
				assert(iter != expResults.end());
				for(auto vterm : iter->second.first)
				{
					if (mon == nullptr) tam.template addTerm<false>(id, Term<C>(vterm.coeff() * term.coeff(), vterm.monomial()));
					else if (vterm.monomial() == nullptr) tam.template addTerm<false>(id, Term<C>(vterm.coeff() * term.coeff(), mon));
					else tam.template addTerm<false>(id, Term<C>(vterm.coeff() * term.coeff(), vterm.monomial() * mon));
				}
			}
			else
			{
				tam.template addTerm<false>(id, term);
			}
		}
	}
	tam.readTerms(id, p.getTerms());
	p.reset_ordered();
    p.template makeMinimallyOrdered<false, true>();
	assert(p.nrTerms() <= expectedResultSize);
	assert(p.isConsistent());
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> substitute(const MultivariatePolynomial<C,O,P>& p, Variable var, const MultivariatePolynomial<C,O,P>& value) {
	MultivariatePolynomial<C,O,P> result(p);
	substitute_inplace(result, var, value);
	return result;
}

template<typename C, typename O, typename P, typename S>
MultivariatePolynomial<C,O,P> substitute(const MultivariatePolynomial<C,O,P>& p, const std::map<Variable,S>& substitutions) {
	static_assert(!std::is_same<S, Term<C>>::value, "Terms are handled by a separate method.");
	MultivariatePolynomial<C,O,P> result;
	auto& tam = MultivariatePolynomial<C,O,P>::mTermAdditionManager;
	auto id = tam.getId(p.nrTerms());
	for (const auto& term: p) {
		Term<C> resultTerm = substitute(term, substitutions);
		if( !carl::isZero(resultTerm) )
		{
			tam.template addTerm<false>(id, resultTerm );
		}
	}
	tam.readTerms(id, result.getTerms());
	result.reset_ordered();
	result.template makeMinimallyOrdered<false, true>();
	assert(result.isConsistent());
	return result;
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> substitute(const MultivariatePolynomial<C,O,P>& p, const std::map<Variable, Term<C>>& substitutions) {
	MultivariatePolynomial<C,O,P> result;
	auto& tam = MultivariatePolynomial<C,O,P>::mTermAdditionManager;
	auto id = tam.getId(p.nrTerms());
	for (const auto& term: p) {
		tam.template addTerm<false>(id, substitute(term, substitutions));
	}
	tam.readTerms(id, result.getTerms());
	result.reset_ordered();
	result.template makeMinimallyOrdered<false, true>();
	assert(result.isConsistent());
	return result;
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> substitute(const MultivariatePolynomial<C,O,P>& p, const std::map<Variable, MultivariatePolynomial<C,O,P>>& substitutions) {
	MultivariatePolynomial<C,O,P> result(p);
	if (is_constant(p) || substitutions.empty())
	{
		return result;
	}
	// Substitute the variables, which have to be replaced by 0, beforehand,
	// as this could significantly simplify this multivariate polynomial.
	for (const auto& sub: substitutions) {
		if(carl::isZero(sub.second))
		{
			substitute_inplace(result, sub.first, sub.second);
			if(is_constant(result))
			{
                assert(result.isConsistent());
				return result;
			}
		}
	}
	// Find and sort all exponents occurring for all variables to substitute as basis.
	std::map<std::pair<Variable, exponent>, MultivariatePolynomial<C,O,P>> expResults;
	for(const auto& term: result)
	{
		if(term.monomial())
		{
			const Monomial& m = *(term.monomial());
			CARL_LOG_TRACE("carl.core.monomial", "Iterating over " << m);
			for(unsigned i = 0; i < m.nrVariables(); ++i)
			{
				CARL_LOG_TRACE("carl.core.monomial", "Iterating: " << m[i].first);
				if(m[i].second > 1 && substitutions.find(m[i].first) != substitutions.end())
				{
					expResults[m[i]] = MultivariatePolynomial<C,O,P>(constant_one<C>::get());
				}
			}
		}
	}
	// Calculate the exponentiation of the multivariate polynomial to substitute the
	// for variables for, reusing the already calculated exponentiations.
	if(!expResults.empty())
	{
		auto expResultA = expResults.begin();
		auto expResultB = expResultA;
		auto sub = substitutions.begin();
		while (sub->first != expResultB->first.first)
		{
			assert(sub != substitutions.end());
			++sub;
		}
		expResultB->second = carl::pow(sub->second, expResultB->first.second);
		++expResultB;
		while(expResultB != expResults.end())
		{
			if(expResultA->first.first != expResultB->first.first)
			{
				++sub;
				assert(sub != substitutions.end());
				// Go to the next variable.
				while (sub->first != expResultB->first.first)
				{
					assert(sub != substitutions.end());
					++sub;
				}
				assert(sub->first == expResultB->first.first);
				expResultB->second = pow(sub->second, expResultB->first.second);
			}
			else
			{
				expResultB->second = expResultA->second * pow(sub->second, expResultB->first.second-expResultA->first.second);
			}
			++expResultA;
			++expResultB;
		}
	}
	MultivariatePolynomial<C,O,P> resultB;
	// Substitute the variable for which all occurring exponentiations are calculated.
	for(const auto& term: result)
	{
		MultivariatePolynomial<C,O,P> termResult(term.coeff());
		if (term.monomial())
		{
			const Monomial& m = *(term.monomial());
			CARL_LOG_TRACE("carl.core.monomial", "Iterating over " << m);
			for(unsigned i = 0; i < m.nrVariables(); ++i)
			{
				CARL_LOG_TRACE("carl.core.monomial", "Iterating: " << m[i].first);
				if(m[i].second == 1)
				{
					auto iter = substitutions.find(m[i].first);
					if(iter != substitutions.end())
					{
						termResult *= iter->second;
					}
					else
					{
						termResult *= m[i].first;
					}
				}
				else
				{
					auto iter = expResults.find(m[i]);
					if(iter != expResults.end())
					{
						termResult *= iter->second;
					}
					else
					{
						termResult *= Term<C>(constant_one<C>::get(), m[i].first, m[i].second);
					}
				}
			}
		}
		resultB += termResult;
	}
	assert(resultB.isConsistent());
	return resultB;
}


template<typename Coeff>
void substitute_inplace(UnivariatePolynomial<Coeff>& p, Variable var, const Coeff& value) {
	if (carl::isZero(p)) return;
	if (var == p.mainVar()) {
		p = UnivariatePolynomial<Coeff>(p.mainVar(), carl::evaluate(p, value));
	} else if constexpr (!is_number<Coeff>::value) {
		// Coefficients from a polynomial ring
		if (value.has(var)) {
			// Fall back to multivariate substitution.
			MultivariatePolynomial<typename UnderlyingNumberType<Coeff>::type> tmp(p);
			substitute_inplace(tmp, var, value);
			p = carl::to_univariate_polynomial(tmp, p.mainVar());
		} else {
			// Safely substitute into each coefficient separately
			for (auto& c: p.coefficients()) {
				substitute_inplace(c, var, value);
			}
		}
	}
	p.stripLeadingZeroes();
	assert(p.isConsistent());
}

template<typename Coeff>
UnivariatePolynomial<Coeff> substitute(const UnivariatePolynomial<Coeff>& p, Variable var, const Coeff& value) {
	if constexpr (is_number<Coeff>::value) {
		if (var == p.mainVar()) {
			return UnivariatePolynomial<Coeff>(p.mainVar(), p.evaluate(value));
		}
		return p;
	} else {
		if (var == p.mainVar()) {
			UnivariatePolynomial<Coeff> res(p.mainVar());
			for (const auto& c: p.coefficients()) {
				res += substitute(c, var, value);
			}
			CARL_LOG_TRACE("carl.core.uvpolynomial", p << " [ " << var << " -> " << value << " ] = " << res);
			return res;
		} else {
			if (value.has(var)) {
				// Fall back to multivariate substitution.
				MultivariatePolynomial<typename UnderlyingNumberType<Coeff>::type> tmp(p);
				substitute_inplace(tmp, var, value);
				return to_univariate_polynomial(tmp, p.mainVar());
			} else {
				std::vector<Coeff> res(p.coefficients().size());
				for (std::size_t i = 0; i < res.size(); i++) {
					res[i] = substitute(p.coefficients()[i], var, value);
				}
				UnivariatePolynomial<Coeff> resp(p.mainVar(), res);
				resp.stripLeadingZeroes();
				CARL_LOG_TRACE("carl.core.uvpolynomial", p << " [ " << var << " -> " << value << " ] = " << resp);
				return resp;
			}
		}
	}
}

}
