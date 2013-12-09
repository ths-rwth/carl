#pragma once
#include "MultivariatePolynomial.h"
#include <memory>
#include <list>

#include "UnivariatePolynomial.h"
#include "logging.h"
#include "carl/numbers/numbers.h"

namespace carl
{

	
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(int c) : MultivariatePolynomial((Coeff)c)
{
    
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const Coeff& c) :
Policies(),
mTerms(c == 0 ? 0 : 1,std::make_shared<const Term<Coeff>>(c))
{
    
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(Variable::Arg v) :
Policies(),
mTerms(1,std::make_shared<const Term<Coeff>>(v))
{
    
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const Monomial& m) :
Policies(),
mTerms(1,std::make_shared<const Term<Coeff>>(m))
{
    
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const Term<Coeff>& t) :
Policies(),
mTerms(1,std::make_shared<const Term<Coeff>>(t))
{
    
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(std::shared_ptr<const Monomial> m) :
Policies(),
mTerms(1,std::make_shared<const Term<Coeff>>(m))
{

}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(std::shared_ptr<const Term<Coeff>> t) :
Policies(),
mTerms(1,t)
{
    
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const UnivariatePolynomial<MultivariatePolynomial<Coeff, Ordering, Policies>>&) :
Policies()
{
    LOG_NOTIMPLEMENTED();
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const UnivariatePolynomial<Coeff>&) :
Policies()
{
    LOG_NOTIMPLEMENTED();
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename OtherPolicies>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(const MultivariatePolynomial<Coeff, Ordering, OtherPolicies>& pol) :
Policies(),
mTerms(pol.begin(), pol.end())
{
	
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename InputIterator>
MultivariatePolynomial<Coeff,Ordering,Policies>::MultivariatePolynomial(InputIterator begin, InputIterator end, bool duplicates, bool sorted) :
Policies()
{
	mTerms.assign(begin, end);
	if(!sorted)
	{
		sortTerms();
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
				if(Term<Coeff>::EqualMonomial(**jt, **it)) 
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
	
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(const std::initializer_list<Term<Coeff>>& terms)
{
	for(Term<Coeff> term : terms)
	{
		mTerms.push_back(std::make_shared<const Term<Coeff>>(term));
	}
	sortTerms();
}


template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>::MultivariatePolynomial(const std::initializer_list<Variable>& terms)
{
	for(Variable term : terms)
	{
		mTerms.push_back(std::make_shared<const Term<Coeff>>(term));
	}
	sortTerms();
}
    
template<typename Coeff, typename Ordering, typename Policies>
std::shared_ptr<const Monomial> MultivariatePolynomial<Coeff,Ordering,Policies>::lmon() const
{
    return lterm()->monomial();
}
template<typename Coeff, typename Ordering, typename Policies>
std::shared_ptr<const Term<Coeff>> MultivariatePolynomial<Coeff,Ordering,Policies>::lterm() const
{
    LOG_ASSERT(!isZero(), "Leading term undefined on zero polynomials.");
	return mTerms.back();
}

template<typename Coeff, typename Ordering, typename Policies>
std::shared_ptr<const Term<Coeff>> MultivariatePolynomial<Coeff,Ordering,Policies>::trailingTerm() const
{
    LOG_ASSERT(!isZero(), "Trailing term undefined on zero polynomials.");
	return mTerms.front();
}

template<typename Coeff, typename Ordering, typename Policies>
Coeff MultivariatePolynomial<Coeff,Ordering,Policies>::lcoeff() const
{
    return lterm()->coeff();
}

template<typename Coeff, typename Ordering, typename Policies>
exponent MultivariatePolynomial<Coeff,Ordering,Policies>::highestDegree() const
{
    if(mTerms.size() == 0) return 0;
    if(Ordering::degreeOrder)
    {
        return mTerms.back()->tdeg();
    }
    else 
    {
        LOG_NOTIMPLEMENTED();
    }
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isZero() const
{
    return mTerms.empty();
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isConstant() const
{
    return (mTerms.size() == 0) || (mTerms.size() == 1 && mTerms.front()->isConstant());
}

template<typename Coeff, typename Ordering, typename Policies>
Coeff MultivariatePolynomial<Coeff, Ordering, Policies>::constantPart() const
{
	if(isZero()) return 0;
	if(trailingTerm()->isConstant())
	{
		return trailingTerm()->coeff();
	}
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isLinear() const
{
    if(mTerms.size() == 0) return true;
    if(Ordering::degreeOrder)
    {
        return mTerms.back()->isLinear();
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
	else
	{
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
    assert(!isZero());
    mTerms.pop_back();
    return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isUnivariate() const
{
	// A constant polynomial is obviously univariate.
	if(isConstant()) return true;
	
	Variable v = Variable::NO_VARIABLE;
	// If the leading term is nonlinear, than there are several variables involved.
	if(lterm()->isLinear())
	{
		//As the monomials are ordered, a leading term is non-constant in a non-constant polynomial,
		//and thus, we can be sure that the variable actually exists.
		v = lterm()->getSingleVariable();
	}
	else
	{
		return false;
	}
	
	for(const std::shared_ptr<const Term<Coeff>>& term : mTerms)
	{
		if(!term->hasNoOtherVariable(v)) return false;
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
    for(const std::shared_ptr<const TermType>& term : mTerms)
        if( term->has(v) ) return true;
    return false;
}

template<typename Coeff, typename Ordering, typename Policies>
bool MultivariatePolynomial<Coeff,Ordering,Policies>::isReducibleIdentity() const
{
    LOG_NOTIMPLEMENTED();
    return false;
}

template<typename Coeff, typename Ordering, typename Policies>
template<typename SubstitutionType>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::substitute(const std::map<Variable,SubstitutionType>& substitutions) const
{
	MultivariatePolynomial result;
	for(auto term : mTerms)
	{
		Term<Coeff>* t = term->substitute(substitutions);
		if(t->coeff() != 0)
		{
			result.mTerms.push_back(std::shared_ptr<const Term<Coeff>>(t));
		}
	}
	result.sortTerms();
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
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::coprimeCoefficients() const
{
	assert(nrTerms() != 0);
	if(nrTerms() == 1) return *this;
	typename TermsType::const_iterator it = mTerms.begin();
	typename IntegralT<Coeff>::type num = getNum((*it)->coeff());
	typename IntegralT<Coeff>::type den = getDenom((*it)->coeff());
	for(++it; it != mTerms.end(); ++it)
	{
		num = carl::gcd(num, getNum((*it)->coeff()));
		den = carl::lcm(den, getDenom((*it)->coeff()));
	}
	Coeff factor = den/num;
	// Notice that even if factor is 1, we create a new polynomial
	MultivariatePolynomial<Coeff, Ordering, Policies> result;
	result.mTerms.reserve(mTerms.size());
	for(const typename std::shared_ptr<const TermType> term : mTerms)
	{
		result.mTerms.push_back(std::make_shared<const TermType>(*term * factor));
	}
	return result;
	
	
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::normalize() const
{
	MultivariatePolynomial result;
	result.mTerms.reserve(mTerms.size());
	for(typename TermsType::const_iterator it = mTerms.begin(); it != mTerms.end(); ++it)
	{
		result.mTerms.emplace_back((*it)->dividedBy(lcoeff()));
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
	for(std::shared_ptr<const Term<Coeff>> t : mTerms)
	{
		tmpTerms.emplace_back(t->derivative(v));
	}
	return MultivariatePolynomial(tmpTerms.begin(), tmpTerms.end(), true, false);
}


template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::SPolynomial(
																const MultivariatePolynomial<Coeff, Ordering, Policies>& p,
																const MultivariatePolynomial<Coeff, Ordering, Policies>& q)
{
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
	for(std::shared_ptr<const Term<Coeff>> t : mTerms)
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
unsigned MultivariatePolynomial<Coeff,Ordering,Policies>::hash() const
{
    // TODO: make this compilable 
    return mTerms.size() << 16 || (std::hash<Coeff>()(lcoeff()) ^ lterm()->getNrVariables());
}

template<typename Coeff, typename Ordering, typename Policies>
template<bool gatherCoeff>
VariableInformation<gatherCoeff, MultivariatePolynomial<Coeff,Ordering,Policies>> MultivariatePolynomial<Coeff,Ordering,Policies>::getVarInfo(const Variable& var) const
{
	VariableInformation<gatherCoeff, MultivariatePolynomial> varinfomap;
	// We iterate over all terms.
	for(std::shared_ptr<const TermType> term : mTerms)
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
	for(std::shared_ptr<const TermType> term : mTerms)
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
	// For constant polynomials, we would have to use some random variable.
	// We could introduce a fixed one, but I am unsure about the consequences.
	// TODO we could implement a method isUnivariate()
	
	Variable::Arg x = lmon()->getSingleVariable();
	std::vector<C> coeffs(highestDegree()+1,0);
	// TODO we do not use the fact that it is already sorted..
	for(std::shared_ptr<const Term<C>> t : mTerms)
	{
		coeffs[t->tdeg()] = t->coeff();
	}
	return UnivariatePolynomial<C>(x, coeffs);
}

template<typename C, typename O, typename P>
UnivariatePolynomial<MultivariatePolynomial<C,O,P>> MultivariatePolynomial<C,O,P>::toUnivariatePolynomial(Variable::Arg mainVar) const
{
	return UnivariatePolynomial<MultivariatePolynomial<C,O,P>>(mainVar, getVarInfo<true>(mainVar));
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> MultivariatePolynomial<C,O,P>::mod(const typename IntegralT<C>::type& modulo) const
{
	assert(modulo != 0);
	MultivariatePolynomial<C,O,P> result;
	result.mTerms.reserve(mTerms.size());
	for(std::shared_ptr<const Term<C>> t : mTerms)
	{
		C m = carl::mod(t->coeff(), modulo);
		if(m != 0)
		{
			result.mTerms.emplace_back(new Term<C>(m, t->monomial()));
		}
	}
}

template<typename C, typename O, typename P>
DivisionResult<MultivariatePolynomial<C,O,P>> MultivariatePolynomial<C,O,P>::divideBy(const MultivariatePolynomial& divisor) const
{
	DivisionResult<MultivariatePolynomial<C,O,P>> res;
	MultivariatePolynomial<C,O,P> p = *this;
	while(!p.isZero())
	{
		Term<C>* factor = p.lterm()->dividedBy(*divisor.lterm());
		if(factor != nullptr)
		{
			
			res.quotient += *factor;
			p -= *factor * divisor;
		}
		else
		{
			res.remainder += *(p.lterm());
			p.stripLT();
		}
		delete factor;
	}
	return res;
}

/**
 * @param dividant 
 * @return 
 */
template<typename C, typename O, typename P>
bool MultivariatePolynomial<C,O,P>::divides(const MultivariatePolynomial<C,O,P>& dividant) const
{
	return !dividant.divideBy(*this).remainder.isZero();
}

template<typename C, typename O, typename P>
bool operator==( const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    if(lhs.mTerms.size() != rhs.mTerms.size()) return false;
    // Compare vector entries. We cannot use std::vector== as we not only want to compare the pointers.
    return std::equal(lhs.mTerms.begin(), lhs.mTerms.end(), rhs.mTerms.begin(),
                    [](const std::shared_ptr<const Term<C>>& lterm, const std::shared_ptr<const Term<C>>& rterm) 
                    -> bool 
                    {
                        return lterm == rterm || *lterm == *rterm;
                    });
}
template<typename C, typename O, typename P>
bool operator==(const UnivariatePolynomial<C>&, const MultivariatePolynomial<C,O,P>&)
{
    LOG_NOTIMPLEMENTED();
    return false;
}
template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs)
{
    return rhs == lhs;
}

template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs)
{
    if(lhs.mTerms.empty() && rhs.coeff() == 0) return true;
    if(lhs.mTerms.size() > 1) return false;
    return *(lhs.mTerms.front()) == rhs;
}
template<typename C, typename O, typename P>
bool operator==(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return rhs == lhs;
}
template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs)
{
    if(lhs.mTerms.size() != 1) return false;
    return *(lhs.mTerms.front()->monomial) == rhs;
}
template<typename C, typename O, typename P>
bool operator==(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return rhs == lhs;
}
template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs)
{
    if(lhs.mTerms.empty() && rhs == 0) return true;
    if(lhs.mTerms.size() > 1) return false;
    return (lhs.mTerms.front()->coeff()) == rhs;
}
template<typename C, typename O, typename P>
bool operator==(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return rhs == lhs;
}
template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs)
{
    if(lhs.mTerms.size() != 1) return false;
    return *(lhs.mTerms.front()->monomial()) == rhs;
}
template<typename C, typename O, typename P>
bool operator==(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return rhs == lhs;
}

template<typename C, typename O, typename P>
bool operator==(const MultivariatePolynomial<C,O,P>& lhs, int rhs)
{
    if(lhs.mTerms.empty() && rhs == 0) return true;
    if(lhs.mTerms.size() > 1) return false;
    return (lhs.mTerms.front()->coeff()) == rhs;
}

template<typename C, typename O, typename P>
bool operator==(int lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
	return rhs == lhs;
}


template<typename C, typename O, typename P>
bool operator!=( const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return !(lhs == rhs); // TODO: != could be much cheaper than ==
}
template<typename C, typename O, typename P>
bool operator!=(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return !(lhs == rhs);
}
template<typename C, typename O, typename P>
bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs)
{
    return !(lhs == rhs);
}
template<typename C, typename O, typename P>
bool operator!=(const UnivariatePolynomial<MultivariatePolynomial<C>>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return !(lhs == rhs);
}
template<typename C, typename O, typename P>
bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C>>& rhs)
{
    return !(lhs == rhs);
}
template<typename C, typename O, typename P>
bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs)
{
    return !(lhs == rhs);
}
template<typename C, typename O, typename P>
bool operator!=(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return !(lhs == rhs);
}
template<typename C, typename O, typename P>
bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs)
{
    return !(lhs == rhs);
}
template<typename C, typename O, typename P>
bool operator!=(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return !(lhs == rhs);
}
template<typename C, typename O, typename P>
bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs)
{
    return !(lhs == rhs);
}
template<typename C, typename O, typename P>
bool operator!=(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return !(lhs == rhs);
}
template<typename C, typename O, typename P>
bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs)
{
    return !(lhs == rhs);
}
template<typename C, typename O, typename P>
bool operator!=(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return !(lhs == rhs);
}

template<typename C, typename O, typename P>
bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, int rhs)
{
    return !(lhs == rhs);
}
template<typename C, typename O, typename P>
bool operator!=(int lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return !(lhs == rhs);
}


//template<typename C, typename O, typename P>
//bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
//{
//    // Compare vector entries. We cannot use std::vector== as we not only want to compare the pointers.
//    return std::equal(lhs.mTerms.begin(), lhs.mTerms.end(), rhs.mTerms.begin(),
//                    [](const std::shared_ptr<const Term<C>>& lterm, const std::shared_ptr<const Term<C>>& rterm) 
//                    -> bool 
//                    {
//                        return lterm != rterm && *lterm < *rterm;
//                    });
//}
//
//template<typename C, typename O, typename P>
//bool operator>(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
//{
//    return rhs<lhs;
//}
//
//template<typename C, typename O, typename P>
//bool operator<=(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
//{
//    return !(rhs<lhs);
//}
//
//template<typename C, typename O, typename P>
//bool operator>=(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
//{
//    return !(rhs>lhs);
//}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const MultivariatePolynomial& rhs)
{
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
    while(true)
    {
        CompareResult cmpres(Ordering::compare(**lhsIt, **rhsIt));
        if(cmpres == CompareResult::LESS)
        {
            newTerms.push_back(*lhsIt);
            if(++lhsIt == mTerms.end()) break;
        }
        else if(cmpres == CompareResult::GREATER)
        {
            newTerms.push_back(*rhsIt);
            if(++rhsIt == rhs.mTerms.end()) break;
        }
        else 
        {
            if( (**lhsIt).coeff() != -(**rhsIt).coeff() )
            {
                newTerms.push_back(std::make_shared<const Term<Coeff>>( (**lhsIt).coeff() + (**rhsIt).coeff(), (**lhsIt).monomial() ));
            }
            ++lhsIt;
            ++rhsIt;
            if(lhsIt == mTerms.end() || rhsIt == rhs.mTerms.end()) break;
        }
    }
    newTerms.insert(newTerms.end(), lhsIt, mTerms.cend());
    newTerms.insert(newTerms.end(), rhsIt, rhs.mTerms.cend());
    
    mTerms = std::move(newTerms);
    return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const Term<Coeff>& rhs)
{
    if(rhs.coeff() == 0) return *this;
    if(Policies::searchLinear) 
    {
        typename TermsType::iterator it(mTerms.begin());
        while(it != mTerms.end())
        {
            // TODO consider comparing the shared pointers.
            CompareResult cmpres(Ordering::compare((**it), rhs));
            if( cmpres == CompareResult::GREATER ) break;
            if( cmpres == CompareResult::EQUAL )
            {
                // new coefficient would be zero, simply removing is enough.
                if((**it).coeff() == -rhs.coeff())
                {
                    mTerms.erase(it);
                }
                // we have to create a new term object. 
                else
                {
                    *it = std::make_shared<const Term<Coeff>>((**it).coeff()+rhs.coeff(), (**it).monomial());
                }
                return *this;
            }
            ++it;    
        }
        // no equal monomial does occur. We can simply insert.
        mTerms.insert(it,std::make_shared<const Term<Coeff>>(rhs));
        return *this;
    }
    else 
    {
        LOG_NOTIMPLEMENTED();
    }
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const Monomial& rhs)
{
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
                return *this;
            }
            ++it;    
        }
        // no eq ual monomial does occur. We can simply insert.
        mTerms.insert(it,std::make_shared<const Term<Coeff>>(rhs));
        return *this;
    }
    else 
    {
        LOG_NOTIMPLEMENTED();
    }
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const Variable::Arg rhs)
{
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
    return *this;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator+=(const Coeff& c)
{
    if(c == 0) return *this;
    if(Ordering::degreeOrder)
    {
        if(mTerms.size() > 0 && mTerms.front()->isConstant())
        {
            Coeff newConstantPart(mTerms.front()->coeff() + c);
            
            if(newConstantPart != 0)
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
    return *this;
}

template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator+( const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    // TODO write dedicated add
    LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> result(lhs);
	result += rhs;
	return result;
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
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs)
{
    // TODO write dedicated add
    LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> result(lhs);
	result += rhs;
	return result;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator+(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return rhs + lhs;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs)
{
    // TODO write dedicated add
    LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> result(lhs);
	result += rhs;
	return result;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator+(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return rhs + lhs;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs)
{
    // TODO write dedicated add
    LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> result(lhs);
	result += rhs;
	return result;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator+(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return rhs + lhs;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs)
{
	// TODO write dedicated add
    LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> result(lhs);
	result += rhs;
	return result;
	
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator+(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return rhs + lhs;
}



template<typename Coeff, typename Ordering, typename Policies>
const MultivariatePolynomial<Coeff, Ordering, Policies> MultivariatePolynomial<Coeff,Ordering,Policies>::operator -() const
{
    MultivariatePolynomial<Coeff, Ordering, Policies> negation;
    negation.mTerms.reserve(mTerms.size());
    for(auto term : mTerms)
    {
        negation.mTerms.push_back(std::make_shared<const Term<Coeff>>(-*term));
    }
    return negation;
}


template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator-=(const MultivariatePolynomial& rhs)
{
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
    if(rhs.coeff() == 0) return *this;
    if(Policies::searchLinear) 
    {
        typename TermsType::iterator it(mTerms.begin());
        while(it != mTerms.end())
        {
            // TODO consider comparing the shared pointers.
            if( (**it).isConstant() ) break;
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
    if(rhs.tdeg() == 0) return *this;
    if(Policies::searchLinear) 
    {
        typename TermsType::iterator it(mTerms.begin());
        while(it != mTerms.end())
        {
            if( (**it).isConstant() ) break;
            CompareResult cmpres(Ordering::compare(*(**it).monomial(), rhs));
            if( cmpres == CompareResult::GREATER ) break;
            if( cmpres == CompareResult::EQUAL )
            {
                // new coefficient would be zero, simply removing is enough.
                if((**it).coeff() == 1)
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
            ++it;    
        }
        // no eq ual monomial does occur. We can simply insert.
        mTerms.insert(it,std::make_shared<const Term<Coeff>>(-1, rhs));
        return *this;
    }
    else 
    {
        LOG_NOTIMPLEMENTED();
    }
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff, Ordering, Policies>& MultivariatePolynomial<Coeff, Ordering, Policies>::operator-=(const Variable::Arg rhs)
{
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
                if((**it).coeff() == 1)
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
    if(c == 0) return *this;
    if(Ordering::degreeOrder)
    {
        if(mTerms.size() > 0 && mTerms.front()->isConstant())
        {
            Coeff newConstantPart(mTerms.front()->coeff() - c);
            
            if(newConstantPart != 0)
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

template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator-( const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    // TODO write dedicated add
    LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> result(lhs);
	result -= rhs;
	return result;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator-(const UnivariatePolynomial<C>&, const MultivariatePolynomial<C,O,P>&)
{
    LOG_NOTIMPLEMENTED();
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs - lhs;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator-(const UnivariatePolynomial<MultivariatePolynomial<C>>&, const MultivariatePolynomial<C,O,P>&)
{
    LOG_NOTIMPLEMENTED();
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C>>& rhs)
{
	return rhs - lhs;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs)
{
    // TODO write dedicated add
    LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> result(lhs);
	result -= rhs;
	return result;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator-(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return rhs - lhs;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs)
{
    // TODO write dedicated add
    LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> result(lhs);
	result -= rhs;
	return result;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator-(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return rhs - lhs;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs)
{
    // TODO write dedicated add
    LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> result(lhs);
	result -= rhs;
	return result;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator-(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
	return rhs - lhs;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs)
{
    // TODO write dedicated add
    LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> result(lhs);
	result -= rhs;
	return result;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator-(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return rhs - lhs;
}

template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator*=(const MultivariatePolynomial<Coeff,Ordering,Policies>& rhs)
{
    if( mTerms.size() == 0 || rhs.mTerms.size() == 0 )
    {
        mTerms.clear();
        return *this;
    }
    TermsType newTerms;
    newTerms.reserve(mTerms.size() * rhs.mTerms.size());
    for(auto termLhs : mTerms)
    {
        for(auto termRhs : rhs.mTerms)
        {
            newTerms.push_back(std::make_shared<Term<Coeff>>(*termLhs * *termRhs));
        }
    }
    mTerms.clear();
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
            if(frontCoeff == frontTerm->coeff())
            {
                assert(frontCoeff!=0);
                mTerms.push_back(frontTerm);
            }
            else if(frontCoeff != 0)
            {
                mTerms.emplace_back(std::make_shared<const Term<Coeff>>(frontCoeff, frontTerm->monomial()));
            }
            frontTerm = *it;
            frontCoeff = (*it)->coeff();
        }
    }
    
    if(frontCoeff == frontTerm->coeff())
    {
        mTerms.push_back(frontTerm);
    }
    else
    {
        mTerms.emplace_back(std::make_shared<const Term<Coeff>>(frontCoeff, frontTerm->monomial()));
    }
    
    return *this;
    
}
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator*=(const Term<Coeff>& rhs)
{
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
MultivariatePolynomial<Coeff,Ordering,Policies>& MultivariatePolynomial<Coeff,Ordering,Policies>::operator*=(const Variable::Arg rhs)
{
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
    if(c == 1) return *this;
    if(c == 0) 
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
const MultivariatePolynomial<C,O,P> operator*( const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    // TODO write dedicated mult
    LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> result(lhs);
	result *= rhs;
	return result;
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
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs)
{
    // TODO write dedicated mult
    LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> result(lhs);
	result *= rhs;
	return result;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator*(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return rhs * lhs;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs)
{
    // TODO write dedicated mult
    LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> result(lhs);
	result *= rhs;
	return result;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator*(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return rhs * lhs;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs)
{
    // TODO write dedicated mult
    LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> result(lhs);
	result *= rhs;
	return result;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator*(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
	return rhs * lhs;
}


template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs)
{
    // TODO write dedicated mult
    LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> result(lhs);
	result *= rhs;
	return result;
}
template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator*(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
    return rhs * lhs;
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P>& MultivariatePolynomial<C,O,P>::operator/=(const MultivariatePolynomial& rhs)
{
	assert(!rhs.isZero());
	*this = divideBy(rhs).quotient;
	return *this;
}

template<typename C, typename O, typename P>
const MultivariatePolynomial<C,O,P> operator/(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs)
{
	MultivariatePolynomial<C,O,P> res(lhs);
	res /= rhs;
	return res;
}

template<typename C, typename O, typename P>
std::ostream& operator<<( std::ostream& os, const MultivariatePolynomial<C,O,P>& rhs )
{
    return (os << rhs.toString(true, true));
}

template<typename Coeff, typename Ordering, typename Policies>
std::string MultivariatePolynomial<Coeff, Ordering, Policies>::toString(bool infix, bool friendlyVarNames) const
{
    if(mTerms.size() == 0) return "0";
    auto term = mTerms.rbegin();
    if(mTerms.size() == 1) return (*term)->toString(infix, friendlyVarNames);
    std::string result = "";
    if( !infix ) result += "(+";
    for( ; term != mTerms.rend(); ++term)
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
void MultivariatePolynomial<Coeff, Ordering, Policies>::sortTerms()
{
    std::sort(mTerms.begin(), mTerms.end(), (bool (*)(const std::shared_ptr<const Term<Coeff>>&, const std::shared_ptr<const Term<Coeff>>& ))Ordering::less);
}

}
