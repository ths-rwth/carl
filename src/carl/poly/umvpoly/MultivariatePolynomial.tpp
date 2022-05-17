/**
 * @file MultivariatePolynomial.tpp
 * @ingroup multirp
 * @author Sebastian Junges
 */

#pragma once

#include "MultivariatePolynomial.h"

#include "Term.h"
#include "UnivariatePolynomial.h"
#include <carl-logging/carl-logging.h>
#include <carl/numbers/numbers.h>

#include <algorithm>
#include <memory>
#include <mutex>
#include <list>
#include <type_traits>

namespace carl
{

template<typename Coeff, typename Ordering, typename Policies>
TermAdditionManager<MultivariatePolynomial<Coeff,Ordering,Policies>,Ordering> MultivariatePolynomial<Coeff,Ordering,Policies>::mTermAdditionManager;

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial():
	mTerms(), mOrdered(true)
{
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const MultivariatePolynomial<Coeff, Ordering, Policies>& p):
	Policies(p),
	mTerms(p.mTerms),
	mOrdered(p.isOrdered())
{
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(MultivariatePolynomial<Coeff, Ordering, Policies>&& p):
	Policies(p),
	mTerms(std::move(p.mTerms)),
	mOrdered(p.isOrdered())
{
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator=(const MultivariatePolynomial<Coeff,Ordering,Policies>& p) {
	Policies::operator=(p);
    mTerms = p.mTerms;
	mOrdered = p.mOrdered;
	assert(this->isConsistent());
	return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator=(MultivariatePolynomial<Coeff,Ordering,Policies>&& p) noexcept {
	Policies::operator=(p);
    mTerms = std::move(p.mTerms);
	mOrdered = p.mOrdered;
	assert(this->isConsistent());
	return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename C>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(EnableIfNotSame<C,sint> c) : MultivariatePolynomial(from_int<Coeff>(c))
{
	mOrdered = true;
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename C>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(EnableIfNotSame<C,uint> c) : MultivariatePolynomial(from_int<Coeff>(c))
{
	mOrdered = true;
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const Coeff& c) :
	Policies(),
	mTerms(c == Coeff(0) ? 0 : 1, Term<Coeff>(c)),
	mOrdered(true)
{
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(Variable::Arg v) :
	Policies(),
	mTerms(1,Term<Coeff>(v)),
	mOrdered(true)
{
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const Monomial::Arg& m) :
	Policies(),
	mTerms(1,Term<Coeff>(m)),
	mOrdered(true)
{
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const Term<Coeff>& t) :
	Policies(),
	mTerms(1,t),
	mOrdered(true)
{
	if (carl::is_zero(t)) {
		this->mTerms.clear();
	}
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const UnivariatePolynomial<MultivariatePolynomial<Coeff, Ordering, Policies>>& p):
	Policies(),
	mTerms(),
	mOrdered(false)
{
	auto id = mTermAdditionManager.getId();
	exponent exp = 0;
	for (const auto& c: p.coefficients()) {
		if (exp == 0) {
			for (const auto& term: c) mTermAdditionManager.template addTerm<true>(id, term);
		} else {
			for (const auto& term: c * Term<Coeff>(constant_one<Coeff>::get(), p.mainVar(), exp)) {
				mTermAdditionManager.template addTerm<true>(id, term);
			}
		}
		exp++;
	}
	mTermAdditionManager.readTerms(id, mTerms);
	makeMinimallyOrdered<false, true>();
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const UnivariatePolynomial<Coeff>& p) :
	Policies(),
	mTerms(),
	mOrdered(true)
{
	exponent exp = 0;
	mTerms.reserve(p.degree());
	for (const auto& c: p.coefficients()) {
		if (!carl::is_zero(c)) {
			if (exp == 0) mTerms.emplace_back(c);
			else mTerms.emplace_back(c, p.mainVar(), exp);
		}
		exp++;
	}
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename OtherPolicies, DisableIf<std::is_same<Policies,OtherPolicies>>>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const MultivariatePolynomial<Coeff, Ordering, OtherPolicies>& pol) :
	Policies(),
	mTerms(pol.begin(), pol.end()),
	mOrdered(pol.isOrdered())
{
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
#ifdef __VS
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(TermsType&& terms, bool duplicates, bool ordered) :
#else
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(MultivariatePolynomial<Coeff, Ordering, Policies>::TermsType&& terms, bool duplicates, bool ordered):
#endif
	mTerms(std::move(terms)),
	mOrdered(ordered)
{
	if( duplicates ) {
		auto id = mTermAdditionManager.getId(mTerms.size());
		for (const auto& t: mTerms) mTermAdditionManager.template addTerm<false>(id, t);
		mTermAdditionManager.readTerms(id, mTerms);
		mOrdered = false;
	}

	if (!mOrdered) {
		makeMinimallyOrdered();
	}

	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
#ifdef __VS
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(const TermsType& terms, bool duplicates, bool ordered) :
#else
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(const MultivariatePolynomial<Coeff, Ordering, Policies>::TermsType& terms, bool duplicates, bool ordered) :
#endif
	mTerms(terms),
	mOrdered(ordered)
{
	if( duplicates ) {
		auto id = mTermAdditionManager.getId(mTerms.size());
		for (const auto& t: mTerms) {
			mTermAdditionManager.template addTerm<false>(id, t);
		}
		mTermAdditionManager.readTerms(id, mTerms);
	}
	if (!ordered) {
		makeMinimallyOrdered();
	}
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(const std::initializer_list<Term<Coeff>>& terms):
	Policies(),
	mTerms(terms),
	mOrdered(false)
{
	makeMinimallyOrdered();
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(const std::initializer_list<Variable>& terms):
	mTerms(),
	mOrdered(false)
{
	for (const Variable& t: terms) {
		mTerms.emplace_back(t);
	}
	makeMinimallyOrdered();
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(const std::pair<ConstructorOperation, std::vector<MultivariatePolynomial>>& p)
	: MultivariatePolynomial(p.first, p.second)
{
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(ConstructorOperation op, const std::vector<MultivariatePolynomial>& operands):
	mTerms(),
	mOrdered(false)
{
	assert(!operands.empty());
	auto it = operands.begin();
	*this = *it;
	if ((op == ConstructorOperation::SUB) && (operands.size() == 1)) {
		// special treatment of unary minus
		*this *= -1;
        assert(this->isConsistent());
		return;
	}
	if (op == ConstructorOperation::DIV) {
		// division shall have at least two arguments
		assert(operands.size() >= 2);
	}
	for (it++; it != operands.end(); it++) {
	switch (op) {
		case ConstructorOperation::ADD: *this += *it; break;
		case ConstructorOperation::SUB: *this -= *it; break;
		case ConstructorOperation::MUL: *this *= *it; break;
		case ConstructorOperation::DIV:
			assert(it->isConstant());
			*this /= it->constantPart();
			break;
		}
	}
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
std::size_t MultivariatePolynomial<Coeff,Ordering,Policies>::totalDegree() const
{
	if (carl::is_zero(*this)) return 0;
	assert(!mTerms.empty());
	if (Ordering::degreeOrder) {
		return this->lterm().tdeg();
	} else {
		CARL_LOG_NOTIMPLEMENTED();
	}
}

template<typename Coeff, typename Ordering, typename Policies>
const Coeff& MultivariatePolynomial<Coeff, Ordering, Policies>::constantPart() const
{
	if(carl::is_zero(*this)) return constant_zero<Coeff>::get();
	if(trailingTerm().isConstant()) {
		return trailingTerm().coeff();
	}
	return constant_zero<Coeff>::get();
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isLinear() const {
	if (carl::is_zero(*this)) return true;
	if (Ordering::degreeOrder) {
		return lterm().isLinear();
	} else {
		return std::all_of(mTerms.begin(), mTerms.end(),
			[](const auto& t){ return t.isLinear(); }
		);
	}
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::tail(bool makeFullyOrdered) const
{
	assert(!mTerms.empty());
	assert(this->isConsistent());
	if (mTerms.size() == 1) return MultivariatePolynomial();
	MultivariatePolynomial tail;
	tail.mTerms.reserve(mTerms.size()-1);
	tail.mTerms.insert(tail.mTerms.begin(), mTerms.begin(), --mTerms.end());
	if(isOrdered())
	{
		assert(tail.mOrdered);
	}
	else
	{
		tail.mOrdered = false;
		if(makeFullyOrdered)
		{
			tail.makeOrdered();
		}
		else
		{
			tail.makeMinimallyOrdered();
		}
	}
	assert(tail.isConsistent());
	return tail;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::stripLT()
{
	assert(!carl::is_zero(*this));
	mTerms.pop_back();
	if (!isOrdered()) makeMinimallyOrdered<false, true>();
	assert(this->isConsistent());
	return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isUnivariate() const {
	// A constant polynomial is obviously univariate.
	if (isConstant()) return true;

	if (this->lterm().getNrVariables() > 1) {
		return false;
	}

	Variable v = lterm().getSingleVariable();
	for (const auto& term : mTerms) {
		if (!term.hasNoOtherVariable(v)) return false;
	}
	return true;
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isReducibleIdentity() const
{
	CARL_LOG_NOTIMPLEMENTED();
	return false;
}

template<typename Coeff, typename Ordering, typename Policies>
void MultivariatePolynomial<Coeff,Ordering,Policies>::subtractProduct(const Term<Coeff>& factor, const MultivariatePolynomial<Coeff,Ordering,Policies> & p) {
	assert(this->isConsistent());
	assert(p.isConsistent());
	if (carl::is_zero(p)) return;
	if (carl::is_zero(*this)) {
		*this = - factor * p;
        assert(this->isConsistent());
	}
	if (carl::is_zero(factor.coeff())) return;
	if (p.nrTerms() == 1) {
		for (const auto& t: p) {
			this->addTerm(- factor * t);
		}
		assert(isConsistent());
		return;
	}

	auto id = mTermAdditionManager.getId(mTerms.size() + p.mTerms.size());
	for (const auto& term: mTerms) {
		mTermAdditionManager.template addTerm<false>(id, term);
	}
	for (const auto& term: p.mTerms) {
		Coeff c = - factor.coeff() * term.coeff();
		auto m = factor.monomial() * term.monomial();
		mTermAdditionManager.template addTerm<false>(id, TermType(c, m));
	}
	mTermAdditionManager.readTerms(id, mTerms);
	mOrdered = false;
	makeMinimallyOrdered<false, true>();
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
void MultivariatePolynomial<Coeff,Ordering,Policies>::addTerm(const Term<Coeff>& term) {
	//std::cout << *this << " + " << term << std::endl;
	if (term.isConstant()) {
		if (hasConstantTerm()) {
			trailingTerm().coeff() += term.coeff();
			if (carl::is_zero(trailingTerm().coeff())) {
				mTerms.erase(mTerms.begin());
			}
		} else mTerms.insert(mTerms.begin(), term);
		return;
	}
	if (mOrdered) {
		auto it = mTerms.begin();
		for (; it != mTerms.end(); it++) {
			CompareResult res = Ordering::compare(term, *it);
			switch (res) {
			case CompareResult::LESS: break;
			case CompareResult::EQUAL: {
				it->coeff() += term.coeff();
				if (carl::is_zero(it->coeff())) {
					mTerms.erase(it);
				}
				return;
			}
			case CompareResult::GREATER: ;
			}
		}
		mTerms.insert(it, term);
	} else {
		switch (Ordering::compare(lterm(), term)) {
		case CompareResult::LESS: {
			mTerms.push_back(term);
			break;
		}
		case CompareResult::EQUAL: {
			lterm().coeff() += term.coeff();
			if (carl::is_zero(lterm().coeff())) {
				mTerms.pop_back();
				makeMinimallyOrdered<false,true>();
			}
			break;
		}
		case CompareResult::GREATER: {
			for (auto it = mTerms.begin(); it != mTerms.end(); it++) {
				if (Ordering::equal(*it, term)) {
					it->coeff() += term.coeff();
					if (carl::is_zero(it->coeff())) {
						mTerms.erase(it);
					}
					return;
				}
			}
			auto lt = mTerms.back();
			mTerms.push_back(term);
			std::swap(lt, mTerms.back());
		}
		}
	}
}

template<typename Coeff, typename Ordering, typename Policies>
Coeff MultivariatePolynomial<Coeff,Ordering,Policies>::coprimeFactor() const
{
	assert(!carl::is_zero(*this));

	if constexpr (carl::is_subset_of_rationals_type<Coeff>::value) {
		auto it = begin();
		auto num = carl::abs(carl::get_num(it->coeff()));
		auto den = carl::abs(carl::get_denom(it->coeff()));
		for (++it; it != end(); ++it) {
			num = carl::gcd(num, carl::abs(carl::get_num(it->coeff())));
			den = carl::lcm(den, carl::abs(carl::get_denom(it->coeff())));
		}
		if (lcoeff() < 0) {
			den = -den;
		}
		return Coeff(den) / num;
	} else {
		return Coeff(1);
	}
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename C, EnableIf<is_subset_of_rationals_type<C>>>
Coeff MultivariatePolynomial<Coeff,Ordering,Policies>::coprimeFactorWithoutConstant() const
{
	assert(!carl::is_zero(*this));
	typename TermsType::const_iterator it = mTerms.begin();
	if (it->isConstant()) ++it;
	typename IntegralType<Coeff>::type num = carl::abs(get_num((it)->coeff()));
	typename IntegralType<Coeff>::type den = carl::abs(get_denom((it)->coeff()));
	for(++it; it != mTerms.end(); ++it)
	{
		num = carl::gcd(num, get_num((it)->coeff()));
		den = carl::lcm(den, get_denom((it)->coeff()));
	}
    if( carl::is_negative(lcoeff()) )
    {
        return Coeff(den)/(-num);
    }
    else
    {
        return Coeff(den)/num;
    }
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::coprimeCoefficients() const
{
	if(carl::is_zero(*this)) return *this;
	Coeff factor = coprimeFactor();
	// Notice that even if factor is 1, we create a new polynomial
	MultivariatePolynomial<Coeff, Ordering, Policies> result;
	result.mTerms.reserve(mTerms.size());
	for (const auto& term: mTerms)
	{
		result.mTerms.push_back(term * factor);
	}
    result.mOrdered = mOrdered;
    assert(result.isConsistent());
	return result;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::coprimeCoefficientsSignPreserving() const
{
	if(nrTerms() == 0) return *this;
	Coeff factor = carl::abs(coprimeFactor());
	// Notice that even if factor is 1, we create a new polynomial
	MultivariatePolynomial<Coeff, Ordering, Policies> result;
	result.mTerms.reserve(mTerms.size());
	for (const auto& term: mTerms)
	{
		result.mTerms.push_back(term * factor);
	}
    result.mOrdered = mOrdered;
    assert(result.isConsistent());
	return result;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::normalize() const
{
	MultivariatePolynomial result(*this);
	if(Policies::has_reasons)
	{
		result.setReasons(this->getReasons());
	}
    if (carl::is_zero(*this)) return result;
	auto lc = lcoeff();
	for (auto& it: result.mTerms)
	{
		it.coeff() /= lc;
	}
	assert(result.isConsistent());
	return result;
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::sqrt(MultivariatePolynomial<Coeff,Ordering,Policies>& res) const
{
    if (carl::is_zero(*this)) {
        res = *this;
        return true;
    } else if (mTerms.size() == 1) {
        Term<Coeff> t;
        if (mTerms.back().sqrt(t)) {
            res = MultivariatePolynomial(t);
            return true;
        }
    }
    return false;
}

template<typename Coeff, typename Ordering, typename Policies>
template<bool gatherCoeff>
VariableInformation<gatherCoeff, MultivariatePolynomial<Coeff,Ordering,Policies>> MultivariatePolynomial<Coeff,Ordering,Policies>::getVarInfo(Variable::Arg v) const
{
	VariableInformation<gatherCoeff, MultivariatePolynomial> varinfomap;
	// We iterate over all terms.
	for(const auto& term : mTerms)
	{
		// And gather information from the terms and meanwhile up
		term.gatherVarInfo(v, varinfomap);
	}
	return varinfomap;
}

template<typename Coeff, typename Ordering, typename Policies>
template<bool gatherCoeff>
VariablesInformation<gatherCoeff, MultivariatePolynomial<Coeff,Ordering,Policies>> MultivariatePolynomial<Coeff,Ordering,Policies>::getVarInfo() const
{
	VariablesInformation<gatherCoeff, MultivariatePolynomial> varinfomap;
	// We iterate over all terms.
	for(const auto& term : mTerms)
	{
		// And gather information from the terms and meanwhile up
		term.gatherVarInfo(varinfomap);
	}
	return varinfomap;
}

template<typename Coeff, typename O, typename P>
template<typename C, EnableIf<is_number_type<C>>>
Coeff MultivariatePolynomial<Coeff,O,P>::numericContent() const
{
	if (carl::is_zero(*this)) return 0;
	typename UnderlyingNumberType<C>::type res = this->mTerms.front().coeff();
	for (unsigned i = 0; i < this->mTerms.size(); i++) {
		///@todo gcd needed for fractions
		res = carl::gcd(res, this->mTerms[i].coeff());
		//assert(false);
	}
	return res;
}

template<typename Coeff, typename O, typename P>
template<typename C, DisableIf<is_number_type<C>>>
typename UnderlyingNumberType<C>::type MultivariatePolynomial<Coeff,O,P>::numericContent() const
{
	if (carl::is_zero(*this)) return 0;
	typename UnderlyingNumberType<C>::type res = this->mTerms.front()->coeff().numericContent();
	for (const auto& t: mTerms) {
		res = gcd(res, t->coeff().numericContent());
	}
	return res;
}

template<typename Coeff, typename O, typename P>
template<typename C, EnableIf<is_number_type<C>>>
typename MultivariatePolynomial<Coeff,O,P>::IntNumberType MultivariatePolynomial<Coeff,O,P>::mainDenom() const {
	IntNumberType res = 1;
	for (const auto& t: mTerms) {
		res = carl::lcm(res, get_denom(t.coeff()));
	}
	return res;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const MultivariatePolynomial& rhs)
{
	assert(this->isConsistent());
	assert(rhs.isConsistent());
	if (mTerms.empty())  	{
		*this = rhs;
        assert(this->isConsistent());
		return *this;
	}
	if (rhs.mTerms.empty()) return *this;
	if (rhs.isConstant()) {
		return *this += rhs.constantPart();
	}
	if (this->isConstant()) {
		Coeff c = constantPart();
		*this = rhs;
		return *this += c;
	}
	TermType newlterm;
	CompareResult res = Ordering::compare(lterm().monomial(), rhs.lterm().monomial());
    auto rhsEnd = rhs.mTerms.end();
	if (res == CompareResult::GREATER) {
		newlterm = std::move( mTerms.back() );
        mTerms.pop_back();
	} else if (res == CompareResult::LESS) {
		newlterm = rhs.lterm();
		--rhsEnd;
	} else {
		mTerms.back().coeff() += rhs.lcoeff();
		newlterm = std::move( mTerms.back() );
        mTerms.pop_back();
		--rhsEnd;
	}
	auto id = mTermAdditionManager.getId(mTerms.size() + rhs.mTerms.size());
	for (auto termIter = mTerms.begin(); termIter != mTerms.end(); ++termIter) {
		mTermAdditionManager.template addTerm<false,false>(id, *termIter);
	}
	for (auto termIter = rhs.mTerms.begin(); termIter != rhsEnd; ++termIter) {
		mTermAdditionManager.template addTerm<false,false>(id, *termIter);
	}
	mTermAdditionManager.readTerms(id, mTerms);
	if (carl::is_zero(newlterm)) {
		makeMinimallyOrdered<false,true>();
	} else {
		mTerms.push_back(newlterm);
	}
	mOrdered = false;
	assert(this->isConsistent());
	assert(rhs.isConsistent());
	return *this;
}


/*template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const TermType& rhs) {
	return *this += std::make_shared<const TermType>(rhs);
}*/

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const TermType& rhs)
{
	assert(this->isConsistent());
	if (carl::is_zero(rhs.coeff())) return *this;
	if (mTerms.empty()) {
		// Empty -> just insert.
		mTerms.push_back(rhs);
		mOrdered = true;
	} else if (rhs.isConstant()) {
		if (this->hasConstantTerm()) {
			// Combine with constant term.
			if (carl::is_zero(Coeff(mTerms.front().coeff() + rhs.coeff()))) {
				mTerms.erase(mTerms.begin());
			} else mTerms.front() = Term<Coeff>(mTerms.front().coeff() + rhs.coeff(), nullptr);
		} else if (mTerms.size() == 1) {
			// Only a single term, insert and swap.
			mTerms.push_back(rhs);
			std::swap(mTerms[0], mTerms[1]);
		} else {
			assert(mTerms.size() > 1);
			// New constant term. Add at the end and swap to correct position.
			mTerms.push_back(rhs);
			std::swap(mTerms.front(), mTerms.back());
			assert(rhs == mTerms.front());
			std::swap(mTerms.back(), *(mTerms.end()-2));
			mOrdered = false;
		}
	} else if (Term<Coeff>::monomialEqual(lterm(), rhs)) {
		// Combine with leading term.
		if (carl::is_zero(Coeff(lcoeff() + rhs.coeff()))) {
			mTerms.pop_back();
			makeMinimallyOrdered<false, true>();
		} else mTerms.back() = Term<Coeff>(lcoeff() + rhs.coeff(), rhs.monomial());
	} else if (Ordering::less(lterm(), rhs)) {
		// New leading term.
		mTerms.push_back(rhs);
	} else {
		// Full-blown addition.
		auto id = mTermAdditionManager.getId(mTerms.size()+1);
		for (const auto& term: mTerms) {
			mTermAdditionManager.template addTerm<false>(id, term);
		}
		mTermAdditionManager.template addTerm<false>(id, rhs);
		mTermAdditionManager.readTerms(id, mTerms);
		makeMinimallyOrdered<false, true>();
		mOrdered = false;
	}
	assert(this->isConsistent());
	return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const Monomial::Arg& rhs)
{
	if (mTerms.empty()) {
		// Empty -> just insert.
		mTerms.emplace_back(constant_one<Coeff>::get(), rhs);
		mOrdered = true;
	} else if (lterm().isConstant()) {
		mTerms.emplace_back(constant_one<Coeff>::get(), rhs);
	} else if (lmon() == rhs) {
		// Combine with leading term.
		if (lcoeff() == -carl::constant_one<Coeff>().get()) {
			mTerms.pop_back();
			makeMinimallyOrdered<false, true>();
		} else {
			mTerms.back() = Term<Coeff>(lcoeff() + constant_one<Coeff>::get(), lmon());
		}
	} else if (Ordering::less(lmon(),rhs)) {
		// New leading term.
		mTerms.emplace_back(rhs);
	} else {
		///@todo insert at correct position if already ordered
		auto it = mTerms.begin();
		for (; it != mTerms.end(); it++) {
			if ((*it).monomial() == rhs) {
				if ((*it).coeff() == -carl::constant_one<Coeff>().get()) {
					mTerms.erase(it);
				} else {
					*it = Term<Coeff>((*it).coeff() + constant_one<Coeff>::get(), (*it).monomial());
				}
				break;
			}
		}
		if (it == mTerms.end()) {
			mTerms.emplace_back(constant_one<Coeff>::get(), rhs);
			std::swap(mTerms[mTerms.size()-2], mTerms[mTerms.size()-1]);
		}
		mOrdered = false;
	}
	assert(this->isConsistent());
	return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(Variable rhs)
{
	return *this += createMonomial(rhs, 1);
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const Coeff& rhs)
{
	if (carl::is_zero(rhs)) return *this;
	if (hasConstantTerm()) {
		if (carl::is_zero(Coeff(constantPart() + rhs))) mTerms.erase(mTerms.begin());
		else mTerms.front() = TermType(constantPart() + rhs);
	} else {
		mTerms.emplace(mTerms.begin(), rhs);
	}
	assert(this->isConsistent());
	return *this;
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> operator+(const UnivariatePolynomial<C>&, const MultivariatePolynomial<C,O,P>&)
{
	CARL_LOG_NOTIMPLEMENTED();
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>&, const UnivariatePolynomial<C>&)
{
	CARL_LOG_NOTIMPLEMENTED();
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> operator+(const UnivariatePolynomial<MultivariatePolynomial<C>>&, const MultivariatePolynomial<C,O,P>&)
{
	CARL_LOG_NOTIMPLEMENTED();
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>&, const UnivariatePolynomial<MultivariatePolynomial<C>>&)
{
	CARL_LOG_NOTIMPLEMENTED();
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::operator -() const
{
	assert(this->isConsistent());
	MultivariatePolynomial<Coeff, Ordering, Policies> negation;
	negation.mTerms.reserve(mTerms.size());
	for (const auto& term: mTerms) {
		negation.mTerms.push_back(-term);
	}
	negation.mOrdered = this->mOrdered;
	assert(negation.isConsistent());
	return negation;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator-=(const MultivariatePolynomial& rhs)
{
	assert(this->isConsistent());
	assert(rhs.isConsistent());
	if(mTerms.empty())
	{
		*this = -rhs;
        assert(this->isConsistent());
		return *this;
	}
	if(rhs.mTerms.empty()) return *this;
	if (rhs.isConstant()) {
		return *this -= rhs.constantPart();
	}
	if (this->isConstant()) {
		Coeff c = constantPart();
		*this = -rhs;
		return *this += c;
	}

	auto id = mTermAdditionManager.getId(mTerms.size() + rhs.mTerms.size());
	for (const auto& term: mTerms) {
		mTermAdditionManager.template addTerm<false>(id, term);
	}
	for (const auto& term: rhs.mTerms) {
		mTermAdditionManager.template addTerm<false>(id, -term);
	}
	mTermAdditionManager.readTerms(id, mTerms);
	mOrdered = false;
	makeMinimallyOrdered<false, true>();
	assert(this->isConsistent());
	return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator-=(const Term<Coeff>& rhs)
{
	///@todo Check if this works with ordering.
	if (carl::is_zero(rhs.coeff())) return *this;
	CARL_LOG_TRACE("carl.core", *this << " -= " << rhs);
	if (Policies::searchLinear)
	{
		auto it = mTerms.begin();
		while(it != mTerms.end())
		{
			// TODO consider comparing the shared pointers.
			if ((*it).monomial() != nullptr) {
				CompareResult cmpres(Ordering::compare((*it), rhs));
				if( mOrdered && cmpres == CompareResult::GREATER ) break;
				if( cmpres == CompareResult::EQUAL )
				{
					// new coefficient would be zero, simply removing is enough.
					if((*it).coeff() == rhs.coeff())
					{
						it = mTerms.erase(it);
						if (it == mTerms.end()) {
							// We removed the leading term.
							makeMinimallyOrdered<false,true>();
						}
					}
					// we have to create a new term object.
					else
					{
						it->coeff() -= rhs.coeff();
					}
					CARL_LOG_TRACE("carl.core", "-> " << *this);
					assert(this->isConsistent());
					return *this;
				}
			}
			++it;
		}
		// no equal monomial does occur. We can simply insert.
		mTerms.insert(it,-rhs);
		makeMinimallyOrdered<false,true>();
		CARL_LOG_TRACE("carl.core", "-> " << *this);
		assert(this->isConsistent());
		return *this;
	}
	else
	{
		CARL_LOG_NOTIMPLEMENTED();
	}
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator-=(const Monomial::Arg& rhs)
{
	///@todo Check if this works with ordering.
	if (!rhs)
	{
		*this -= constant_one<Coeff>::get();
        assert(this->isConsistent());
		return *this;
	}
	if (Policies::searchLinear)
	{
		auto it = mTerms.begin();
		while(it != mTerms.end())
		{
			if ((*it).monomial() != nullptr) {
				CompareResult cmpres(Ordering::compare((*it).monomial(), rhs));
				if( mOrdered && cmpres == CompareResult::GREATER ) break;
				if( cmpres == CompareResult::EQUAL )
				{
					// new coefficient would be zero, simply removing is enough.
					if(carl::is_one((*it).coeff()))
					{
						it = mTerms.erase(it);
						if (it == mTerms.end()) {
							// We removed the leading term.
							makeMinimallyOrdered<false,true>();
						}
					}
					// we have to create a new term object.
					else
					{
						it->coeff() -= constant_one<Coeff>::get();
					}
					assert(this->isConsistent());
					return *this;
				}
			}
			++it;
		}
		// no equal monomial does occur. We can simply insert.
		mTerms.insert(it,Term<Coeff>(-carl::constant_one<Coeff>().get(), rhs));
		makeMinimallyOrdered<false,true>();
		assert(this->isConsistent());
		return *this;
	}
	else
	{
		CARL_LOG_NOTIMPLEMENTED();
	}
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator-=(Variable::Arg rhs)
{
	return *this += Term<Coeff>(-carl::constant_one<Coeff>().get(), rhs, 1);
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator-=(const Coeff& rhs)
{
	return *this += (-rhs);
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator*=(const MultivariatePolynomial<Coeff,Ordering,Policies>& rhs)
{
	assert(this->isConsistent());
	assert(rhs.isConsistent());
	if(mTerms.empty()) return *this;
	if(rhs.mTerms.empty()) {
		mTerms.clear();
        assert(this->isConsistent());
		return *this;
	}
	if (rhs.isConstant()) {
		return *this *= rhs.constantPart();
	}
	if (carl::is_one(*this)) {
		return *this = rhs;
	}
	if (this->isConstant()) {
		Coeff c = constantPart();
		*this = rhs;
		return *this *= c;
	}
	auto id = mTermAdditionManager.getId(mTerms.size() * rhs.mTerms.size());
	TermType newlterm;
	bool first = true;
	for (auto t1 = mTerms.rbegin(); t1 != mTerms.rend(); t1++) {
		for (auto t2 = rhs.mTerms.rbegin(); t2 != rhs.mTerms.rend(); t2++) {
			if (first) {
				newlterm = *t1 * *t2;
				first = false;
			} else mTermAdditionManager.template addTerm<false>(id, std::move((*t1)*(*t2)));
		}
	}
	mTermAdditionManager.readTerms(id, mTerms);
	if (carl::is_zero(newlterm)) makeMinimallyOrdered<false, true>();
	else mTerms.push_back(newlterm);
	//makeMinimallyOrdered<false, true>();
	mOrdered = false;
	assert(this->isConsistent());
	return *this;
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator*=(const Term<Coeff>& rhs)
{
	assert(this->isConsistent());
	///@todo more efficient.
	TermsType newTerms;
	newTerms.reserve(mTerms.size());
	for(const auto& term : mTerms)
	{
		newTerms.push_back(term * rhs);
	}
	mTerms = std::move(newTerms);
	assert(this->isConsistent());
	return *this;
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator*=(const Monomial::Arg& rhs)
{
	assert(this->isConsistent());
	///@todo more efficient.
	TermsType newTerms;
	newTerms.reserve(mTerms.size());
	for(const auto& term : mTerms)
	{
		newTerms.push_back(term * rhs);
	}
	mTerms = std::move(newTerms);
	assert(this->isConsistent());
	return *this;
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator*=(Variable::Arg rhs)
{
	///@todo more efficient.
	TermsType newTerms;
	newTerms.reserve(mTerms.size());
	for(const auto& term : mTerms)
	{
		newTerms.push_back(term * rhs);
	}
	mTerms = std::move(newTerms);
	assert(this->isConsistent());
	return *this;
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator*=(const Coeff& rhs)
{
	///@todo more efficient.
	if (carl::is_one(rhs)) return *this;
	if (carl::is_zero(rhs))
	{
		mTerms.clear();
        assert(this->isConsistent());
		return *this;
	}
	TermsType newTerms;
	newTerms.reserve(mTerms.size());
	for(const auto& term : mTerms)
	{
		newTerms.push_back(term * rhs);
	}
	mTerms = std::move(newTerms);
	assert(this->isConsistent());
	return *this;
}

template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator*(const UnivariatePolynomial<C>&, const MultivariatePolynomial<C,O,P>&)
{
	CARL_LOG_NOTIMPLEMENTED();
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs * lhs;
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator/=(const Coeff& rhs)
{
	assert(!carl::is_zero(rhs));
	if (carl::is_one(rhs)) return *this;
	for (auto& term : mTerms) {
		term.coeff() /= rhs;
	}
	return *this;
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> operator/(const MultivariatePolynomial<C,O,P>& lhs, unsigned long rhs)
{
	MultivariatePolynomial<C,O,P> result;
	for (const auto& t: lhs.mTerms) {
		result.mTerms.emplace_back(t/rhs);
	}
	result.mOrdered = lhs.mOrdered;
    assert(result.isConsistent());
	return result;
}

template<typename Coeff, typename Ordering, typename Policies>
template<bool findConstantTerm, bool findLeadingTerm>
void MultivariatePolynomial<Coeff, Ordering, Policies>::makeMinimallyOrdered() const {
	static_assert(findLeadingTerm, "We do not have implementations of makeMinimallyOrdered() which assume a given leading term position.");
	if(findConstantTerm)
	{
		if (mTerms.size() < 2) return;
		auto it = mTerms.begin();
		auto lTerm = it;
		auto constTerm = mTerms.end();
		if ((*it).isConstant()) constTerm = it;

		for (it++; it != mTerms.end();) {
			if ((*it).isConstant()) {
				assert(constTerm == mTerms.end());
				constTerm = it;
			} else if (Ordering::less(*lTerm, *it)) {
				lTerm = it;
			} else {
				assert(!Term<Coeff>::monomialEqual(*lTerm, *it));
			}
			it++;
		}
		makeMinimallyOrdered(lTerm, constTerm);
	}
	else
	{
		if (mTerms.size() < 2) return;
		auto it = mTerms.begin();
		const auto itToLast = mTerms.end() - 1;
		auto lTerm = it;

		for (it++; it != itToLast; ++it) {
			if (Ordering::less(*lTerm, *it)) {
				lTerm = it;
			} else {
				assert(!Term<Coeff>::monomialEqual(*lTerm, *it));
			}
		}

		assert(!Term<Coeff>::monomialEqual(*lTerm, *itToLast));
		if (!Ordering::less(*lTerm, *itToLast)) {
			std::swap(*lTerm, mTerms.back());
		}
	}
}


template<typename Coeff, typename Ordering, typename Policies>
void MultivariatePolynomial<Coeff, Ordering, Policies>::makeMinimallyOrdered(typename TermsType::iterator& lterm, typename TermsType::iterator& cterm) const {
	if (cterm != mTerms.end()) {
		// Prevent that the swaps cancel each other.
		if (lterm == mTerms.begin()) lterm = cterm;
		std::swap(*cterm, mTerms.front());
	}
	std::swap(*lterm, mTerms.back());
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff, Ordering, Policies>::isConsistent() const {
	std::set<Monomial::Arg> monomials;
	for (unsigned i = 0; i < this->mTerms.size(); i++) {
		//assert(this->mTerms[i]);
		assert(!carl::is_zero(mTerms[i]));
		if (i > 0) {
			assert(this->mTerms[i].tdeg() > 0);
		}
		auto it = monomials.insert(mTerms[i].monomial());
		assert(it.second);
	}
	if (mOrdered) {
		for (unsigned i = 1; i < this->mTerms.size(); i++) {
			if (!Ordering::less(mTerms[i-1], mTerms[i])) {
				CARL_LOG_ERROR("carl.core", "Ordering error for " << *this << ": " << mTerms[i-1] << " < " << mTerms[i]);
			}
			assert(Ordering::less(mTerms[i-1], mTerms[i]));
		}
	}
	else
	{
		if(nrTerms()> 1)
		{
			for (unsigned i = 0; i < this->mTerms.size() - 1; i++) {
				if (!Ordering::less(mTerms[i], lterm())) {
					CARL_LOG_ERROR("carl.core", "Ordering error for " << *this << ": " << mTerms[i] << " < " << lterm());
				}
				assert(Ordering::less(mTerms[i], lterm()));
			}
		}
	}
	return true;
}

}
