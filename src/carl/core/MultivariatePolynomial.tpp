/**
 * @file MultivariatePolynomial.tpp
 * @ingroup multirp
 * @author Sebastian Junges
 */

#pragma once
#include "MultivariatePolynomial.h"
#include <algorithm>
#include <memory>
#include <list>
#include <type_traits>

#include "Term.h"
#include "UnivariatePolynomial.h"
#include "logging.h"
#include "../numbers/numbers.h"

namespace carl
{
	
template<typename Coeff, typename Ordering, typename Policies>
TermAdditionManager<MultivariatePolynomial<Coeff,Ordering,Policies>> MultivariatePolynomial<Coeff,Ordering,Policies>::mTermAdditionManager;

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial()
{
	mOrdered = true;
	assert(this->isConsistent());
}
   
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(int c) : MultivariatePolynomial((Coeff)c)
{
	mOrdered = true;
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(unsigned c) : MultivariatePolynomial((Coeff)c)
{
	mOrdered = true;
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const Coeff& c) :
Policies(),
mTerms(c == (Coeff)0 ? 0 : 1,std::make_shared<const Term<Coeff>>(c))
{
	mOrdered = true;
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(Variable::Arg v) :
Policies(),
mTerms(1,std::make_shared<const Term<Coeff>>(v))
{
	mOrdered = true;
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const Monomial::Arg& m) :
Policies(),
mTerms(1,std::make_shared<const Term<Coeff>>(m))
{
	mOrdered = true;
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const Term<Coeff>& t) :
Policies(),
mTerms(1,std::make_shared<const Term<Coeff>>(t))
{
	if (t.isZero()) {
		this->mTerms.clear();
	}
	mOrdered = true;
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const std::shared_ptr<const Term<Coeff>>& t) :
Policies(),
mTerms(1,t)
{
	if (t->isZero()) {
		this->mTerms.clear();
	}
	mOrdered = true;
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const UnivariatePolynomial<MultivariatePolynomial<Coeff, Ordering, Policies>>& p) :
Policies()
{
#ifdef USE_MONOMIAL_POOL
	std::size_t id = mTermAdditionManager.getId();
	exponent exp = 0;
	for (const auto& c: p.coefficients()) {
		if (exp == 0) {
			for (const auto& term: c) mTermAdditionManager.addTerm(id, term);
		} else {
			for (const auto& term: c * Term<Coeff>(1, p.mainVar(), exp)) {
				mTermAdditionManager.addTerm(id, term);
			}
		}
		exp++;
	}
	mTermAdditionManager.readTerms(id, mTerms);
#else
	std::size_t terms = 0;
	for (const auto& c: p.coefficients()) terms += c.nrTerms();
	std::size_t id = mTermAdditionManager.getTermMapId(*this, terms);
	exponent exp = 0;
	for (const auto& c: p.coefficients()) {
		if (exp == 0) {
			for (const auto& term: c) mTermAdditionManager.addTerm(*this, id, term);
		} else {
			for (const auto& term: c * Term<Coeff>(1, p.mainVar(), exp)) {
				mTermAdditionManager.addTerm(*this, id, term);
			}
		}
		exp++;
	}
	mTermAdditionManager.readTerms(*this, id, mTerms);
#endif
	makeMinimallyOrdered<false, true>();
	mOrdered = false;
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const UnivariatePolynomial<Coeff>& p) :
Policies()
{
	exponent exp = 0;
	mTerms.reserve(p.degree());
	for (const auto& c: p.coefficients()) {
		if (c != constant_zero<Coeff>::get()) {
			if (exp == 0) mTerms.emplace_back(new Term<Coeff>(c));
			else mTerms.emplace_back(new Term<Coeff>(c, p.mainVar(), exp));
		}
		exp++;
	}
	mOrdered = true;
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename OtherPolicies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const MultivariatePolynomial<Coeff, Ordering, OtherPolicies>& pol) :
	Policies(),
	mTerms(pol.begin(), pol.end()),
	mOrdered(pol.isOrdered())
{
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(MultivariatePolynomial<Coeff, Ordering, Policies>::TermsType&& terms, bool duplicates, bool ordered):
	mTerms(std::move(terms)),
	mOrdered(ordered)
{
	if( duplicates ) {
#ifdef USE_MONOMIAL_POOL
		std::size_t id = mTermAdditionManager.getId(mTerms.size());
		for (const auto& t: mTerms) mTermAdditionManager.addTerm(id, t);
		mTermAdditionManager.readTerms(id, mTerms);
#else
		mTermAdditionManager.removeDuplicates(*this);
#endif
		mOrdered = false;
	}
	
	if (!mOrdered) {
		makeMinimallyOrdered();
	}

	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(const MultivariatePolynomial<Coeff, Ordering, Policies>::TermsType& terms, bool duplicates, bool ordered):
	mTerms(terms),
	mOrdered(ordered)
{
	if( duplicates ) {
#ifdef USE_MONOMIAL_POOL
		std::size_t id = mTermAdditionManager.getId(mTerms.size());
		for (const auto& t: mTerms) mTermAdditionManager.addTerm(id, t);
		mTermAdditionManager.readTerms(id, mTerms);
#else
		mTermAdditionManager.removeDuplicates(*this);
#endif
	} 
	if (!ordered) {
		makeMinimallyOrdered();
	}
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(const std::initializer_list<Term<Coeff>>& terms)
{
	for (const auto& t: terms) {
		mTerms.push_back(std::make_shared<const Term<Coeff>>(t));
	}
	makeMinimallyOrdered();
	mOrdered = false;
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(const std::initializer_list<Variable>& terms)
{
	for (const Variable& t: terms) {
		mTerms.push_back(std::make_shared<const Term<Coeff>>(t));
	}
	makeMinimallyOrdered();
	mOrdered = false;
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(const std::pair<ConstructorOperation, std::vector<MultivariatePolynomial>>& p)
{
	auto op = p.first;
	auto sub = p.second;
	assert(!sub.empty());
	auto it = sub.begin();
	*this = *it;
	if ((op == ConstructorOperation::SUB) && (sub.size() == 1)) {
		// special treatment of unary minus
		*this *= -1;
        assert(this->isConsistent());
		return;
	}
	if (op == ConstructorOperation::DIV) {
		// division shall have at least two arguments
		assert(sub.size() >= 2);
	}
	for (it++; it != sub.end(); it++) {
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
const Monomial::Arg& MultivariatePolynomial<Coeff,Ordering,Policies>::lmon() const
{
	return lterm()->monomial();
}
template<typename Coeff, typename Ordering, typename Policies>
const std::shared_ptr<const Term<Coeff>>& MultivariatePolynomial<Coeff,Ordering,Policies>::lterm() const
{
	LOG_ASSERT("carl.core", !isZero(), "Leading term undefined on zero polynomials.");
	return mTerms.back();
}

template<typename Coeff, typename Ordering, typename Policies>
const std::shared_ptr<const Term<Coeff>>& MultivariatePolynomial<Coeff,Ordering,Policies>::trailingTerm() const
{
	LOG_ASSERT("carl.core", !isZero(), "Trailing term undefined on zero polynomials.");
	return mTerms.front();
}

template<typename Coeff, typename Ordering, typename Policies>
const Coeff& MultivariatePolynomial<Coeff,Ordering,Policies>::lcoeff() const
{
	return lterm()->coeff();
}

template<typename Coeff, typename Ordering, typename Policies>
exponent MultivariatePolynomial<Coeff,Ordering,Policies>::totalDegree() const
{
	assert(mTerms.size() != 0);
	if (Ordering::degreeOrder) {
		return this->lterm()->tdeg();
	} else {
		LOG_NOTIMPLEMENTED();
	}
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isZero() const
{
	return mTerms.empty();
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isOne() const
{
	return (mTerms.size() == 1) && mTerms.front()->isOne();
}


template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isConstant() const
{
	return (mTerms.size() == 0) || (mTerms.size() == 1 && mTerms.front()->isConstant());
}

template<typename Coeff, typename Ordering, typename Policies>
const Coeff& MultivariatePolynomial<Coeff, Ordering, Policies>::constantPart() const
{
	if(isZero()) return constant_zero<Coeff>::get();
	if(trailingTerm()->isConstant()) {
		return trailingTerm()->coeff();
	}
	return constant_zero<Coeff>::get();
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isLinear() const
{
	if(mTerms.size() == 0) return true;
	if(Ordering::degreeOrder)
	{
		return this->lterm()->isLinear();
	}
	else 
	{
		LOG_NOTIMPLEMENTED();
	}
}

template<typename Coeff, typename Ordering, typename Policies>
Definiteness MultivariatePolynomial<Coeff,Ordering,Policies>::definiteness() const
{
	auto term = mTerms.rbegin();
	if( term == mTerms.rend() ) return Definiteness::NON;
	Definiteness result = (*term)->definiteness();
	++term;
	if( term == mTerms.rend() ) return result;
	if( result > Definiteness::NON )
	{
		for( ; term != mTerms.rend(); ++term )
		{
			Definiteness termDefin = (*term)->definiteness();
			if( termDefin > Definiteness::NON )
			{
				if( termDefin > result ) result = termDefin;
			}
			else return Definiteness::NON;
		}
	}
	else if( result < Definiteness::NON )
	{
		for( ; term != mTerms.rend(); ++term )
		{
			Definiteness termDefin = (*term)->definiteness();
			if( termDefin < Definiteness::NON )
			{
				if( termDefin > result ) result = termDefin;
			}
			else return Definiteness::NON;
		}
	}
	return result;
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::hasConstantTerm() const
{
	if(nrTerms() == 0) return false;
	else {
		// A term may not be zero...
		return trailingTerm()->isConstant();
	}
}

template<typename Coeff, typename Ordering, typename Policies>
const std::shared_ptr<const Term<Coeff>>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator[](unsigned index) const
{
	assert(index < nrTerms());
	return mTerms.at(index);
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::tail(bool makeFullOrdered) const
{
	assert(mTerms.size() != 0);
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
		if(makeFullOrdered) 
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
	assert(!isZero());
	mTerms.pop_back();
	if (!isOrdered()) makeMinimallyOrdered<false, true>();
	assert(this->isConsistent());
	return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isUnivariate() const {
	// A constant polynomial is obviously univariate.
	if (isConstant()) return true;
	
	if (this->lterm()->getNrVariables() > 1) {
		return false;
	}
	
	Variable v = lterm()->getSingleVariable();
	for (auto term : mTerms) {
		if (!term->hasNoOtherVariable(v)) return false;
	}
	return true;
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isTsos() const
{
	// A polynomial is a tsos if it is the sum of squares in its standard representation.
	for(const std::shared_ptr<const TermType>& term : mTerms)
	{
		if(!term->isSquare()) return false;
	}
	return true;
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::has(Variable::Arg v) const
{
	for (auto& term : mTerms)
		if (term->has(v)) return true;
	return false;
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isReducibleIdentity() const
{
	LOG_NOTIMPLEMENTED();
	return false;
}

template<typename Coeff, typename Ordering, typename Policies>
void MultivariatePolynomial<Coeff,Ordering,Policies>::subtractProduct(const Term<Coeff>& factor, const MultivariatePolynomial<Coeff,Ordering,Policies> & p) {
	assert(this->isConsistent());
	assert(p.isConsistent());
	if(mTerms.size() == 0)
	{
		*this = factor*p;
        assert(this->isConsistent());
	}
	if(p.mTerms.size() == 0) return;
	if(factor.coeff() == constant_zero<Coeff>::get()) return;

#ifdef USE_MONOMIAL_POOL
	std::size_t id = mTermAdditionManager.getId(mTerms.size() + p.mTerms.size());
	for (const auto& term: mTerms) {
		mTermAdditionManager.addTerm(id, term);
	}
	for (const auto& term: p.mTerms) {
		auto c = - factor.coeff() * term->coeff();
		auto m = factor.monomial() * term->monomial();
		mTermAdditionManager.addTerm(id, std::make_shared<const TermType>(c, m));
	}
	mTermAdditionManager.readTerms(id, mTerms);
#else
	std::size_t id = mTermAdditionManager.getTermMapId(*this, mTerms.size() + p.mTerms.size());
	for (const auto& term: mTerms) {
		mTermAdditionManager.addTerm(*this, id, term);
	}
	for (const auto& term: p.mTerms) {
		auto c = - factor.coeff() * term->coeff();
		auto m = factor.monomial() * term->monomial();
		mTermAdditionManager.addTerm(*this, id, std::make_shared<const TermType>(c, m));
	}
	mTermAdditionManager.readTerms(*this, id, mTerms);
#endif
	mOrdered = false;
	makeMinimallyOrdered<false, true>();
	assert(this->isConsistent());
}


template<typename Coeff, typename Ordering, typename Policies>
template<typename C, EnableIf<is_field<C>>>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::divideBy(const Coeff& divisor) const
{
	MultivariatePolynomial<Coeff,Ordering,Policies> res;
	res.mTerms.reserve(mTerms.size());
	for (unsigned i = 0; i < mTerms.size(); i++) {
		res.mTerms.emplace_back(mTerms[i]->divideBy(divisor));
	}
    assert(res.isConsistent());
	return res;
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename C, EnableIf<is_field<C>>>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::divideBy(const MultivariatePolynomial<Coeff,Ordering,Policies>& divisor, MultivariatePolynomial<Coeff,Ordering,Policies>& quotient) const
{
	static_assert(is_field<C>::value, "Division only defined for field coefficients");
	MultivariatePolynomial res(*this);
#ifdef USE_MONOMIAL_POOL
	std::size_t id = mTermAdditionManager.getId(res.mTerms.size());
	while(!res.isZero())
	{
		Term<C>* factor = res.lterm()->divideBy(*divisor.lterm());
		// nullptr if lt(divisor) does not divide lt(p).
		if(factor != nullptr)
		{
			res.subtractProduct(*factor, divisor);
			//p -= *factor * divisor;
			mTermAdditionManager.addTerm(id, std::shared_ptr<TermType>(factor));
		}
		else
		{
			return false;
		}
	}
	mTermAdditionManager.readTerms(id, quotient.mTerms);
#else
	std::size_t id = mTermAdditionManager.getTermMapId(*this, mTerms.size() * divisor.nrTerms());
	while(!res.isZero())
	{
		Term<C>* factor = res.lterm()->divideBy(*divisor.lterm());
		// nullptr if lt(divisor) does not divide lt(p).
		if(factor != nullptr)
		{
			mTermAdditionManager.addTerm(*this, id, std::shared_ptr<TermType>(factor));
			res -= *factor * divisor;
		}
		else
		{
			return false;
		}
	}
	mTermAdditionManager.readTerms(*this, id, quotient.mTerms);
#endif
	quotient.mOrdered = false;
	quotient.makeMinimallyOrdered<false, true>();
	assert(quotient.isConsistent());
	return true;
}

template<typename C, typename O, typename P>
DivisionResult<MultivariatePolynomial<C,O,P>> MultivariatePolynomial<C,O,P>::divideBy(const MultivariatePolynomial& divisor) const
{
	static_assert(is_field<C>::value, "Division only defined for field coefficients");
	DivisionResult<MultivariatePolynomial<C,O,P>> result;
	MultivariatePolynomial p = *this;
	while(!p.isZero())
	{
		Term<C>* factor = p.lterm()->divideBy(*divisor.lterm());
		// nullptr if lt(divisor) does not divide lt(p).
		if(factor != nullptr)
		{
			result.quotient += *factor;
			p.subtractProduct(*factor, divisor);
			//p -= *factor * divisor;
			delete factor;
		}
		else
		{
			result.remainder += p.lterm();
			p.stripLT();
		}
	}
	assert(result.quotient.isConsistent());
	assert(result.remainder.isConsistent());
	assert(*this == result.quotient * divisor + result.remainder);
	return result;
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> MultivariatePolynomial<C,O,P>::quotient(const MultivariatePolynomial& divisor) const
{
	assert(!divisor.isZero());
	if(*this == divisor)
	{
		return MultivariatePolynomial<C,O,P>(1);
	}
	if(divisor.isOne())
	{
		return *this;
	}
	//static_assert(is_field<C>::value, "Division only defined for field coefficients");
	MultivariatePolynomial p(*this);
#ifdef USE_MONOMIAL_POOL
	std::size_t id = mTermAdditionManager.getId(p.mTerms.size());
	while(!p.isZero())
	{
		Term<C>* factor = p.lterm()->divideBy(*divisor.lterm());
		// nullptr if lt(divisor) does not divide lt(p).
		if(factor != nullptr)
		{
			//p -= *factor * divisor;
			p.subtractProduct(*factor, divisor);
			mTermAdditionManager.addTerm(id, std::shared_ptr<TermType>(factor));
		}
		else
		{
			p.stripLT();
		}
	}
	MultivariatePolynomial<C,O,P> result;
	mTermAdditionManager.readTerms(id, result.mTerms);
#else
	std::size_t id = mTermAdditionManager.getTermMapId(*this, mTerms.size() * divisor.nrTerms());
	while(!p.isZero())
	{
		Term<C>* factor = p.lterm()->divideBy(*divisor.lterm());
		// nullptr if lt(divisor) does not divide lt(p).
		if(factor != nullptr)
		{
			p -= *factor * divisor;
			mTermAdditionManager.addTerm(*this, id, std::shared_ptr<TermType>(factor));
		}
		else
		{
			p.stripLT();
		}
	}
	MultivariatePolynomial<C,O,P> result;
	mTermAdditionManager.readTerms(*this, id, result.mTerms);
#endif
	result.mOrdered = false;
	result.makeMinimallyOrdered<false, true>();
	assert(result.isConsistent());
	assert(this->isConsistent());
	return result;
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> MultivariatePolynomial<C,O,P>::remainder(const MultivariatePolynomial& divisor) const
{
	static_assert(is_field<C>::value, "Division only defined for field coefficients");
	assert(!divisor.isZero());
	if(this == &divisor || divisor.isOne() || *this == divisor)
	{
		return MultivariatePolynomial<C,O,P>();
	}
	
	MultivariatePolynomial<C,O,P> remainder;
	MultivariatePolynomial p = *this;
	while(!p.isZero())
	{
		if(p.lterm()->tdeg() < divisor.lterm()->tdeg())
		{
			assert(p.lterm()->divideBy(*divisor.lterm()) == nullptr);
			if( O::degreeOrder )
			{
				remainder += p;
                assert(remainder.isConsistent());
				return remainder;
			}
			remainder += p.lterm();
			p.stripLT();
		}
		else
		{
			Term<C>* factor = p.lterm()->divideBy(*divisor.lterm());
			// nullptr if lt(divisor) does not divide lt(p).
			if(factor != nullptr)
			{
				p.subtractProduct(*factor, divisor);
				//p -= *factor * divisor;
				delete factor;
			}
			else
			{
				remainder += p.lterm();
				p.stripLT();
			}
		}
	}
	assert(remainder.isConsistent());
	assert(*this == quotient(divisor) * divisor + remainder);
	return remainder;
}

template<typename Coeff, typename Ordering, typename Policies>
void MultivariatePolynomial<Coeff,Ordering,Policies>::substituteIn(Variable::Arg var, const MultivariatePolynomial<Coeff, Ordering, Policies>& value)
{
	assert(this->isConsistent());
	assert(value.isConsistent());
	if (!this->has(var)) {
		return;
	}
	std::stringstream ss;
	ss << *this;
	TermsType newTerms;
	// If we replace a variable by zero, just eliminate all terms containing the variable.
	if(value.isZero())
	{
		for (const auto& term: mTerms) {
			if (!term->has(var)) {
				newTerms.push_back(term);
			}
		}
		mTerms.swap(newTerms);
		LOGMSG_TRACE("carl.core", ss.str() << " [ " << var << " -> " << value << " ] = " << *this);
        assert(this->isConsistent());
		return;
	}
	// Find all exponents occurring with the variable to substitute as basis.
	// expResults will finally be a mapping from every exponent e for which var^e occurs to the value^e and the number of times var^e occurs.
	// Meanwhile, we store an upper bound on the expected number of terms of the result in expectedResultSize.
	std::map<exponent, std::pair<MultivariatePolynomial, size_t>> expResults;
	size_t expectedResultSize = 0;
	std::pair<MultivariatePolynomial, unsigned> def( MultivariatePolynomial((Coeff) 1), 1 );
	for(const auto& term: mTerms)
	{
		if(term->monomial())
		{ // This is not the constant part.
			exponent e = term->monomial()->exponentOfVariable(var);
			if(e > 1)
			{ // Variable occurs with exponent at least two. Insert into map and increase counter in map.
				auto iterBoolPair = expResults.insert(std::pair<exponent, std::pair<MultivariatePolynomial, size_t>>(e, def));
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
		expResultB->second.first = value.pow(expResultB->first);
		expectedResultSize += expResultB->second.second * expResultB->second.first.nrTerms();
		++expResultB;
		while(expResultB != expResults.end())
		{
			// Calculate next var^e based on the last one.
			expResultB->second.first = expResultA->second.first * value.pow(expResultB->first - expResultA->first);
			expectedResultSize += expResultB->second.second * expResultB->second.first.nrTerms();
			++expResultA;
			++expResultB;
		}
	}
	// Substitute the variable.
#ifdef USE_MONOMIAL_POOL
	std::size_t id = mTermAdditionManager.getId(expectedResultSize);
	for (const auto& term: mTerms)
	{
		if (term->monomial() == nullptr) {
			mTermAdditionManager.addTerm(id, term);
		} else {
			exponent e = term->monomial()->exponentOfVariable(var);
			Monomial::Arg mon;
			if (e > 0) mon = term->monomial()->dropVariable(var);
			if (e == 1) {
				for(auto vterm : value.mTerms)
				{
					if (mon == nullptr) mTermAdditionManager.addTerm(id, std::make_shared<Term<Coeff>>(vterm->coeff() * term->coeff(), vterm->monomial()));
					else if (vterm->monomial() == nullptr) mTermAdditionManager.addTerm(id, std::make_shared<Term<Coeff>>(vterm->coeff() * term->coeff(), mon));
					else mTermAdditionManager.addTerm(id, std::make_shared<Term<Coeff>>(vterm->coeff() * term->coeff(), vterm->monomial() * mon));
				}
			} else if(e > 1) {
				auto iter = expResults.find(e);
				assert(iter != expResults.end());
				for(auto vterm : iter->second.first.mTerms)
				{
					if (mon == nullptr) mTermAdditionManager.addTerm(id, std::make_shared<Term<Coeff>>(vterm->coeff() * term->coeff(), vterm->monomial()));
					else if (vterm->monomial() == nullptr) mTermAdditionManager.addTerm(id, std::make_shared<Term<Coeff>>(vterm->coeff() * term->coeff(), mon));
					else mTermAdditionManager.addTerm(id, std::make_shared<Term<Coeff>>(vterm->coeff() * term->coeff(), vterm->monomial() * mon));
				}
			}
			else
			{
				mTermAdditionManager.addTerm(id, term);
			}
		}
	}
	mTermAdditionManager.readTerms(id, mTerms);
#else
	std::size_t id = mTermAdditionManager.getTermMapId(*this, expectedResultSize);
	for (const auto& term: mTerms)
	{
		if (term->monomial() == nullptr) {
			mTermAdditionManager.addTerm(*this, id, term);
		} else {
			exponent e = term->monomial()->exponentOfVariable(var);
			Monomial::Arg mon;
			if (e > 0) mon = term->monomial()->dropVariable(var);
			if (e == 1) {
				for(auto vterm : value.mTerms)
				{
					if (mon == nullptr) mTermAdditionManager.addTerm(*this, id, std::make_shared<Term<Coeff>>(vterm->coeff() * term->coeff(), vterm->monomial()));
					else if (vterm->monomial() == nullptr) mTermAdditionManager.addTerm(*this, id, std::make_shared<Term<Coeff>>(vterm->coeff() * term->coeff(), mon));
					else mTermAdditionManager.addTerm(*this, id, std::make_shared<Term<Coeff>>(vterm->coeff() * term->coeff(), vterm->monomial() * mon));
				}
			} else if(e > 1) {
				auto iter = expResults.find(e);
				assert(iter != expResults.end());
				for(auto vterm : iter->second.first.mTerms)
				{
					if (mon == nullptr) mTermAdditionManager.addTerm(*this, id, std::make_shared<Term<Coeff>>(vterm->coeff() * term->coeff(), vterm->monomial()));
					else if (vterm->monomial() == nullptr) mTermAdditionManager.addTerm(*this, id, std::make_shared<Term<Coeff>>(vterm->coeff() * term->coeff(), mon));
					else mTermAdditionManager.addTerm(*this, id, std::make_shared<Term<Coeff>>(vterm->coeff() * term->coeff(), vterm->monomial() * mon));
				}
			}
			else
			{
				mTermAdditionManager.addTerm(*this, id, term);
			}
		}
	}
	mTermAdditionManager.readTerms(*this, id, mTerms);
#endif
    mOrdered = false;
    makeMinimallyOrdered<false, true>();
	assert(mTerms.size() <= expectedResultSize);
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::substitute(Variable::Arg var, const MultivariatePolynomial<Coeff, Ordering, Policies>& value) const
{
	MultivariatePolynomial result(*this);
	result.substituteIn(var, value);
	return result;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::substitute(const std::map<Variable, MultivariatePolynomial<Coeff, Ordering, Policies>>& substitutions) const
{
	MultivariatePolynomial result(*this);
	if(isConstant() || substitutions.empty())
	{
		return result;
	}
	// Substitute the variables, which have to be replaced by 0, beforehand, 
	// as this could significantly simplify this multivariate polynomial.
	for(auto sub = substitutions.begin(); sub != substitutions.end(); ++sub)
	{
		if(sub->second.isZero())
		{
			result.substituteIn(sub->first, sub->second);
			if(result.isConstant())
			{
                assert(result.isConsistent());
				return result;
			}
		}
	}
	// Find and sort all exponents occurring for all variables to substitute as basis.
	std::map<std::pair<Variable, exponent>, MultivariatePolynomial> expResults;
	for(const auto& term: result.mTerms)
	{
		if(term->monomial())
		{
			const Monomial& m = *(term->monomial());
			LOGMSG_TRACE("carl.core.monomial", "Iterating over " << m);
			for(unsigned i = 0; i < m.nrVariables(); ++i)
			{
				LOGMSG_TRACE("carl.core.monomial", "Iterating: " << m[i].first);
				if(m[i].second > 1 && substitutions.find(m[i].first) != substitutions.end())
				{
					expResults[m[i]] = MultivariatePolynomial((Coeff) 1);
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
		while(sub->first != expResultB->first.first) 
		{
			assert(sub != substitutions.end());
			++sub;
		}
		expResultB->second = sub->second.pow(expResultB->first.second);
		++expResultB;
		while(expResultB != expResults.end())
		{
			if(expResultA->first.first != expResultB->first.first)
			{
				++sub;
				assert(sub != substitutions.end());
				// Go to the next variable.
				while(sub->second.isZero()) 
				{
					assert(sub != substitutions.end());
					++sub;
				}
				assert(sub->first == expResultB->first.first);
				expResultB->second = sub->second.pow(expResultB->first.second);
			}
			else
			{
				expResultB->second = expResultA->second * sub->second.pow(expResultB->first.second-expResultA->first.second);
			}
			++expResultA;
			++expResultB;
		}
	}
	MultivariatePolynomial resultB((Coeff)0);
	// Substitute the variable for which all occurring exponentiations are calculated.
	for(const auto& term: result.mTerms)
	{
		MultivariatePolynomial termResult(term->coeff());
		if(term->monomial())
		{   
			const Monomial& m = *(term->monomial());
			LOGMSG_TRACE("carl.core.monomial", "Iterating over " << m);
			for(unsigned i = 0; i < m.nrVariables(); ++i)
			{
				LOGMSG_TRACE("carl.core.monomial", "Iterating: " << m[i].first);
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
						termResult *= Term<Coeff>((Coeff)1, m[i].first, m[i].second);
					}
				}
			}
			
		}
		resultB += termResult;
	}
	assert(resultB.isConsistent());
	return resultB;
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename SubstitutionType>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::substitute(const std::map<Variable,SubstitutionType>& substitutions) const
{
    static_assert(!std::is_same<SubstitutionType, Term<Coeff>>::value, "Terms are handled by a seperate method.");
	MultivariatePolynomial result;
#ifdef USE_MONOMIAL_POOL
	std::size_t id = mTermAdditionManager.getId(mTerms.size());
	for (const auto& term: mTerms) {
        Term<Coeff >* resultTerm = term->substitute(substitutions);
        if( !resultTerm->isZero() )
        {
            mTermAdditionManager.addTerm(id, std::shared_ptr<const Term<Coeff >>( resultTerm ));
        }
        else
        {
            delete resultTerm;
        }
	}
	mTermAdditionManager.readTerms(id, result.mTerms);
#else
	std::size_t id = mTermAdditionManager.getTermMapId(*this, mTerms.size());
	for (const auto& term: mTerms) {
        Term<Coeff >* resultTerm = term->substitute(substitutions);
        if( !resultTerm->isZero() )
        {
            mTermAdditionManager.addTerm(*this, id, std::shared_ptr<const Term<Coeff >>( resultTerm ));
        }
        else
        {
            delete resultTerm;
        }
	}
	mTermAdditionManager.readTerms(*this, id, result.mTerms);
#endif
    result.makeMinimallyOrdered<false, true>();
	assert(result.isConsistent());
	return result;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies> MultivariatePolynomial<Coeff, Ordering, Policies>::substitute(const std::map<Variable, Term<Coeff>>& substitutions) const
{
	MultivariatePolynomial result;
#ifdef USE_MONOMIAL_POOL
	std::size_t id = mTermAdditionManager.getId(mTerms.size());
	for (const auto& term: mTerms) {
		mTermAdditionManager.addTerm(id, std::shared_ptr<const Term<Coeff >>( term->substitute(substitutions) ));
	}
	mTermAdditionManager.readTerms(id, result.mTerms);
#else
	std::size_t id = mTermAdditionManager.getTermMapId(*this, mTerms.size());
	for (const auto& term: mTerms) {
		mTermAdditionManager.addTerm(*this, id, std::shared_ptr<const Term<Coeff >>( term->substitute(substitutions) ));
	}
	mTermAdditionManager.readTerms(*this, id, result.mTerms);
#endif
	result.makeMinimallyOrdered<false, true>();
	assert(result.isConsistent());
	return result;
}


template<typename Coeff, typename Ordering, typename Policies>
template<typename SubstitutionType>
Coeff MultivariatePolynomial<Coeff,Ordering,Policies>::evaluate(const std::map<Variable,SubstitutionType>& substitutions) const
{
	// We do not have to construct polynomials all the time.
	LOG_INEFFICIENT();
	MultivariatePolynomial result = substitute(substitutions);
	assert(result.isConstant());
	return result.constantPart();
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename C, EnableIf<is_subset_of_rationals<C>>>
Coeff MultivariatePolynomial<Coeff,Ordering,Policies>::coprimeFactor() const
{
	assert(nrTerms() != 0);
	typename TermsType::const_iterator it = mTerms.begin();
	typename IntegralType<Coeff>::type num = getNum((*it)->coeff());
	typename IntegralType<Coeff>::type den = getDenom((*it)->coeff());
	for(++it; it != mTerms.end(); ++it)
	{
		num = carl::gcd(num, getNum((*it)->coeff()));
		den = carl::lcm(den, getDenom((*it)->coeff()));
	}
	return Coeff(den)/num;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::coprimeCoefficients() const
{
	if(nrTerms() == 0) return *this;
	Coeff factor = coprimeFactor();
	// Notice that even if factor is 1, we create a new polynomial
	MultivariatePolynomial<Coeff, Ordering, Policies> result;
	result.mTerms.reserve(mTerms.size());
	for (const auto& term: mTerms)
	{
		result.mTerms.push_back(std::make_shared<const TermType>(*term * factor));
	}
    result.mOrdered = mOrdered;
    assert(result.isConsistent());
	return result;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::normalize() const
{
	MultivariatePolynomial result;
	if(Policies::has_reasons)
	{
		result.setReasons(this->getReasons());
	}
	result.mTerms.reserve(mTerms.size());
	for (const auto& it: mTerms)
	{
		result.mTerms.emplace_back(it->divideBy(lcoeff()));
	}
	result.mOrdered = mOrdered;
	assert(result.isConsistent());
	return result;
	
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::derivative(Variable::Arg v, unsigned nth) const
{
	///@todo this method is broken:
	/// - support nth > 1
	/// - allow 0
	/// - fix ordering issues with result
	assert(!isZero());
	assert(this->isConsistent());
	// TODO n > 1 not yet implemented!
	assert(nth == 1);
	TermsType tmpTerms;
	for(const auto& t : mTerms) {
		tmpTerms.emplace_back(t->derivative(v));
		if (tmpTerms.back()->isZero()) tmpTerms.pop_back();
	}
	MultivariatePolynomial result(std::move(tmpTerms), true, mOrdered);
    assert(result.isConsistent());
	return result;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::SPolynomial(
																const MultivariatePolynomial<Coeff, Ordering, Policies>& p,
																const MultivariatePolynomial<Coeff, Ordering, Policies>& q)
{
	assert(p.nrTerms() != 0);
	assert(q.nrTerms() != 0);
	assert(p.isConsistent());
	assert(q.isConsistent());

	
	if( p.nrTerms() == 1 && q.nrTerms() == 1 )
	{
		return MultivariatePolynomial();
	}
	else if( p.nrTerms() == 1 )
	{
		return -(p.lterm()->calcLcmAndDivideBy( q.lmon() ) * q.tail());
	}
	else if( q.nrTerms() == 1 )
	{
		return (q.lterm()->calcLcmAndDivideBy( p.lmon() ) * p.tail());
	}
	else
	{
		return (p.tail() * q.lterm()->calcLcmAndDivideBy(p.lmon())) - (q.tail() * p.lterm()->calcLcmAndDivideBy( q.lmon() ));
	}
	
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::pow(unsigned exp) const
{
	if (exp == 0) return MultivariatePolynomial(constant_one<Coeff>::get());
	if (isZero()) return MultivariatePolynomial(constant_zero<Coeff>::get());
	MultivariatePolynomial<Coeff,Ordering,Policies> res(constant_one<Coeff>::get());
	MultivariatePolynomial<Coeff,Ordering,Policies> mult(*this);
	while(exp > 0) {
		if (exp & 1) res *= mult;
		exp /= 2;
		if(exp > 0)
			mult *= mult;
	}
	return res;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::naive_pow(unsigned exp) const
{
	if(exp == 0)
	{
		return MultivariatePolynomial((Coeff)1);
	}
	
	LOG_INEFFICIENT();
	MultivariatePolynomial<Coeff,Ordering,Policies> res(*this);
	for(unsigned i = 1; i < exp; i++)
	{
		res *= *this;
	}
	return res;	
}

template<typename Coeff, typename Ordering, typename Policies>
void MultivariatePolynomial<Coeff,Ordering,Policies>::gatherVariables(std::set<Variable>& vars) const
{
	for(const auto& t : mTerms)
	{
		t->gatherVariables(vars);
	}
}

template<typename Coeff, typename Ordering, typename Policies>
std::set<Variable> MultivariatePolynomial<Coeff,Ordering,Policies>::gatherVariables() const
{
	std::set<Variable> vars;
	gatherVariables(vars);
	return vars;
}

template<typename Coeff, typename Ordering, typename Policies>
template<bool gatherCoeff>
VariableInformation<gatherCoeff, MultivariatePolynomial<Coeff,Ordering,Policies>> MultivariatePolynomial<Coeff,Ordering,Policies>::getVarInfo(Variable::Arg var) const
{
	VariableInformation<gatherCoeff, MultivariatePolynomial> varinfomap;
	// We iterate over all terms.
	for(const auto& term : mTerms)
	{
		// And gather information from the terms and meanwhile up
		term->gatherVarInfo(var, varinfomap);
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
		term->gatherVarInfo(varinfomap);
	}
	return varinfomap;
	
}

template<typename C, typename O, typename P>
UnivariatePolynomial<C> MultivariatePolynomial<C,O,P>::toUnivariatePolynomial() const
{
	// Only correct when it is already only in one variable.
	assert(gatherVariables().size() == 1);
	Variable::Arg x = lmon()->getSingleVariable();
	std::vector<C> coeffs(totalDegree()+1,0);
	for (const auto& t : mTerms)
	{
		coeffs[t->tdeg()] = t->coeff();
	}
	return UnivariatePolynomial<C>(x, coeffs);
}

template<typename C, typename O, typename P>
UnivariatePolynomial<MultivariatePolynomial<C,O,P>> MultivariatePolynomial<C,O,P>::toUnivariatePolynomial(Variable::Arg v) const
{
	assert(this->isConsistent());
	std::vector<MultivariatePolynomial<C,O,P>> coeffs(1);
	for (const auto& term: this->mTerms) {
		if (term->monomial() == nullptr) coeffs[0] += *term;
		else {
			auto mon = term->monomial();
			auto exponent = mon->exponentOfVariable(v);
			if (exponent >= coeffs.size()) {
				coeffs.resize(exponent + 1, MultivariatePolynomial<C,O,P>(0));
			}
			std::shared_ptr<const carl::Monomial> tmp = mon->dropVariable(v);
			coeffs[exponent] += term->coeff() * tmp;
		}
	}
	// Convert result back to MultivariatePolynomial and check that the result is equal to *this
	assert(MultivariatePolynomial<C>(UnivariatePolynomial<MultivariatePolynomial<C,O,P>>(v, coeffs)) == *this);
	return UnivariatePolynomial<MultivariatePolynomial<C,O,P>>(v, coeffs);
}

template<typename Coeff, typename O, typename P>
template<typename C, EnableIf<is_number<C>>>
Coeff MultivariatePolynomial<Coeff,O,P>::numericContent() const
{
	if (this->isZero()) return 0;
	typename UnderlyingNumberType<C>::type res = this->mTerms.front()->coeff();
	for (unsigned i = 0; i < this->mTerms.size(); i++) {
		///@todo gcd needed for fractions
		res = carl::gcd(res, this->mTerms[i]->coeff());
		//assert(false);
	}
	return res;
}

template<typename Coeff, typename O, typename P>
template<typename C, DisableIf<is_number<C>>>
typename UnderlyingNumberType<C>::type MultivariatePolynomial<Coeff,O,P>::numericContent() const
{
	if (this->isZero()) return 0;
	typename UnderlyingNumberType<C>::type res = this->mTerms.front()->coeff().numericContent();
	for (const auto& t: mTerms) {
		res = gcd(res, t->coeff().numericContent());
	}
	return res;
}

template<typename Coeff, typename O, typename P>
template<typename C, EnableIf<is_number<C>>>
typename MultivariatePolynomial<Coeff,O,P>::IntNumberType MultivariatePolynomial<Coeff,O,P>::mainDenom() const {
	IntNumberType res = 1;
	for (const auto& t: mTerms) {
		res = carl::lcm(res, getDenom(t->coeff()));
	}
	return res;
}



template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	// Try to avoid sorting
	if (lhs.nrTerms() != rhs.nrTerms()) return false;
	if (lhs.nrTerms() == 0) return true;
	if (lhs.hasConstantTerm() ^ rhs.hasConstantTerm()) return false;
	if (lhs.hasConstantTerm() && (lhs.constantPart() != rhs.constantPart())) return false;
	if (*lhs.lterm() != *rhs.lterm()) return false;
	lhs.makeOrdered();
	rhs.makeOrdered();
	// Compare vector entries. We cannot use std::vector== as we not only want to compare the pointers.
	return std::equal(lhs.begin(), lhs.end(), rhs.begin(),
		[](const std::shared_ptr<const Term<C>>& lterm, const std::shared_ptr<const Term<C>>& rterm) 
		-> bool 
		{
			return lterm == rterm || *lterm == *rterm;
		});
}

template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
	if (lhs.isZero() && carl::isZero(rhs.coeff())) return true;
	if (lhs.nrTerms() > 1) return false;
	return *(lhs.lterm()) == rhs;
}

template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const std::shared_ptr<const carl::Monomial>& rhs) {
	if (lhs.nrTerms() != 1) return false;
	if (lhs.lmon() == nullptr) return false;
	return lhs.lmon() == rhs;
}

template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
	if (lhs.isZero()) return carl::isZero(rhs);
	if (lhs.nrTerms() > 1) return false;
	if (lhs.lmon() != nullptr) return false;
	return lhs.lcoeff() == rhs;
}

template<typename C, typename O, typename P, DisableIf<std::is_integral<C>>>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, int i) {
	if (lhs.isZero()) return i == 0;
	if (lhs.nrTerms() > 1) return false;
	if (lhs.lmon() != nullptr) return false;
	return lhs.lcoeff() == i;
}

template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs) {
	if (lhs.nrTerms() != 1) return false;
	if (lhs.lmon() == nullptr) return false;
	return lhs.lmon() == rhs;
}

template<typename C, typename O, typename P>
bool operator==(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	LOG_INEFFICIENT();
	return MultivariatePolynomial<C,O,P>(lhs) == rhs;
}

template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs) {
	return rhs == lhs;
}

template<typename C, typename O, typename P>
bool operator==(const UnivariatePolynomial<MultivariatePolynomial<C>>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	LOG_INEFFICIENT();
	return MultivariatePolynomial<C>(lhs) == rhs;
}

template<typename C, typename O, typename P>
bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (lhs.totalDegree() != rhs.totalDegree()) return lhs.totalDegree() < rhs.totalDegree();
	if (lhs.totalDegree() == 0) return lhs.constantPart() < rhs.constantPart();
	if (*lhs.lterm() < *rhs.lterm()) return true;
	if (*lhs.lterm() > *rhs.lterm()) return false;
	// We have to sort, but we can skip the leading term.
	lhs.makeOrdered();
	rhs.makeOrdered();
	auto lit = lhs.rbegin();
	auto rit = rhs.rbegin();
	for (lit++, rit++; lit != lhs.rend(); lit++, rit++) {
		if (rit == rhs.rend()) return false;
		if (**lit < **rit) return true;
		if (**lit > **rit) return false;
	}
	return rit != rhs.rend();
}
template<typename C, typename O, typename P>
bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
	if (lhs.nrTerms() == 0) return carl::constant_zero<C>().get() < rhs;
	return *(lhs.lterm()) < rhs;
}
template<typename C, typename O, typename P>
bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const std::shared_ptr<const carl::Monomial>& rhs) {
	if (lhs.nrTerms() == 0) return true;
	return *(lhs.lterm()) < rhs;
}
template<typename C, typename O, typename P>
bool operator<(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs) {
	if (lhs.nrTerms() == 0) return true;
	return *(lhs.lterm()) < rhs;
}
template<typename C, typename O, typename P>
bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
	if (lhs.nrTerms() == 0) return carl::constant_zero<C>().get() < rhs;
	return *(lhs.lterm()) < rhs;
}
template<typename C, typename O, typename P>
bool operator<(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (rhs.nrTerms() == 0) return lhs < carl::constant_zero<C>().get();
	if (lhs < *(rhs.lterm())) return true;
	if (lhs == *(rhs.lterm())) return rhs.nrTerms() > 1;
	return false;
}
template<typename C, typename O, typename P>
bool operator<(const std::shared_ptr<const carl::Monomial>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (rhs.nrTerms() == 0) return false;
	if (lhs < *(rhs.lterm())) return true;
	if (lhs == *(rhs.lterm())) return rhs.nrTerms() > 1;
	return false;
}
template<typename C, typename O, typename P>
bool operator<(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (rhs.nrTerms() == 0) return false;
	if (lhs < *(rhs.lterm())) return true;
	if (lhs == *(rhs.lterm())) return rhs.nrTerms() > 1;
	return false;
}
template<typename C, typename O, typename P>
bool operator<(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (rhs.nrTerms() == 0) return lhs < carl::constant_zero<C>().get();
	return lhs < *(rhs.lterm());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const MultivariatePolynomial& rhs)
{
	/** @todo Make this faster
	 * 
	 * If both polynomials are ordered: Use merging strategy we used before.
	 * 
	 * Otherwise:
	 *  Insert only terms of a single polynomial (the smaller one)
	 *  Check if the terms of the other ones are in the map
	 *	If yes: Remove from map, add to result
	 *	If no: add to result
	 */
	assert(this->isConsistent());
	assert(rhs.isConsistent());
	if(mTerms.size() == 0) 
	{
		*this = rhs;
        assert(this->isConsistent());
		return *this;
	}
	if(rhs.mTerms.size() == 0) return *this;
#undef OPTIMIZE
//#define OPTIMIZE
#ifdef OPTIMIZE
	std::shared_ptr<const TermType> newlterm;
	bool ltermFromRHS = false;
	if (Term<Coeff>::monomialLess(rhs.lterm(), lterm())) {
		newlterm = lterm();
		mTerms.pop_back();
	} else if (Term<Coeff>::monomialLess(lterm(), rhs.lterm())) {
		newlterm = rhs.lterm();
		rhs.mTerms.pop_back();
		ltermFromRHS = true;
	} else {
		//std::cout << "same lmon" << std::endl;
	}
#endif
#ifdef USE_MONOMIAL_POOL
	std::size_t id = mTermAdditionManager.getId(mTerms.size() + rhs.mTerms.size());
	for (const auto& term: mTerms) {
		mTermAdditionManager.addTerm(id, term);
	}
	for (const auto& term: rhs.mTerms) {
		mTermAdditionManager.addTerm(id, term);
	}
	mTermAdditionManager.readTerms(id, mTerms);
#else
	std::size_t id = mTermAdditionManager.getTermMapId(*this, mTerms.size() + rhs.mTerms.size());
	for (const auto& term: mTerms) {
		mTermAdditionManager.addTerm(*this, id, term);
	}
	for (const auto& term: rhs.mTerms) {
		mTermAdditionManager.addTerm(*this, id, term);
	}
	mTermAdditionManager.readTerms(*this, id, mTerms);
#endif
#ifdef OPTIMIZE
	if (newlterm != nullptr) {
		mTerms.push_back(newlterm);
		if (ltermFromRHS) rhs.mTerms.push_back(newlterm);
	} else {
		//std::cout << "ordering" << std::endl;
		makeMinimallyOrdered();
	}
#else
	makeMinimallyOrdered();
#endif
	mOrdered = false;
	assert(this->isConsistent());
	return *this;
}


template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const TermType& rhs) {
	return *this += std::make_shared<const TermType>(rhs);
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const std::shared_ptr<const TermType>& rhs)
{
	if(rhs->coeff() == constant_zero<Coeff>::get()) return *this;
	if (mTerms.size() == 0) {
		// Empty -> just insert.
		mTerms.push_back(rhs);
		mOrdered = true;
	} else if (rhs->isConstant()) {
		if (this->hasConstantTerm()) {
			// Combine with constant term.
			if (mTerms.front()->coeff() + rhs->coeff() == constant_zero<Coeff>::get()) {
				mTerms.erase(mTerms.begin());
			} else mTerms.front().reset(new Term<Coeff>(mTerms.front()->coeff() + rhs->coeff(), nullptr));
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
		if (lcoeff() + rhs->coeff() == constant_zero<Coeff>::get()) {
			mTerms.pop_back();
			makeMinimallyOrdered<false, true>();
		} else mTerms.back().reset(new Term<Coeff>(lcoeff() + rhs->coeff(), rhs->monomial()));
	} else if (Ordering::less(lterm(), rhs)) {
		// New leading term.
		mTerms.push_back(rhs);
	} else {
		// Full-blown addition.
#ifdef USE_MONOMIAL_POOL
		std::size_t id = mTermAdditionManager.getId(mTerms.size()+1);
		for (const auto& term: mTerms) {
			mTermAdditionManager.addTerm(id, term);
		}
		mTermAdditionManager.addTerm(id, rhs);
		mTermAdditionManager.readTerms(id, mTerms);
#else
		std::size_t id = mTermAdditionManager.getTermMapId(*this, mTerms.size() + 1);
		for (const auto& term: mTerms) {
			mTermAdditionManager.addTerm(*this, id, term);
		}
		mTermAdditionManager.addTerm(*this, id, rhs);
		mTermAdditionManager.readTerms(*this, id, mTerms);
#endif
		makeMinimallyOrdered<false, true>();
		mOrdered = false;
	}
	assert(this->isConsistent());
	return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const Monomial::Arg& rhs)
{
	if (mTerms.size() == 0) {
		// Empty -> just insert.
		mTerms.emplace_back(new TermType(constant_one<Coeff>::get(), rhs));
		mOrdered = true;
	} else if (lterm()->isConstant()) {
		mTerms.emplace_back(new TermType(constant_one<Coeff>::get(), rhs));
	} else if (lmon() == rhs) {
		// Combine with leading term.
		if (lcoeff() == -carl::constant_one<Coeff>().get()) {
			mTerms.pop_back();
			makeMinimallyOrdered<false, true>();
		} else {
			mTerms.back().reset(new Term<Coeff>(lcoeff() + constant_one<Coeff>::get(), lmon()));
		}
	} else if (Ordering::less(lmon(),rhs)) {
		// New leading term.
		mTerms.emplace_back(new TermType(rhs));
	} else {
		///@todo insert at correct position if already ordered
		auto it = mTerms.begin();
		for (; it != mTerms.end(); it++) {
			if ((*it)->monomial() == rhs) {
				if ((*it)->coeff() == -carl::constant_one<Coeff>().get()) {
					mTerms.erase(it);
				} else {
					it->reset(new Term<Coeff>((*it)->coeff() + constant_one<Coeff>::get(), (*it)->monomial()));
				}
				break;
			}
		}
		if (it == mTerms.end()) {
			mTerms.emplace_back(new Term<Coeff>(constant_one<Coeff>::get(), rhs));
			std::swap(mTerms[mTerms.size()-2], mTerms[mTerms.size()-1]);
		}
		mOrdered = false;
	}
	assert(this->isConsistent());
	return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(Variable::Arg rhs)
{
#ifdef USE_MONOMIAL_POOL
	return *this += MonomialPool::getInstance().create(rhs, 1);
#else
	return *this += std::make_shared<const Monomial>(rhs);
#endif
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const Coeff& c)
{
	if (c == constant_zero<Coeff>::get()) return *this;
	if (hasConstantTerm()) {
		if (constantPart() + c == constant_zero<Coeff>::get()) mTerms.erase(mTerms.begin());
		else mTerms.front().reset(new TermType(constantPart() + c));
	} else {
		mTerms.emplace(mTerms.begin(), new TermType(c));
	}
	assert(this->isConsistent());
	return *this;
}

template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator+(const UnivariatePolynomial<C>&, const MultivariatePolynomial<C,O,P>&)
{
	LOG_NOTIMPLEMENTED();
}

template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>&, const UnivariatePolynomial<C>&)
{
	LOG_NOTIMPLEMENTED();
}

template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator+(const UnivariatePolynomial<MultivariatePolynomial<C>>&, const MultivariatePolynomial<C,O,P>&)
{
	LOG_NOTIMPLEMENTED();
}

template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>&, const UnivariatePolynomial<MultivariatePolynomial<C>>&)
{
	LOG_NOTIMPLEMENTED();
}

template<typename Coeff, typename Ordering, typename Policies>
const MultivariatePolynomial<Coeff, Ordering, Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::operator -() const
{
	assert(this->isConsistent());
	MultivariatePolynomial<Coeff, Ordering, Policies> negation;
	negation.mTerms.reserve(mTerms.size());
	for (const auto& term: mTerms) {
		negation.mTerms.push_back(std::make_shared<const Term<Coeff>>(-*term));
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
	if(mTerms.size() == 0)
	{
		*this = -rhs;
        assert(this->isConsistent());
		return *this;
	}
	if(rhs.mTerms.size() == 0) return *this;
	
#ifdef USE_MONOMIAL_POOL
	std::size_t id = mTermAdditionManager.getId(mTerms.size() + rhs.mTerms.size());
	for (const auto& term: mTerms) {
		mTermAdditionManager.addTerm(id, term);
	}
	for (const auto& term: rhs.mTerms) {
		mTermAdditionManager.addTerm(id, std::shared_ptr<const TermType>(new TermType(-*term)));
	}
	mTermAdditionManager.readTerms(id, mTerms);
#else
	std::size_t id = mTermAdditionManager.getTermMapId(*this, mTerms.size() + rhs.mTerms.size());
	for (const auto& term: mTerms) {
		mTermAdditionManager.addTerm(*this, id, term);
	}
	for (const auto& term: rhs.mTerms) {
		mTermAdditionManager.addTerm(*this, id, std::shared_ptr<const TermType>(new TermType(-*term)));
	}
	mTermAdditionManager.readTerms(*this, id, mTerms);
#endif
	mOrdered = false;
	makeMinimallyOrdered<false, true>();
	assert(this->isConsistent());
	return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator-=(const Term<Coeff>& rhs)
{
	///@todo Check if this works with ordering.
	if(carl::isZero(rhs.coeff())) return *this;
	if(Policies::searchLinear) 
	{
		typename TermsType::iterator it(mTerms.begin());
		while(it != mTerms.end())
		{
			// TODO consider comparing the shared pointers.
			if ((**it).monomial() != nullptr) {
				CompareResult cmpres(Ordering::compare((**it), rhs));
				if( cmpres == CompareResult::GREATER ) break;
				if( cmpres == CompareResult::EQUAL )
				{
					// new coefficient would be zero, simply removing is enough.
					if((**it).coeff() == rhs.coeff())
					{
						mTerms.erase(it);
					}
					// we have to create a new term object.
					else
					{
						*it = std::make_shared<const Term<Coeff>>((**it).coeff()-rhs.coeff(), (**it).monomial());
					}
					assert(this->isConsistent());
					return *this;
				}
			}
			++it;	
		}
		// no equal monomial does occur. We can simply insert.
		mTerms.insert(it,std::make_shared<const Term<Coeff>>(-rhs));
		assert(this->isConsistent());
		return *this;
	}
	else 
	{
		LOG_NOTIMPLEMENTED();
	}
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator-=(const Monomial::Arg& rhs)
{
	///@todo Check if this works with ordering.
	if(!rhs) 
	{
		*this -= constant_one<Coeff>::get();
        assert(this->isConsistent());
		return *this;
	}
	if(Policies::searchLinear) 
	{
		typename TermsType::iterator it(mTerms.begin());
		while(it != mTerms.end())
		{
			if ((**it).monomial() != nullptr) {
				CompareResult cmpres(Ordering::compare((**it).monomial(), rhs));
				if( cmpres == CompareResult::GREATER ) break;
				if( cmpres == CompareResult::EQUAL )
				{
					// new coefficient would be zero, simply removing is enough.
					if(carl::isOne((**it).coeff()))
					{
						mTerms.erase(it);
					}
					// we have to create a new term object.
					else
					{
						*it = std::make_shared<const Term<Coeff>>((**it).coeff()-constant_one<Coeff>::get(), (**it).monomial());
					}
					assert(this->isConsistent());
					return *this;
				}
			}
			++it;	
		}
		// no equal monomial does occur. We can simply insert.
		mTerms.insert(it,std::make_shared<const Term<Coeff>>(-carl::constant_one<Coeff>().get(), rhs));
		assert(this->isConsistent());
		return *this;
	}
	else 
	{
		LOG_NOTIMPLEMENTED();
	}
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator-=(Variable::Arg rhs)
{
	return *this += Term<Coeff>(-carl::constant_one<Coeff>().get(), rhs, 1);
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator-=(const Coeff& c)
{
	return *this += (-c);
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator*=(const MultivariatePolynomial<Coeff,Ordering,Policies>& rhs)
{
	assert(this->isConsistent());
	assert(rhs.isConsistent());
	if(mTerms.size() == 0) return *this;
	if(rhs.mTerms.size() == 0) {
		mTerms.clear();
        assert(this->isConsistent());
		return *this;
	}
#ifdef USE_MONOMIAL_POOL
	std::size_t id = mTermAdditionManager.getId(mTerms.size() * rhs.mTerms.size());
	TermType* newlterm = nullptr;
	for (auto t1 = mTerms.rbegin(); t1 != mTerms.rend(); t1++) {
		for (auto t2 = rhs.mTerms.rbegin(); t2 != rhs.mTerms.rend(); t2++) {
			if (newlterm != nullptr) mTermAdditionManager.addTerm(id, std::make_shared<const TermType>(std::move((**t1)*(**t2))));
			else newlterm = new TermType(**t1 * **t2);
		}
	}
	mTermAdditionManager.readTerms(id, mTerms);
	mTerms.emplace_back(newlterm);
#else
	std::size_t id = mTermAdditionManager.getTermMapId(*this, mTerms.size() * rhs.mTerms.size());
	TermType* newlterm = nullptr;
	for (auto t1 = mTerms.rbegin(); t1 != mTerms.rend(); t1++) {
		for (auto t2 = rhs.mTerms.rbegin(); t2 != rhs.mTerms.rend(); t2++) {
			if (newlterm != nullptr) mTermAdditionManager.addTerm(*this, id, std::shared_ptr<const TermType>(new TermType((**t1)*(**t2))));
			else newlterm = new TermType(**t1 * **t2);
		}
	}
	mTermAdditionManager.readTerms(*this, id, mTerms);
	mTerms.emplace_back(newlterm);
#endif
	makeMinimallyOrdered<false, true>();
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
		newTerms.push_back(std::make_shared<Term<Coeff>>(*term * rhs));
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
		newTerms.push_back(std::make_shared<Term<Coeff>>(*term * rhs));
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
		newTerms.push_back(std::make_shared<Term<Coeff>>(*term * rhs));
	}
	mTerms = std::move(newTerms);
	assert(this->isConsistent());
	return *this;
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator*=(const Coeff& c)
{
	///@todo more efficient.
	if(carl::isOne(c)) return *this;
	if(carl::isZero(c)) 
	{
		mTerms.clear();
        assert(this->isConsistent());
		return *this;
	}
	TermsType newTerms;
	newTerms.reserve(mTerms.size());
	for(auto term : mTerms)
	{
		newTerms.push_back(std::make_shared<Term<Coeff>>(*term * c));
	}
	mTerms = std::move(newTerms);
	assert(this->isConsistent());
	return *this;
}

template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator*(const UnivariatePolynomial<C>&, const MultivariatePolynomial<C,O,P>&)
{
	LOG_NOTIMPLEMENTED();
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs * lhs;
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator/=(const Coeff& c)
{
	assert(c != constant_zero<Coeff>::get());
	if (carl::isOne(c)) return *this;
	for (auto& term : mTerms) {
		///@todo add /=(Coefficient) for Term.
		term.reset(term->divideBy(c));
	}
	return *this;
}

template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator/(const MultivariatePolynomial<C,O,P>& lhs, unsigned long rhs)
{
	MultivariatePolynomial<C,O,P> result;
	for (const auto& t: lhs.mTerms) {
		result.mTerms.emplace_back(new Term<C>(*t/rhs));
	}
	result.mOrdered = lhs.mOrdered;
    assert(result.isConsistent());
	return result;
}

template<typename C, typename O, typename P>
std::ostream& operator<<( std::ostream& os, const MultivariatePolynomial<C,O,P>& rhs )
{
	return (os << rhs.toString(true, true));
}

template<typename Coeff, typename Ordering, typename Policies>
std::string MultivariatePolynomial<Coeff, Ordering, Policies>::toString(bool infix, bool friendlyVarNames) const
{
	///@todo sort for this?
	if(mTerms.size() == 0) return "0";
	if(mTerms.size() == 1) return this->mTerms.front()->toString(infix, friendlyVarNames);
	std::string result = "";
	if( !infix ) result += "(+";
	for (auto term = mTerms.rbegin(); term != mTerms.rend(); term++)
	{
		if(infix)
		{
			if(term != mTerms.rbegin()) result += "+";
		}
		else result += " ";
		result += (*term)->toString(infix, friendlyVarNames);
	}
	if( !infix ) result += ")";
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
		if ((*it)->isConstant()) constTerm = it;

		for (it++; it != mTerms.end();) {
			if ((*it)->isConstant()) {
				assert(constTerm == mTerms.end());
				constTerm = it;
			} else if (Ordering::less(*lTerm, *it)) {
				lTerm = it;
			} else {
				assert(!Term<Coeff>::monomialEqual(**lTerm, **it));
			}
			it++;
		}
		makeMinimallyOrdered(lTerm, constTerm);
	}
	else
	{
		if (mTerms.size() < 3) return;
		auto it = mTerms.begin();
		const auto itToLast = mTerms.end() - 1;
		auto lTerm = it;

		for (it++; it != itToLast; ++it) {
			if (Ordering::less(*lTerm, *it)) {
				lTerm = it;
			} else {
				assert(!Term<Coeff>::monomialEqual(**lTerm, **it));
			}
		}

		assert(!Term<Coeff>::monomialEqual(**lTerm, **itToLast));
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
void MultivariatePolynomial<Coeff, Ordering, Policies>::setTerms(std::vector<std::shared_ptr<const Term<Coeff>>>& newTerms)
{
	///@todo make this obsolete
	while ((newTerms.size() > 0) && newTerms.back()->isZero()) {
		newTerms.pop_back();
	}

	mTerms.clear();
	if(newTerms.empty())
	{
        assert(this->isConsistent());
		return;
	}
	else if(newTerms.size() == 1)
	{
		mTerms.push_back(newTerms.back());
        assert(this->isConsistent());
		return;
	}
	// Sort the entries from newterms.
	// As automatic template deduction will not work (Ordering::less is overloaded), we give an explicit function pointer cast.
	std::sort(newTerms.begin(), newTerms.end(), (bool (&)(std::shared_ptr<const Term<Coeff>> const&, std::shared_ptr<const Term<Coeff>> const&))Ordering::less);
	// remove duplicates by adding their coefficients.
	// list.unique() fails because it does not handle coefficient updates.
	std::shared_ptr<const Term<Coeff>> frontTerm = newTerms.front();
	Coeff frontCoeff(frontTerm->coeff());
	
	for(auto it = ++newTerms.begin(); it != newTerms.end(); ++it)
	{
		if(Ordering::compare(*frontTerm, **it) == CompareResult::EQUAL)
		{
			// Do not add yet, but simply add the coefficient.
			frontCoeff += (*it)->coeff();
		}
		else
		{
			if (!carl::isZero(frontCoeff)) {
				if(frontCoeff == frontTerm->coeff())
				{
					mTerms.push_back(frontTerm);
				}
				else if(frontCoeff != (Coeff)0)
				{
					mTerms.emplace_back(std::make_shared<const Term<Coeff>>(frontCoeff, frontTerm->monomial()));
				}
			}
			frontTerm = *it;
			frontCoeff = (*it)->coeff();
		}
	}
	
	if(frontCoeff == frontTerm->coeff())
	{
		assert(!carl::isZero(frontCoeff));
		mTerms.push_back(frontTerm);
	}
	else if (!carl::isZero(frontCoeff))
	{
		mTerms.emplace_back(std::make_shared<const Term<Coeff>>(frontCoeff, frontTerm->monomial()));
	}
//	assert( newTerms.empty() );
    assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff, Ordering, Policies>::isConsistent() const {
	for (unsigned i = 0; i < this->mTerms.size(); i++) {
		assert(this->mTerms[i]);
		assert(!this->mTerms[i]->isZero());
		if (i > 0) {
			assert(this->mTerms[i]->tdeg() > 0);
		}
	}
	if (mOrdered) {
		for (unsigned i = 1; i < this->mTerms.size(); i++) {
			if (!Ordering::less(mTerms[i-1], mTerms[i])) {
				std::cout << "Error for " << mTerms[i-1] << " < " << mTerms[i] << std::endl;
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
					std::cout << "Error for " << mTerms[i] << " < " << lterm() << std::endl;
				}
				assert(Ordering::less(mTerms[i], lterm()));
			}	
		}
	}
	return true;
}

}
