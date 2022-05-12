/**
 * @file Converter.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <sstream>
#include <vector>

#include "ginac.h"

#include <carl/poly/umvpoly/Monomial.h>
#include <carl/poly/umvpoly/MultivariatePolynomial.h>
#include <carl/poly/umvpoly/Term.h>
#include <carl/core/Variable.h>
#include <carl/core/VariablePool.h>
#include <carl/numbers/numbers.h>

#include "GiNaCAdaptor.h"

namespace carl {

template<typename C>
class ConvertTo {
	C c;
public:
	test
	template<typename N>
	typename C::Number number(const N& n) {
		return c(n);
	}
	typename C::Variable variable(Variable::Arg v) {
		return c(v);
	}
	typename C::VariablePower varpower(Variable::Arg v, std::size_t exp)  {
		return c(variable(v), exp);
	}
	typename C::Monomial monomial(const Monomial::Arg& m) {
		std::vector<typename C::VariablePower> tmp;
		if (m != nullptr) {
			for (const auto& vp: *m) tmp.push_back(varpower(vp.first, vp.second));
		}
		return c(tmp);
	}
	template<typename N>
	typename C::Term term(const Term<N>& t) {
		return c(number(t.coeff()), monomial(t.monomial()));
	}
	template<typename N, typename O, typename P>
	typename C::MPolynomial mpolynomial(const MultivariatePolynomial<N,O,P>& p) {
		std::vector<typename C::Term> tmp;
		for (const auto& t: p) tmp.push_back(term(t));
		return c(tmp);
	}
};

template<typename C>
class ConvertFrom {
	C c;
public:
	template<typename N>
	typename C::Number number(const N& n) {
		return c(n);
	}
	template<typename V>
	Variable variable(const V& v) {
		return c(v);
	}
	template<typename V>
	Monomial::Arg varpower(const V& v, std::size_t exp) {
		return c(variable(v), exp);
	}
	template<typename M>
	Monomial::Arg monomial(const M& m) {
		auto tmp = c(m);
		for (const auto& t: tmp) {

		}
		return std::accumulate(tmp.begin(), tmp.end(), nullptr, std::multiplies<Monomial::Arg>());
	}
	template<typename T>
	Term<typename C::Number> term(const T& t) {
		auto tmp = c(t);
		return c(tmp.first) * c(tmp.second);
	}
	template<typename P>
	MultivariatePolynomial<typename C::Number> mpolynomial(const P& p) {
		auto tmp = c(p);
		return std::accumulate(tmp.begin(), tmp.end(), nullptr, std::plus<Term<typename C::Number>>());
	}
};

}
