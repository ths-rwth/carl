/**
 * @file MultivariatePolynomial.tpp
 * @ingroup multirp
 * @author Sebastian Junges
 */

#pragma once

#include "MultivariatePolynomial.h"

#include "Term.h"
#include "UnivariatePolynomial.h"
#include "logging.h"
#include "../numbers/numbers.h"

#include "polynomialfunctions/CoprimePart.h"

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
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(EnableIfNotSame<C,sint> c) : MultivariatePolynomial(fromInt<Coeff>(c))
{
	mOrdered = true;
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename C>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(EnableIfNotSame<C,uint> c) : MultivariatePolynomial(fromInt<Coeff>(c))
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
	if (t.isZero()) {
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
		if (!carl::isZero(c)) {
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
	if (isZero()) return 0;
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
	if(isZero()) return constant_zero<Coeff>::get();
	if(trailingTerm().isConstant()) {
		return trailingTerm().coeff();
	}
	return constant_zero<Coeff>::get();
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isLinear() const {
	if (mTerms.empty()) return true;
	if (Ordering::degreeOrder) {
		return lterm().isLinear();
	} else {
		return std::all_of(mTerms.begin(), mTerms.end(),
			[](const auto& t){ return t.isLinear(); }
		);
	}
}

template<typename Coeff, typename Ordering, typename Policies>
Definiteness MultivariatePolynomial<Coeff,Ordering,Policies>::definiteness( bool _fullEffort ) const
{
	// Todo: handle constant polynomials
	if (isLinear()) {
		CARL_LOG_DEBUG("carl.core", "Linear and hence " << Definiteness::NON);
		return Definiteness::NON;
	}
	auto term = mTerms.rbegin();
	if (term == mTerms.rend()) return Definiteness::NON;
	Definiteness result = term->definiteness();
	CARL_LOG_DEBUG("carl.core", "Got " << result << " from first term " << *term);
	++term;
	if (term == mTerms.rend()) return result;
	if (result > Definiteness::NON) {
		for (; term != mTerms.rend(); ++term) {
			Definiteness termDefin = (term)->definiteness();
			if (termDefin > Definiteness::NON) {
				if( termDefin > result ) result = termDefin;
			} else {
                result = Definiteness::NON;
                break;
            }
		}
	} else if (result < Definiteness::NON) {
		for (; term != mTerms.rend(); ++term) {
			Definiteness termDefin = (term)->definiteness();
			if (termDefin < Definiteness::NON) {
				if( termDefin < result ) result = termDefin;
			} else {
                result = Definiteness::NON;
                break;
            }
		}
	}
	CARL_LOG_DEBUG("carl.core", "Eventually got " << result);
    if( _fullEffort && result == Definiteness::NON && totalDegree() == 2 )
    {
        assert( !isConstant() );
        bool lTermNegative = carl::isNegative( lterm().coeff() );
        MultivariatePolynomial<Coeff,Ordering,Policies> tmp = *this;
        if( hasConstantTerm() )
        {
            bool constPartNegative = carl::isNegative( constantPart() );
            if( constPartNegative != lTermNegative ) return Definiteness::NON;
            result = lTermNegative ? Definiteness::NEGATIVE : Definiteness::POSITIVE;
            tmp -= constantPart();
        }
        else
        {
            result = lTermNegative ? Definiteness::NEGATIVE_SEMI : Definiteness::POSITIVE_SEMI;
        }
        if( lTermNegative )
            tmp = -tmp;
        if( !tmp.sosDecomposition( true ).empty() ) return result;
        return Definiteness::NON;
    }
	return result;
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
	if (p.isZero()) return;
	if (isZero()) {
		*this = - factor * p;
        assert(this->isConsistent());
	}
	if (carl::isZero(factor.coeff())) return;
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
			if (carl::isZero(trailingTerm().coeff())) {
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
				if (carl::isZero(it->coeff())) {
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
			if (carl::isZero(lterm().coeff())) {
				mTerms.pop_back();
				makeMinimallyOrdered<false,true>();
			}
			break;
		}
		case CompareResult::GREATER: {
			for (auto it = mTerms.begin(); it != mTerms.end(); it++) {
				if (Ordering::equal(*it, term)) {
					it->coeff() += term.coeff();
					if (carl::isZero(it->coeff())) {
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
template<typename C, EnableIf<is_field<C>>>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::divideBy(const Coeff& divisor) const
{
	MultivariatePolynomial<Coeff,Ordering,Policies> res;
	res.mTerms.reserve(mTerms.size());
	for (unsigned i = 0; i < mTerms.size(); i++) {
		Term<Coeff> tmp;
		if (mTerms[i].divide(divisor, tmp)) {
			res.mTerms.push_back(tmp);
		}
	}
	res.mOrdered = this->mOrdered;
    assert(res.isConsistent());
	return res;
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename C, EnableIf<is_field<C>>>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::divideBy(const MultivariatePolynomial<Coeff,Ordering,Policies>& divisor, MultivariatePolynomial<Coeff,Ordering,Policies>& quotient) const
{
	static_assert(is_field<C>::value, "Division only defined for field coefficients");
	if (divisor.isOne()) {
		quotient = *this;
		return true;
	}
	if (this->isZero()) {
		quotient = MultivariatePolynomial();
		return true;
	}
	auto id = mTermAdditionManager.getId(0);
	auto thisid = mTermAdditionManager.getId(mTerms.size());
	for (const auto& t: mTerms) {
		mTermAdditionManager.template addTerm<false,true>(thisid, t);
	}
	while (true) {
		Term<C> factor = mTermAdditionManager.getMaxTerm(thisid);
		if (factor.isZero()) break;
		if (factor.divide(divisor.lterm(), factor)) {
			for (const auto& t: divisor) {
				mTermAdditionManager.template addTerm<true,true>(thisid, -factor*t);
			}
			//res.subtractProduct(factor, divisor);
			//p -= factor * divisor;
			mTermAdditionManager.template addTerm<true>(id, factor);
		} else {
			return false;
		}
	}
	mTermAdditionManager.readTerms(id, quotient.mTerms);
	mTermAdditionManager.dropTerms(thisid);
	quotient.mOrdered = false;
	quotient.makeMinimallyOrdered<false, true>();
	assert(quotient.isConsistent());
	return true;
}

template<typename C, typename O, typename P>
DivisionResult<MultivariatePolynomial<C,O,P>> MultivariatePolynomial<C,O,P>::divideBy(const MultivariatePolynomial& divisor) const
{
	static_assert(is_field<C>::value, "Division only defined for field coefficients");
	MultivariatePolynomial<C,O,P> q;
	MultivariatePolynomial<C,O,P> r;
	MultivariatePolynomial p = *this;
	while(!p.isZero())
	{
		Term<C> factor;
		if (p.lterm().divide(divisor.lterm(), factor)) {
			q += factor;
			p.subtractProduct(factor, divisor);
			//p -= factor * divisor;
		}
		else
		{
			r += p.lterm();
			p.stripLT();
		}
	}
	assert(q.isConsistent());
	assert(r.isConsistent());
	assert(*this == q * divisor + r);
	return DivisionResult<MultivariatePolynomial<C,O,P>>(q,r);
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
	auto id = mTermAdditionManager.getId(p.mTerms.size());
	while(!p.isZero())
	{
		Term<C> factor;
		if (p.lterm().divide(divisor.lterm(), factor)) {
			//p -= factor * divisor;
			p.subtractProduct(factor, divisor);
			mTermAdditionManager.template addTerm<true>(id, factor);
		}
		else
		{
			p.stripLT();
		}
	}
	MultivariatePolynomial<C,O,P> result;
	mTermAdditionManager.readTerms(id, result.mTerms);
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
		if(p.lterm().tdeg() < divisor.lterm().tdeg())
		{
			assert(!p.lterm().divisible(divisor.lterm()));
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
			Term<C> factor;
			if (p.lterm().divide(divisor.lterm(), factor)) {
				p.subtractProduct(factor, divisor);
				//p -= factor * divisor;
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

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> MultivariatePolynomial<C,O,P>::prem(const MultivariatePolynomial& divisor, Variable::Arg var) const
{
	assert(!divisor.isZero());
	return MultivariatePolynomial(this->toUnivariatePolynomial(var).prem(divisor.toUnivariatePolynomial(var)));
	return this->remainder(divisor);
}

template<typename Coeff, typename Ordering, typename Policies>
void MultivariatePolynomial<Coeff,Ordering,Policies>::substituteIn(Variable::Arg var, const MultivariatePolynomial<Coeff, Ordering, Policies>& value)
{
	assert(this->isConsistent());
	assert(value.isConsistent());
	if (!this->has(var)) {
		return;
	}
	TermsType newTerms;
	// If we replace a variable by zero, just eliminate all terms containing the variable.
	if(value.isZero())
	{
		bool removedLast = false;
		for (const auto& term: mTerms) {
			if (!term.has(var)) {
				newTerms.push_back(term);
				removedLast = false;
			} else removedLast = true;
		}
		mTerms.swap(newTerms);
		CARL_LOG_TRACE("carl.core", *this << " [ " << var << " -> " << value << " ] = " << *this);
		if (removedLast) {
			mOrdered = false;
			makeMinimallyOrdered<false, true>();
		}
        assert(this->isConsistent());
		return;
	}
	// Find all exponents occurring with the variable to substitute as basis.
	// expResults will finally be a mapping from every exponent e for which var^e occurs to the value^e and the number of times var^e occurs.
	// Meanwhile, we store an upper bound on the expected number of terms of the result in expectedResultSize.
	std::map<exponent, std::pair<MultivariatePolynomial, size_t>> expResults;
	size_t expectedResultSize = 0;
	std::pair<MultivariatePolynomial, unsigned> def( MultivariatePolynomial(constant_one<Coeff>::get()), 1 );
	for(const auto& term: mTerms)
	{
		if(term.monomial())
		{ // This is not the constant part.
			exponent e = term.monomial()->exponentOfVariable(var);
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
	auto id = mTermAdditionManager.getId(expectedResultSize);
	for (const auto& term: mTerms)
	{
		if (term.monomial() == nullptr) {
			mTermAdditionManager.template addTerm<false>(id, term);
		} else {
			exponent e = term.monomial()->exponentOfVariable(var);
			Monomial::Arg mon;
			if (e > 0) mon = term.monomial()->dropVariable(var);
			if (e == 1) {
				for(auto vterm : value.mTerms)
				{
					if (mon == nullptr) mTermAdditionManager.template addTerm<false>(id, Term<Coeff>(vterm.coeff() * term.coeff(), vterm.monomial()));
					else if (vterm.monomial() == nullptr) mTermAdditionManager.template addTerm<false>(id, Term<Coeff>(vterm.coeff() * term.coeff(), mon));
					else mTermAdditionManager.template addTerm<false>(id, Term<Coeff>(vterm.coeff() * term.coeff(), vterm.monomial() * mon));
				}
			} else if(e > 1) {
				auto iter = expResults.find(e);
				assert(iter != expResults.end());
				for(auto vterm : iter->second.first.mTerms)
				{
					if (mon == nullptr) mTermAdditionManager.template addTerm<false>(id, Term<Coeff>(vterm.coeff() * term.coeff(), vterm.monomial()));
					else if (vterm.monomial() == nullptr) mTermAdditionManager.template addTerm<false>(id, Term<Coeff>(vterm.coeff() * term.coeff(), mon));
					else mTermAdditionManager.template addTerm<false>(id, Term<Coeff>(vterm.coeff() * term.coeff(), vterm.monomial() * mon));
				}
			}
			else
			{
				mTermAdditionManager.template addTerm<false>(id, term);
			}
		}
	}
	mTermAdditionManager.readTerms(id, mTerms);
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
		if(term.monomial())
		{
			const Monomial& m = *(term.monomial());
			CARL_LOG_TRACE("carl.core.monomial", "Iterating over " << m);
			for(unsigned i = 0; i < m.nrVariables(); ++i)
			{
				CARL_LOG_TRACE("carl.core.monomial", "Iterating: " << m[i].first);
				if(m[i].second > 1 && substitutions.find(m[i].first) != substitutions.end())
				{
					expResults[m[i]] = MultivariatePolynomial(constant_one<Coeff>::get());
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
		expResultB->second = sub->second.pow(expResultB->first.second);
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
	MultivariatePolynomial resultB;
	// Substitute the variable for which all occurring exponentiations are calculated.
	for(const auto& term: result.mTerms)
	{
		MultivariatePolynomial termResult(term.coeff());
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
						termResult *= Term<Coeff>(constant_one<Coeff>::get(), m[i].first, m[i].second);
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
	auto id = mTermAdditionManager.getId(mTerms.size());
	for (const auto& term: mTerms) {
        Term<Coeff> resultTerm = term.substitute(substitutions);
        if( !resultTerm.isZero() )
        {
            mTermAdditionManager.template addTerm<false>(id, resultTerm );
        }
	}
	mTermAdditionManager.readTerms(id, result.mTerms);
	result.mOrdered = false;
    result.makeMinimallyOrdered<false, true>();
	assert(result.isConsistent());
	return result;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies> MultivariatePolynomial<Coeff, Ordering, Policies>::substitute(const std::map<Variable, Term<Coeff>>& substitutions) const
{
	MultivariatePolynomial result;
	auto id = mTermAdditionManager.getId(mTerms.size());
	for (const auto& term: mTerms) {
		mTermAdditionManager.template addTerm<false>(id, term.substitute(substitutions));
	}
	mTermAdditionManager.readTerms(id, result.mTerms);
	result.mOrdered = false;
	result.makeMinimallyOrdered<false, true>();
	assert(result.isConsistent());
	return result;
}


template<typename Coeff, typename Ordering, typename Policies>
template<typename SubstitutionType>
SubstitutionType MultivariatePolynomial<Coeff,Ordering,Policies>::evaluate(const std::map<Variable,SubstitutionType>& substitutions) const
{
	if(isZero()) {
		return constant_zero<SubstitutionType>::get();
	} else {
		SubstitutionType result(mTerms[0].evaluate(substitutions)); 
		for (unsigned i = 1; i < mTerms.size(); ++i) {
			result += mTerms[i].evaluate(substitutions);
		}
		return result;
	};
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename C, EnableIf<is_subset_of_rationals<C>>>
Coeff MultivariatePolynomial<Coeff,Ordering,Policies>::coprimeFactor() const
{
	assert(nrTerms() != 0);
	typename TermsType::const_iterator it = mTerms.begin();
	typename IntegralType<Coeff>::type num = carl::abs(getNum((it)->coeff()));
	typename IntegralType<Coeff>::type den = carl::abs(getDenom((it)->coeff()));
	for(++it; it != mTerms.end(); ++it)
	{
		num = carl::gcd(num, getNum((it)->coeff()));
		den = carl::lcm(den, getDenom((it)->coeff()));
	}
    if( carl::isNegative(lcoeff()) )
    {
        return Coeff(den)/Coeff(-num);
    }
    else
    {
        return Coeff(den)/Coeff(num);
    }
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename C, EnableIf<is_subset_of_rationals<C>>>
Coeff MultivariatePolynomial<Coeff,Ordering,Policies>::coprimeFactorWithoutConstant() const
{
	assert(nrTerms() != 0);
	typename TermsType::const_iterator it = mTerms.begin();
	if (it->isConstant()) ++it;
	typename IntegralType<Coeff>::type num = carl::abs(getNum((it)->coeff()));
	typename IntegralType<Coeff>::type den = carl::abs(getDenom((it)->coeff()));
	for(++it; it != mTerms.end(); ++it)
	{
		num = carl::gcd(num, getNum((it)->coeff()));
		den = carl::lcm(den, getDenom((it)->coeff()));
	}
    if( carl::isNegative(lcoeff()) )
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
	if(nrTerms() == 0) return *this;
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
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::normalize() const
{
	MultivariatePolynomial result(*this);
	if(Policies::has_reasons)
	{
		result.setReasons(this->getReasons());
	}
    if (isZero()) return result;
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
    if (isZero()) {
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
		tmpTerms.push_back(t.derivative(v));
		if (tmpTerms.back().isZero()) tmpTerms.pop_back();
	}
	MultivariatePolynomial result(std::move(tmpTerms), true, mOrdered);
    assert(result.isConsistent());
	return result;
}

template<typename Coeff, typename Ordering, typename Policies>
void MultivariatePolynomial<Coeff,Ordering,Policies>::square()
{
	assert(this->isConsistent());
	auto id = mTermAdditionManager.getId(mTerms.size() * mTerms.size());
	Term<Coeff> newlterm;
	for (auto it1 = mTerms.rbegin(); it1 != mTerms.rend(); it1++) {
		if (it1 == mTerms.rbegin()) newlterm = it1->pow(2);
		else mTermAdditionManager.template addTerm<false>(id, it1->pow(2));
		for (auto it2 = it1+1; it2 != mTerms.rend(); it2++) {
			mTermAdditionManager.template addTerm<false>(id, Coeff(2) * *it1 * *it2);
		}
	}
	mOrdered = false;
	mTermAdditionManager.readTerms(id, mTerms);
	if (!newlterm.isZero()) mTerms.push_back(newlterm);
	assert(this->isConsistent());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::pow(std::size_t exp) const
{
	//std::cout << "pw(" << *this << " ^ " << exp << ")" << std::endl;
	if (isZero()) return MultivariatePolynomial(constant_zero<Coeff>::get());
	if (exp == 0) return MultivariatePolynomial(constant_one<Coeff>::get());
	if (exp == 1) return MultivariatePolynomial(*this);
	if (exp == 2) return *this * *this;
	MultivariatePolynomial<Coeff,Ordering,Policies> res(constant_one<Coeff>::get());
	MultivariatePolynomial<Coeff,Ordering,Policies> mult(*this);
	while(exp > 0) {
#if 0
		if (exp & 1) res *= mult;
		exp /= 2;
		if(exp > 0) mult.square();
#else
		res *= mult;
		exp--;
#endif
	}
	return res;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::naive_pow(unsigned exp) const
{
	if (exp == 0) {
		return MultivariatePolynomial(constant_one<Coeff>::get());
	}

	CARL_LOG_INEFFICIENT();
	MultivariatePolynomial<Coeff,Ordering,Policies> res(*this);
	for (unsigned i = 1; i < exp; i++) {
		res *= *this;
	}
	return res;
}

template<typename Coeff, typename Ordering, typename Policies>
void MultivariatePolynomial<Coeff,Ordering,Policies>::gatherVariables(std::set<Variable>& vars) const
{
	for(const auto& t : mTerms)
	{
		t.gatherVariables(vars);
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

template<typename C, typename O, typename P>
UnivariatePolynomial<C> MultivariatePolynomial<C,O,P>::toUnivariatePolynomial() const
{
	// Only correct when it is already only in one variable.
	assert(gatherVariables().size() == 1);
	Variable::Arg x = lmon()->getSingleVariable();
	std::vector<C> coeffs(totalDegree()+1,0);
	for (const auto& t : mTerms)
	{
		coeffs[t.tdeg()] = t.coeff();
	}
	return UnivariatePolynomial<C>(x, coeffs);
}

template<typename C, typename O, typename P>
UnivariatePolynomial<MultivariatePolynomial<C,O,P>> MultivariatePolynomial<C,O,P>::toUnivariatePolynomial(Variable::Arg v) const
{
	assert(this->isConsistent());
	std::vector<MultivariatePolynomial<C,O,P>> coeffs(1);
	for (const auto& term: this->mTerms) {
		if (term.monomial() == nullptr) coeffs[0] += term;
		else {
			auto mon = term.monomial();
			auto exponent = mon->exponentOfVariable(v);
			if (exponent >= coeffs.size()) {
				coeffs.resize(exponent + 1);
			}
			std::shared_ptr<const carl::Monomial> tmp = mon->dropVariable(v);
			coeffs[exponent] += term.coeff() * tmp;
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
	typename UnderlyingNumberType<C>::type res = this->mTerms.front().coeff();
	for (unsigned i = 0; i < this->mTerms.size(); i++) {
		///@todo gcd needed for fractions
		res = carl::gcd(res, this->mTerms[i].coeff());
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
		res = carl::lcm(res, getDenom(t.coeff()));
	}
	return res;
}

template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	// Try to avoid sorting
	if (&lhs == &rhs) return true;
	if (lhs.nrTerms() != rhs.nrTerms()) return false;
	if (lhs.nrTerms() == 0) return true;
#ifdef THREAD_SAFE
	static std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);
#endif
	static std::vector<const C*> coeffs;
	coeffs.resize(MonomialPool::getInstance().largestID() + 1);
	memset(&coeffs[0], 0, sizeof(typename std::vector<const C*>::value_type)*coeffs.size());
	for (const auto& t: lhs.mTerms) {
		std::size_t id = 0;
		if (t.monomial()) id = t.monomial()->id();
		coeffs[id] = &t.coeff();
	}
	for (const auto& t: rhs.mTerms) {
		std::size_t id = 0;
		if (t.monomial()) id = t.monomial()->id();
		if ((coeffs[id] == nullptr) || *coeffs[id] != t.coeff()) return false;
	}
	return true;
}

template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
	if (lhs.isZero() && carl::isZero(rhs.coeff())) return true;
	if (lhs.nrTerms() > 1) return false;
	return lhs.lterm() == rhs;
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
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, int rhs) {
	if (lhs.isZero()) return rhs == 0;
	if (lhs.nrTerms() > 1) return false;
	if (lhs.lmon() != nullptr) return false;
	return lhs.lcoeff() == rhs;
}

template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs) {
	if (lhs.nrTerms() != 1) return false;
	if (lhs.lmon() == nullptr) return false;
	return lhs.lmon() == rhs;
}

template<typename C, typename O, typename P>
bool operator==(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	CARL_LOG_INEFFICIENT();
	return MultivariatePolynomial<C,O,P>(lhs) == rhs;
}

template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs) {
	return rhs == lhs;
}

template<typename C, typename O, typename P>
bool operator==(const UnivariatePolynomial<MultivariatePolynomial<C>>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	CARL_LOG_INEFFICIENT();
	return MultivariatePolynomial<C>(lhs) == rhs;
}

template<typename C, typename O, typename P>
bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (lhs.isZero() && rhs.isZero()) return false;
	if (lhs.isZero()) return true;
	if (rhs.isZero()) return false;
	if (lhs.totalDegree() != rhs.totalDegree()) return lhs.totalDegree() < rhs.totalDegree();
	if (lhs.totalDegree() == 0) return lhs.constantPart() < rhs.constantPart();
	if (lhs.lterm() < rhs.lterm()) return true;
	if (lhs.lterm() > rhs.lterm()) return false;
	// We have to sort, but we can skip the leading term.
	lhs.makeOrdered();
	rhs.makeOrdered();
	auto lit = lhs.rbegin();
	auto rit = rhs.rbegin();
	for (lit++, rit++; lit != lhs.rend(); lit++, rit++) {
		if (rit == rhs.rend()) return false;
		if (*lit < *rit) return true;
		if (*lit > *rit) return false;
	}
	return rit != rhs.rend();
}
template<typename C, typename O, typename P>
bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
	if (lhs.isZero()) return carl::constant_zero<C>().get() < rhs;
	return (lhs.lterm()) < rhs;
}
template<typename C, typename O, typename P>
bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const std::shared_ptr<const carl::Monomial>& rhs) {
	if (lhs.nrTerms() == 0) return true;
	return (lhs.lterm()) < rhs;
}
template<typename C, typename O, typename P>
bool operator<(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs) {
	if (lhs.nrTerms() == 0) return true;
	return (lhs.lterm()) < rhs;
}
template<typename C, typename O, typename P>
bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
	if (lhs.nrTerms() == 0) return carl::constant_zero<C>().get() < rhs;
	return (lhs.lterm()) < rhs;
}
template<typename C, typename O, typename P>
bool operator<(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (rhs.nrTerms() == 0) return lhs < carl::constant_zero<C>().get();
	if (lhs < (rhs.lterm())) return true;
	if (lhs == (rhs.lterm())) return rhs.nrTerms() > 1;
	return false;
}
template<typename C, typename O, typename P>
bool operator<(const std::shared_ptr<const carl::Monomial>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (rhs.nrTerms() == 0) return false;
	if (lhs < (rhs.lterm())) return true;
	if (lhs == (rhs.lterm())) return rhs.nrTerms() > 1;
	return false;
}
template<typename C, typename O, typename P>
bool operator<(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (rhs.nrTerms() == 0) return false;
	if (lhs < (rhs.lterm())) return true;
	if (lhs == (rhs.lterm())) return rhs.nrTerms() > 1;
	return false;
}
template<typename C, typename O, typename P>
bool operator<(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (rhs.nrTerms() == 0) return lhs < carl::constant_zero<C>().get();
	return lhs < (rhs.lterm());
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
	if (newlterm.isZero()) {
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
	if (carl::isZero(rhs.coeff())) return *this;
	if (mTerms.empty()) {
		// Empty -> just insert.
		mTerms.push_back(rhs);
		mOrdered = true;
	} else if (rhs.isConstant()) {
		if (this->hasConstantTerm()) {
			// Combine with constant term.
			if (carl::isZero(Coeff(mTerms.front().coeff() + rhs.coeff()))) {
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
		if (carl::isZero(Coeff(lcoeff() + rhs.coeff()))) {
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
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(Variable::Arg rhs)
{
	return *this += MonomialPool::getInstance().create(rhs, 1);
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const Coeff& rhs)
{
	if (carl::isZero(rhs)) return *this;
	if (hasConstantTerm()) {
		if (carl::isZero(Coeff(constantPart() + rhs))) mTerms.erase(mTerms.begin());
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
	if (carl::isZero(rhs.coeff())) return *this;
	CARL_LOG_TRACE("carl.core", *this << " -= " << rhs);
	if (Policies::searchLinear)
	{
		typename TermsType::iterator it(mTerms.begin());
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
		typename TermsType::iterator it(mTerms.begin());
		while(it != mTerms.end())
		{
			if ((*it).monomial() != nullptr) {
				CompareResult cmpres(Ordering::compare((*it).monomial(), rhs));
				if( mOrdered && cmpres == CompareResult::GREATER ) break;
				if( cmpres == CompareResult::EQUAL )
				{
					// new coefficient would be zero, simply removing is enough.
					if(carl::isOne((*it).coeff()))
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
	if (this->isOne()) {
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
	if (newlterm.isZero()) makeMinimallyOrdered<false, true>();
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
	if (carl::isOne(rhs)) return *this;
	if (carl::isZero(rhs))
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
	assert(!carl::isZero(rhs));
	if (carl::isOne(rhs)) return *this;
	for (auto& term : mTerms) {
		term.coeff() /= rhs;
	}
	return *this;
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> operator/(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	MultivariatePolynomial<C,O,P> res;
	bool flag = lhs.divideBy(rhs, res);
	assert(flag);
	return res;
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

template<typename C, typename O, typename P>
std::ostream& operator<<(std::ostream& os, const MultivariatePolynomial<C,O,P>& p) {
	if (p.isZero()) return os << "0";
	return os << carl::stream_joined(" + ", p);
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
		assert(!this->mTerms[i].isZero());
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
