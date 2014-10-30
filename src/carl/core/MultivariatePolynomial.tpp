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

#include "Term.h"
#include "UnivariatePolynomial.h"
#include "logging.h"
#include "../numbers/numbers.h"

namespace carl
{
    
template<typename Coeff, typename Ordering, typename Policies>
TermAdditionManager<MultivariatePolynomial<Coeff,Ordering,Policies>> MultivariatePolynomial<Coeff,Ordering,Policies>::mTermAdditionManager = TermAdditionManager<MultivariatePolynomial<Coeff,Ordering,Policies>>();

template<typename Coeff, typename Ordering, typename Policies>
void MultivariatePolynomial<Coeff,Ordering,Policies>::resizeTermAdditionManager( size_t _newSize )
{
    mTermAdditionManager.resize( _newSize );
}
   
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(int c) : MultivariatePolynomial((Coeff)c)
{
	this->checkConsistency();
	mOrdered = true;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(unsigned c) : MultivariatePolynomial((Coeff)c)
{
	this->checkConsistency();
	mOrdered = true;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const Coeff& c) :
Policies(),
mTerms(c == (Coeff)0 ? 0 : 1,std::make_shared<const Term<Coeff>>(c))
{
	this->checkConsistency();
	mOrdered = true;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(Variable::Arg v) :
Policies(),
mTerms(1,std::make_shared<const Term<Coeff>>(v))
{
	this->checkConsistency();
	mOrdered = true;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const Monomial& m) :
Policies(),
mTerms(1,std::make_shared<const Term<Coeff>>(m))
{
	this->checkConsistency();
	mOrdered = true;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const Term<Coeff>& t) :
Policies(),
mTerms(1,std::make_shared<const Term<Coeff>>(t))
{
	if (t.isZero()) {
		this->mTerms.clear();
	}
	this->checkConsistency();
	mOrdered = true;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(std::shared_ptr<const Monomial> m) :
Policies(),
mTerms(1,std::make_shared<const Term<Coeff>>(m))
{
	this->checkConsistency();
	mOrdered = true;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(std::shared_ptr<const Term<Coeff>> t) :
Policies(),
mTerms(1,t)
{
	if (t->isZero()) {
		this->mTerms.clear();
	}
	this->checkConsistency();
	mOrdered = true;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const UnivariatePolynomial<MultivariatePolynomial<Coeff, Ordering, Policies>>& p) :
Policies()
{
	///@todo is += needed here? take care of leading term
	if (p.coefficients().size() > 0) {
		*this += p.coefficients()[0];
	}
	for (unsigned deg = 1; deg < p.coefficients().size(); deg++) {
		*this += p.coefficients()[deg] * Term<Coeff>(1, p.mainVar(), deg);
	}
	this->checkConsistency();
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const UnivariatePolynomial<Coeff>& p) :
Policies()
{
	///@todo is += needed here? take care of leading term
	if (p.coefficients().size() > 0) {
		*this += p.coefficients()[0];
	}
	for (unsigned deg = 1; deg < p.coefficients().size(); deg++) {
		*this += p.coefficients()[deg] * Term<Coeff>(Coeff(1), p.mainVar(), deg);
	}
	this->checkConsistency();
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename OtherPolicies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const MultivariatePolynomial<Coeff, Ordering, OtherPolicies>& pol) :
	Policies(),
	mTerms(pol.begin(), pol.end()),
	mOrdered(pol.isOrdered())
{
	this->checkConsistency();
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename InputIterator>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(InputIterator begin, InputIterator end, bool duplicates, bool sorted) :
Policies()
{
	///@todo make this more efficient, ordering is not strictly necessary
	mTerms.assign(begin, end);
	if(!sorted)
	{
		makeOrdered();
	}
	if(duplicates)
	{
		// We now iterate over the terms to find equal monomials.
		for(typename TermsType::iterator it=mTerms.begin(); it != mTerms.end(); )
		{
			// look ahead for equal monomials
			typename TermsType::iterator jt=it;
			Coeff c = (*it)->coeff();
			for(++jt; jt != mTerms.end(); ++jt)
			{
				if(Term<Coeff>::monomialEqual(**jt, **it)) 
				{
					c += (*jt)->coeff();
					// We do not yet remove the term as this would cause multiple movements
					// over the whole operation. Instead, we write a zero and clear these zeros later on.
					// TODO make a global shared ptr for zero terms.
					*jt = std::make_shared<const Term<Coeff>>((Coeff)0);
				}
				else
				{
					break;
				}
			}
			if(c != (*it)->coeff())
			{
				if(c == (Coeff)0)
				{
					*it = std::make_shared<const Term<Coeff>>((Coeff)0);
				}
				else
				{
					*it = std::make_shared<const Term<Coeff>>(c, (**it).monomial());
				}
				// We need to update this iterator.
			}
			
			// Go on where the look ahead stopped.
			it = jt;
		}
		// Now we have to remove zeros.
		for(typename TermsType::iterator it=mTerms.begin(); it != mTerms.end(); )
		{
			if(**it == (Coeff)0)
			{
				it = mTerms.erase(it);
			}
			else
			{
				++it;
			}
		}
		
	}
	
	this->checkConsistency();
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(MultivariatePolynomial<Coeff, Ordering, Policies>::TermsType&& terms, bool duplicates, bool ordered):
	mTerms(terms),
	mOrdered(ordered)
{
	///@todo search for duplicates
	if (!ordered) {
		makeMinimallyOrdered();
	}
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(const std::initializer_list<Term<Coeff>>& terms)
{
	///@todo make this more efficient, ordering is not strictly necessary
	for(Term<Coeff> term : terms)
	{
		mTerms.push_back(std::make_shared<const Term<Coeff>>(term));
	}
	makeOrdered();
	this->checkConsistency();
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(const std::initializer_list<Variable>& terms)
{
	///@todo make this more efficient, ordering is not strictly necessary
	for(Variable term : terms)
	{
		mTerms.push_back(std::make_shared<const Term<Coeff>>(term));
	}
	makeOrdered();
	this->checkConsistency();
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
	this->checkConsistency();
}
    
template<typename Coeff, typename Ordering, typename Policies>
std::shared_ptr<const Monomial> MultivariatePolynomial<Coeff,Ordering,Policies>::lmon() const
{
    return lterm()->monomial();
}
template<typename Coeff, typename Ordering, typename Policies>
std::shared_ptr<const Term<Coeff>> MultivariatePolynomial<Coeff,Ordering,Policies>::lterm() const
{
    LOG_ASSERT("carl.core", !isZero(), "Leading term undefined on zero polynomials.");
	return mTerms.back();
}

template<typename Coeff, typename Ordering, typename Policies>
std::shared_ptr<const Term<Coeff>> MultivariatePolynomial<Coeff,Ordering,Policies>::trailingTerm() const
{
    LOG_ASSERT("carl.core", !isZero(), "Trailing term undefined on zero polynomials.");
	return mTerms.front();
}

template<typename Coeff, typename Ordering, typename Policies>
Coeff MultivariatePolynomial<Coeff,Ordering,Policies>::lcoeff() const
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
Coeff MultivariatePolynomial<Coeff, Ordering, Policies>::constantPart() const
{
	if(isZero()) return (Coeff)0;
	if(trailingTerm()->isConstant()) {
		return trailingTerm()->coeff();
	}
    return (Coeff)0;
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
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::tail() const
{
	///@todo make sure that the resulting polynomial is minimally ordered
	assert(mTerms.size() != 0);
	if (mTerms.size() == 1) return MultivariatePolynomial();
    MultivariatePolynomial tail;
	tail.mTerms.reserve(mTerms.size()-1);
    tail.mTerms.insert(tail.mTerms.begin(), mTerms.begin(), --mTerms.end());
    return tail;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::stripLT()
{
	///@todo search for new leading term
    assert(!isZero());
    mTerms.pop_back();
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
template<typename C, EnableIf<is_field<C>>>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::divideBy(const Coeff& divisor) const
{
	///@todo could be more efficient
	MultivariatePolynomial<Coeff,Ordering,Policies> res(*this);
	for (unsigned i = 0; i < res.mTerms.size(); i++) {
		res.mTerms[i].reset(res.mTerms[i]->divideBy(divisor));
	}
	return res;
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename C, EnableIf<is_field<C>>>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::divideBy(const MultivariatePolynomial<Coeff,Ordering,Policies>& divisor, MultivariatePolynomial<Coeff,Ordering,Policies>& quotient) const
{
	///@todo ok with ordering?
	static_assert(is_field<C>::value, "Division only defined for field coefficients");
	MultivariatePolynomial p = *this;
	while(!p.isZero())
	{
		Term<C>* factor = p.lterm()->divideBy(*divisor.lterm());
		// nullptr if lt(divisor) does not divide lt(p).
		if(factor != nullptr)
		{
			quotient += *factor;
			p -= *factor * divisor;
			delete factor;
		}
		else
		{
			return false;
		}
		
	}
	return true;
}

template<typename C, typename O, typename P>
DivisionResult<MultivariatePolynomial<C,O,P>> MultivariatePolynomial<C,O,P>::divideBy(const MultivariatePolynomial& divisor) const
{
	///@todo ok with ordering?
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
			p -= *factor * divisor;
			delete factor;
		}
		else
		{
			result.remainder += p.lterm();
			p.stripLT();
		}
		
	}
	assert(*this == result.quotient * divisor + result.remainder);
	return result;
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> MultivariatePolynomial<C,O,P>::quotient(const MultivariatePolynomial& divisor) const
{
	///@todo ok with ordering?
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
	MultivariatePolynomial<C,O,P> result;
	MultivariatePolynomial p = *this;
	while(!p.isZero())
	{
		Term<C>* factor = p.lterm()->divideBy(*divisor.lterm());
		// nullptr if lt(divisor) does not divide lt(p).
		if(factor != nullptr)
		{
			result += *factor;
			p -= *factor * divisor;
			delete factor;
		}
		else
		{
			p.stripLT();
		}
	}
	return result;
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> MultivariatePolynomial<C,O,P>::remainder(const MultivariatePolynomial& divisor) const
{
	///@todo ok with ordering?
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
                p -= *factor * divisor;
                delete factor;
            }
            else
            {
                remainder += p.lterm();
                p.stripLT();
            }
        }
	}
	assert(*this == quotient(divisor) * divisor + remainder);
	return remainder;
}





template<typename Coeff, typename Ordering, typename Policies>
void MultivariatePolynomial<Coeff,Ordering,Policies>::substituteIn(Variable::Arg var, const MultivariatePolynomial<Coeff, Ordering, Policies>& value)
{
	///@todo ok with ordering?
	this->checkConsistency();
	value.checkConsistency();
    if (!this->has(var)) {
        return;
    }
	std::stringstream ss;
	ss << *this;
    TermsType newTerms;
    // If we replace a variable by zero, just eliminate all terms containing the variable.
    if(value.isZero())
    {
        for(auto term : mTerms) {
            if (!term->has(var)) {
                newTerms.push_back(term);
            }
        }
        mTerms.swap(newTerms);
		LOGMSG_TRACE("carl.core", ss.str() << " [ " << var << " -> " << value << " ] = " << *this);
        return;
    }
    // Find all exponents occurring with the variable to substitute as basis.
    // expResults will finally be a mapping from every exponent e for which var^e occurs to the value^e and the number of times var^e occurs.
    // Meanwhile, we store an upper bound on the expected number of terms of the result in expectedResultSize.
    std::map<exponent, std::pair<MultivariatePolynomial, size_t>> expResults;
    size_t expectedResultSize = 0;
    std::pair<MultivariatePolynomial, unsigned> def( MultivariatePolynomial((Coeff) 1), 1 );
    for(auto term : mTerms)
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
//    newTerms.reserve(expectedResultSize);
    MultivariatePolynomial result( Coeff( 0 ) );
    for(auto term : mTerms)
    {
		if (term->monomial() == nullptr) {
//			newTerms.push_back(term);
            result += term->coeff();
		} else {
			exponent e = term->monomial()->exponentOfVariable(var);
            Term<Coeff> t(term->coeff(), term->monomial()->dropVariable(var));
			if(e > 1)
			{
				auto iter = expResults.find(e);
				assert(iter != expResults.end());
//				for(auto vterm : iter->second.first.mTerms)
//				{
//					Term<Coeff> t(term->coeff(), term->monomial()->dropVariable(var));
//					newTerms.push_back(std::make_shared<Term<Coeff>>(*vterm * t));
//				}
                result += iter->second.first * t; 
			}
			else if(e == 1)
			{
//				for(auto vterm : value.mTerms)
//				{
//					Term<Coeff> t(term->coeff(), term->monomial()->dropVariable(var));
//					newTerms.push_back(std::make_shared<Term<Coeff>>(*vterm * t));
//                }
                result += value * t; 
			}
			else
			{
//				newTerms.push_back(term);
                result += *term;
			}
		}
	}
//	setTerms(newTerms);
	assert(mTerms.size() <= expectedResultSize);
	this->checkConsistency();
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
	///@todo ok with ordering?
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
                return result;
            }
        }
    }
    // Find and sort all exponents occurring for all variables to substitute as basis.
    std::map<std::pair<Variable, exponent>, MultivariatePolynomial> expResults;
	for(auto term : result.mTerms)
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
        while(sub->second.isZero()) 
        {
            assert(sub != substitutions.end());
            ++sub;
        }
        assert(sub->first == expResultB->first.first);
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
    for(auto term : result.mTerms)
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
    return resultB;
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename SubstitutionType>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::substitute(const std::map<Variable,SubstitutionType>& substitutions) const
{
	///@todo ok with ordering?
	static_assert(!std::is_same<SubstitutionType, Term<Coeff>>::value, "Terms are handled by a seperate method.");
    TermsType newTerms;
    MultivariatePolynomial result( Coeff( 0 ) );
	for(auto term : mTerms)
	{
		Term<Coeff>* t = term->substitute(substitutions);
		if(t->coeff() != (Coeff)0)
		{
			newTerms.push_back(std::shared_ptr<const Term<Coeff>>(t));
		}
        else
        {
            delete t;
        }
	}   
	result.setTerms(newTerms);
	return result;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies> MultivariatePolynomial<Coeff, Ordering, Policies>::substitute(const std::map<Variable, Term<Coeff>>& substitutions) const
{
	///@todo ok with ordering?
	TermsType newTerms;
    MultivariatePolynomial result( Coeff( 0 ) );
	for(auto term : mTerms)
	{
		Term<Coeff>* t = term->substitute(substitutions);
		if(t->coeff() != (Coeff)0)
		{
			newTerms.push_back(std::shared_ptr<const Term<Coeff>>(t));
		}
//        else
//        {
//            delete t;
//        }
	}   
	result.setTerms(newTerms);
	return result;
}


template<typename Coeff, typename Ordering, typename Policies>
template<typename SubstitutionType>
Coeff MultivariatePolynomial<Coeff,Ordering,Policies>::evaluate(const std::map<Variable,SubstitutionType>& substitutions) const
{
	///@todo make this more efficient
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
	return result;
	
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::derivative(Variable::Arg v, unsigned nth) const
{
	assert(!isZero());
	// TODO n > 1 not yet implemented!
	assert(nth == 1);
	TermsType tmpTerms;
	for(const auto& t : mTerms) {
		tmpTerms.emplace_back(t->derivative(v));
	}
	return MultivariatePolynomial(tmpTerms.begin(), tmpTerms.end(), true, false);
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::SPolynomial(
																const MultivariatePolynomial<Coeff, Ordering, Policies>& p,
																const MultivariatePolynomial<Coeff, Ordering, Policies>& q)
{
	///@todo ok with ordering?
	assert(p.nrTerms() != 0);
	assert(q.nrTerms() != 0);

	
	if( p.nrTerms() == 1 && q.nrTerms() == 1 )
	{
		return MultivariatePolynomial();
	}
	else if( p.nrTerms() == 1 )
	{
		return -(p.lterm()->calcLcmAndDivideBy( *q.lmon() ) * q.tail());
	}
	else if( q.nrTerms() == 1 )
	{
		return (q.lterm()->calcLcmAndDivideBy( *p.lmon() ) * p.tail());
	}
	else
	{
		return (p.tail() * q.lterm()->calcLcmAndDivideBy(*p.lmon())) - (q.tail() * p.lterm()->calcLcmAndDivideBy( *q.lmon() ));
	}
	
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::pow(unsigned exp) const
{
	if (exp == 0) return MultivariatePolynomial((Coeff)1);
	if (isZero()) return MultivariatePolynomial((Coeff)0);
	MultivariatePolynomial<Coeff,Ordering,Policies> res(Coeff(1));
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
	std::vector<MultivariatePolynomial<C,O,P>> coeffs(1);
	for (const auto& term: this->mTerms) {
		if (term->monomial() == nullptr) coeffs[0] += *term;
		else {
			auto mon = term->monomial();
			auto exponent = mon->exponentOfVariable(v);
			if (exponent >= coeffs.size()) {
				coeffs.resize(exponent + 1, MultivariatePolynomial<C,O,P>(0));
			}
			Monomial* tmp = mon->dropVariable(v);
			if (tmp == nullptr) {
				coeffs[exponent] += term->coeff();
			} else {
				coeffs[exponent] += term->coeff() * *tmp;
				delete tmp;
			}
		}
	}
	// Convert result back to MultivariatePolynomial and check that the result is equal to *this
	assert(MultivariatePolynomial<C>(UnivariatePolynomial<MultivariatePolynomial<C,O,P>>(v, coeffs)) == *this);
	return UnivariatePolynomial<MultivariatePolynomial<C,O,P>>(v, coeffs);
}

template<typename Coeff, typename O, typename P>
template<typename C, EnableIf<is_number<C>>>
typename UnderlyingNumberType<C>::type MultivariatePolynomial<Coeff,O,P>::numericContent() const
{
	if (this->isZero()) return 0;
	typename UnderlyingNumberType<C>::type res = this->mTerms.front()->coeff();
	for (unsigned i = 0; i < this->mTerms.size(); i++) {
		///@todo gcd needed for fractions
		//res = carl::gcd(res, this->mTerms[i]->coeff());
		assert(false);
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
    if (lhs.isZero() && rhs.coeff() == C(0)) return true;
    if (lhs.nrTerms() > 1) return false;
    return *(lhs.lterm()) == rhs;
}

template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs) {
    if (lhs.nrTerms() != 1) return false;
	if (lhs.lmon() == nullptr) return false;
    return *(lhs.lmon()) == rhs;
}

template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
    if (lhs.isZero()) return rhs == C(0);
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
    return *(lhs.lmon()) == rhs;
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
	if (lhs.nrTerms() == 0) return C(0) < rhs;
	return *(lhs.lterm()) < rhs;
}
template<typename C, typename O, typename P>
bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs) {
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
	if (lhs.nrTerms() == 0) return C(0) < rhs;
	return *(lhs.lterm()) < rhs;
}
template<typename C, typename O, typename P>
bool operator<(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (rhs.nrTerms() == 0) return lhs < C(0);
	return lhs < *(rhs.lterm());
}
template<typename C, typename O, typename P>
bool operator<(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (rhs.nrTerms() == 0) return false;
	return lhs < *(rhs.lterm());
}
template<typename C, typename O, typename P>
bool operator<(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (rhs.nrTerms() == 0) return false;
	return lhs < *(rhs.lterm());
}
template<typename C, typename O, typename P>
bool operator<(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (rhs.nrTerms() == 0) return lhs < C(0);
	return lhs < *(rhs.lterm());
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const MultivariatePolynomial& rhs)
{
	///@todo Check if this works with ordering.
	this->checkConsistency();
	rhs.checkConsistency();
    if(mTerms.size() == 0) 
	{
		mTerms = rhs.mTerms;
		return *this;
	}
    if(rhs.mTerms.size() == 0) return *this;
    
    TermsType newTerms;
    newTerms.reserve(mTerms.size() + rhs.mTerms.size());
    typename TermsType::const_iterator lhsIt(mTerms.begin());
    typename TermsType::const_iterator rhsIt(rhs.mTerms.begin());
#define ADD_VERSION 3
    while(true)
    {
        CompareResult cmpres(Ordering::compare(**lhsIt, **rhsIt));
        if(cmpres == CompareResult::LESS)
        {
#if ADD_VERSION==1
            newTerms.push_back(*lhsIt);
            if(++lhsIt == mTerms.end()) break;
#elif ADD_VERSION==2
			auto next = std::lower_bound(lhsIt, mTerms.cend(), *rhsIt, &TermType::monomialLessPtr);
			newTerms.insert(newTerms.end(), lhsIt, next);
			lhsIt = next;
			if (lhsIt == mTerms.cend()) break;
#elif ADD_VERSION==3
			newTerms.push_back(*lhsIt);
			lhsIt++;
			auto next = std::lower_bound(lhsIt, mTerms.cend(), *rhsIt, &TermType::monomialLessPtr);
			newTerms.insert(newTerms.end(), lhsIt, next);
			lhsIt = next;
			if (lhsIt == mTerms.cend()) break;
#endif
        }
        else if(cmpres == CompareResult::GREATER)
        {
#if ADD_VERSION==1
            newTerms.push_back(*rhsIt);
            if(++rhsIt == rhs.mTerms.end()) break;
#elif ADD_VERSION==2
			auto next = std::lower_bound(rhsIt, rhs.mTerms.cend(), *lhsIt, &TermType::monomialLessPtr);
			newTerms.insert(newTerms.end(), rhsIt, next);
			rhsIt = next;
			if (rhsIt == rhs.mTerms.cend()) break;
#elif ADD_VERSION==3
			newTerms.push_back(*rhsIt);
			rhsIt++;
			auto next = std::lower_bound(rhsIt, rhs.mTerms.cend(), *lhsIt, &TermType::monomialLessPtr);
			newTerms.insert(newTerms.end(), rhsIt, next);
			rhsIt = next;
			if (rhsIt == rhs.mTerms.cend()) break;
#endif
        }
        else 
        {
            if( (**lhsIt).coeff() != -(**rhsIt).coeff() )
            {
				auto tmp = (**lhsIt).coeff() + (**rhsIt).coeff();
				if (tmp != Coeff(0)) {
					newTerms.push_back(std::make_shared<const Term<Coeff>>( tmp, (**lhsIt).monomial() ));
				}
            }
            ++lhsIt;
            ++rhsIt;
            if(lhsIt == mTerms.end() || rhsIt == rhs.mTerms.end()) break;
        }
    }
    newTerms.insert(newTerms.end(), lhsIt, mTerms.cend());
    newTerms.insert(newTerms.end(), rhsIt, rhs.mTerms.cend());
    
    mTerms = std::move(newTerms);
	this->checkConsistency();
    return *this;
}


template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const TermType& rhs) {
	///@todo this can be more efficient.
	return *this += std::make_shared<const TermType>(rhs);
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const std::shared_ptr<const TermType>& rhs)
{
	///@todo more efficient
    if(rhs->coeff() == (Coeff)0) return *this;
    if(Policies::searchLinear) 
    {
        typename TermsType::iterator it(mTerms.begin());
        while(it != mTerms.end())
        {
            // TODO consider comparing the shared pointers.
            CompareResult cmpres(Ordering::compare(*it, rhs));
            if( cmpres == CompareResult::GREATER ) break;
            if( cmpres == CompareResult::EQUAL )
            {
                // new coefficient would be zero, simply removing is enough.
                if((**it).coeff() == -rhs->coeff())
                {
                    mTerms.erase(it);
                }
                // we have to create a new term object.
                else
                {
                    *it = std::make_shared<const Term<Coeff>>((**it).coeff()+rhs->coeff(), (**it).monomial());
                }
				this->checkConsistency();
                return *this;
            }
            ++it;    
        }
        // no equal monomial does occur. We can simply insert.
        mTerms.insert(it, rhs);
		this->checkConsistency();
        return *this;
    }
    else 
    {
        LOG_NOTIMPLEMENTED();
    }
	this->checkConsistency();
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const Monomial& rhs)
{
	///@todo make this more efficient
    if(rhs.tdeg() == 0) return *this;
    if(Policies::searchLinear) 
    {
        typename TermsType::iterator it(mTerms.begin());
        while(it != mTerms.end())
        {
            CompareResult cmpres(Ordering::compare(*(**it).monomial(), rhs));
            if( cmpres == CompareResult::GREATER ) break;
            if( cmpres == CompareResult::EQUAL )
            {
                // new coefficient would be zero, simply removing is enough.
                if((**it).coeff() == -1)
                {
                    mTerms.erase(it);
                }
                // we have to create a new term object.
                else
                {
                    *it = std::make_shared<const Term<Coeff>>((**it).coeff()+1, (**it).monomial());
                }
				this->checkConsistency();
                return *this;
            }
            ++it;    
        }
        // no eq ual monomial does occur. We can simply insert.
        mTerms.insert(it,std::make_shared<const Term<Coeff>>(rhs));
		this->checkConsistency();
        return *this;
    }
    else 
    {
        LOG_NOTIMPLEMENTED();
    }
	this->checkConsistency();
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(Variable::Arg rhs)
{
	///@todo make this more efficient
    if(Policies::searchLinear) 
    {
        typename TermsType::iterator it(mTerms.begin());
        while(it != mTerms.end())
        {
            CompareResult cmpres(Ordering::compare(**it, rhs));
            if( cmpres == CompareResult::GREATER ) break;
            if( cmpres == CompareResult::EQUAL )
            {
                // new coefficient would be zero, simply removing is enough.
                if((**it).coeff() == -1)
                {
                    mTerms.erase(it);
                }
                // we have to create a new term object.
                else
                {
                    *it = std::make_shared<const Term<Coeff>>((**it).coeff()+1, (**it).monomial());
                }
				this->checkConsistency();
                return *this;
            }
            ++it;    
        }
        // no eq ual monomial does occur. We can simply insert.
        mTerms.insert(it,std::make_shared<const Term<Coeff>>(rhs));
    }
    else 
    {
        LOG_NOTIMPLEMENTED();
        
    }
	this->checkConsistency();
    return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const Coeff& c)
{
	///@todo more efficient
    if(c == (Coeff)0) return *this;
    if(Ordering::degreeOrder)
    {
        if(mTerms.size() > 0 && mTerms.front()->isConstant())
        {
            Coeff newConstantPart(mTerms.front()->coeff() + c);
            
            if(newConstantPart != (Coeff)0)
            {
                mTerms.front() = (std::make_shared<Term<Coeff>>(newConstantPart));
            }
            else
            {
                mTerms.erase(mTerms.begin());
            }
        }
        else
        {
            mTerms.emplace(mTerms.begin(),std::make_shared<Term<Coeff>>(c));
        }
    }
    else
    {
        LOG_NOTIMPLEMENTED();
        
    }
	this->checkConsistency();
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
    MultivariatePolynomial<Coeff, Ordering, Policies> negation;
    negation.mTerms.reserve(mTerms.size());
    for (const auto& term: mTerms) {
        negation.mTerms.push_back(std::make_shared<const Term<Coeff>>(-*term));
    }
	negation.checkConsistency();
    return negation;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator-=(const MultivariatePolynomial& rhs)
{
	///@todo Check if this works with ordering.
    if(rhs.mTerms.size() == 0) return *this;
    
    TermsType newTerms;
    newTerms.reserve(mTerms.size() + rhs.mTerms.size());
    typename TermsType::const_iterator lhsIt(mTerms.begin());
    typename TermsType::const_iterator rhsIt(rhs.mTerms.begin());
    if(mTerms.size() > 0)
    {
        while(true)
        {
            CompareResult cmpres(Ordering::compare(**lhsIt, **rhsIt));
            if(cmpres == CompareResult::LESS)
            {
                newTerms.push_back(std::make_shared<const Term<Coeff>>((**lhsIt)));
                if(++lhsIt == mTerms.end()) break;
            }
            else if(cmpres == CompareResult::GREATER)
            {
                newTerms.push_back(std::make_shared<const Term<Coeff>>(-(**rhsIt)));
                if(++rhsIt == rhs.mTerms.end()) break;
            }
            else 
            {
                assert(cmpres == CompareResult::EQUAL);
                if( (**lhsIt).coeff() != (**rhsIt).coeff() )
                {
                    newTerms.push_back(std::make_shared<const Term<Coeff>>( (**lhsIt).coeff() - (**rhsIt).coeff(), (**lhsIt).monomial() ));
                }
                ++lhsIt;
                ++rhsIt;
                if(lhsIt == mTerms.end() || rhsIt == rhs.mTerms.end() ) break;
            }
        }
        while(lhsIt != mTerms.end())
        {
            newTerms.push_back(std::make_shared<const Term<Coeff>>((**lhsIt)));
            ++lhsIt;
        }
    }
    
    while(rhsIt != rhs.mTerms.end())
    {
        newTerms.push_back(std::make_shared<const Term<Coeff>>(-(**rhsIt)));
        ++rhsIt;
    }
    
    mTerms = std::move(newTerms);
    return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator-=(const Term<Coeff>& rhs)
{
	///@todo Check if this works with ordering.
    if(rhs.coeff() == (Coeff)0) return *this;
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
					return *this;
				}
			}
            ++it;    
        }
        // no equal monomial does occur. We can simply insert.
        mTerms.insert(it,std::make_shared<const Term<Coeff>>(-rhs));
        return *this;
    }
    else 
    {
        LOG_NOTIMPLEMENTED();
    }
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator-=(const Monomial& rhs)
{
	///@todo Check if this works with ordering.
    if(rhs.tdeg() == 0) return *this;
    if(Policies::searchLinear) 
    {
        typename TermsType::iterator it(mTerms.begin());
        while(it != mTerms.end())
        {
            if ((**it).monomial() != nullptr) {
				CompareResult cmpres(Ordering::compare(*(**it).monomial(), rhs));
				if( cmpres == CompareResult::GREATER ) break;
				if( cmpres == CompareResult::EQUAL )
				{
					// new coefficient would be zero, simply removing is enough.
					if((**it).coeff() == (Coeff)1)
					{
						mTerms.erase(it);
					}
					// we have to create a new term object.
					else
					{
						*it = std::make_shared<const Term<Coeff>>((**it).coeff()-1, (**it).monomial());
					}
					return *this;
				}
			}
            ++it;    
        }
        // no equal monomial does occur. We can simply insert.
        mTerms.insert(it,std::make_shared<const Term<Coeff>>(-1, rhs));
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
	///@todo Check if this works with ordering.
    if(Policies::searchLinear) 
    {
        typename TermsType::iterator it(mTerms.begin());
        while(it != mTerms.end())
        {
			if ((**it).monomial() != nullptr) {
				CompareResult cmpres(Ordering::compare(*(**it).monomial(), rhs));
				if( cmpres == CompareResult::GREATER ) break;
				if( cmpres == CompareResult::EQUAL )
				{
					// new coefficient would be zero, simply removing is enough.
					if((**it).coeff() == (Coeff)1)
					{
						mTerms.erase(it);
					}
					// we have to create a new term object.
					else
					{
						*it = std::make_shared<const Term<Coeff>>((**it).coeff()-1, (**it).monomial());
					}
					return *this;
				}
			}
            ++it;    
        }
        // no eq ual monomial does occur. We can simply insert.
        mTerms.insert(it,std::make_shared<const Term<Coeff>>(-1, rhs));
    }
    else 
    {
        LOG_NOTIMPLEMENTED();
        
    }
    return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator-=(const Coeff& c)
{
	///@todo Check if this works with ordering.
    if(c == (Coeff)0) return *this;
    if(Ordering::degreeOrder)
    {
        if(mTerms.size() > 0 && mTerms.front()->isConstant())
        {
            Coeff newConstantPart(mTerms.front()->coeff() - c);
            
            if(newConstantPart != (Coeff)0)
            {
                mTerms.front() = (std::make_shared<Term<Coeff>>(newConstantPart));
            }
            else
            {
                mTerms.erase(mTerms.begin());
            }
        }
        else
        {
            mTerms.emplace(mTerms.begin(),std::make_shared<Term<Coeff>>(-c));
        }
    }
    else
    {
        LOG_NOTIMPLEMENTED();
        
    }
    return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator*=(const MultivariatePolynomial<Coeff,Ordering,Policies>& rhs)
{
	///@todo Check if this works with ordering.
    if( mTerms.size() == 0 || rhs.mTerms.size() == 0 )
    {
        mTerms.clear();
        return *this;
    }
//    TermsType newTerms;
//    newTerms.reserve(mTerms.size() * rhs.mTerms.size());
//    for(auto termLhs : mTerms)
//    {
//        for(auto termRhs : rhs.mTerms)
//        {
//            newTerms.push_back(std::make_shared<Term<Coeff>>(*termLhs * *termRhs));
//        }
//    }
//    setTerms(newTerms);
    MultivariatePolynomial result( Coeff( 0 ) );
    for(auto termLhs : mTerms)
    {
        result += rhs * (*termLhs);
    }
    *this = result;
    return *this;
    
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator*=(const Term<Coeff>& rhs)
{
	///@todo more efficient.
    TermsType newTerms;
    newTerms.reserve(mTerms.size());
    for(auto term : mTerms)
    {
        newTerms.push_back(std::make_shared<Term<Coeff>>(*term * rhs));
    }
    mTerms = std::move(newTerms);
    return *this;
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator*=(const Monomial& rhs)
{
	///@todo more efficient.
    TermsType newTerms;
    newTerms.reserve(mTerms.size());
    for(auto term : mTerms)
    {
        newTerms.push_back(std::make_shared<Term<Coeff>>(*term * rhs));
    }
    mTerms = std::move(newTerms);
    return *this;
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator*=(Variable::Arg rhs)
{
	///@todo more efficient.
    TermsType newTerms;
    newTerms.reserve(mTerms.size());
    for(auto term : mTerms)
    {
        newTerms.push_back(std::make_shared<Term<Coeff>>(*term * rhs));
    }
    mTerms = std::move(newTerms);
    return *this;
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator*=(const Coeff& c)
{
	///@todo more efficient.
    if(c == (Coeff)1) return *this;
    if(c == (Coeff)0) 
    {
        mTerms.clear();
        return *this;
    }
    TermsType newTerms;
    newTerms.reserve(mTerms.size());
    for(auto term : mTerms)
    {
        newTerms.push_back(std::make_shared<Term<Coeff>>(*term * c));
    }
    mTerms = std::move(newTerms);
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
	///@todo more efficient.
	assert(c != (Coeff)0);
    if(c == 1) return *this;
	TermsType newTerms;
    newTerms.reserve(mTerms.size());
    for(auto term : mTerms) {
        newTerms.push_back(std::shared_ptr<Term<Coeff>>(term->divideBy(c)));
    }
    mTerms = std::move(newTerms);
    return *this;
}

template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator/(const MultivariatePolynomial<C,O,P>& lhs, unsigned long rhs)
{
	///@todo more efficient.
    MultivariatePolynomial<C,O,P> result(lhs);
	for(auto& t : result.mTerms)
	{
		t = std::shared_ptr<const Term<C>>(new Term<C>(*t/rhs));
	}
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
void MultivariatePolynomial<Coeff, Ordering, Policies>::makeMinimallyOrdered() const {
	if (mTerms.size() < 2) return;
	auto it = mTerms.begin();
	auto lTerm = it;
	auto constTerm = mTerms.end();
	if (it->isConstant()) constTerm = it;

	for (it++; it != mTerms.end();) {
		if (it->isConstant()) {
			assert(constTerm == mTerms.end());
			constTerm = it;
		} else if (Term<Coeff>::monomialLess(*lTerm, **it)) {
			lTerm = it;
		}
		assert(!Term<Coeff>::monomialEqual(*lTerm, **it));
		it++;
	}
	if (constTerm != mTerms.end()) {
		// Prevent that the swaps cancel each other.
		if (lTerm == mTerms.begin()) lTerm = constTerm;
		std::swap(*constTerm, mTerms.begin());
	}
	std::swap(*lTerm, mTerms.rbegin());
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
        return;
    }
    else if(newTerms.size() == 1)
    {
        mTerms.push_back(newTerms.back());
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
			if (frontCoeff != (Coeff)0) {
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
		assert(frontCoeff != (Coeff)0);
        mTerms.push_back(frontTerm);
    }
    else if (frontCoeff != (Coeff)0)
    {
        mTerms.emplace_back(std::make_shared<const Term<Coeff>>(frontCoeff, frontTerm->monomial()));
    }
//    assert( newTerms.empty() );
}

template<typename Coeff, typename Ordering, typename Policies>
void MultivariatePolynomial<Coeff, Ordering, Policies>::checkConsistency() const {
	for (unsigned i = 0; i < this->mTerms.size(); i++) {
		assert(this->mTerms[i]);
		assert(!this->mTerms[i]->isZero());
		if (i > 0) {
			assert(this->mTerms[i]->tdeg() > 0);
		}
	}
}

}
