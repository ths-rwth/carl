/**
 * @file Term.tpp
 * @ingroup MultiRP
 * @author Sebastian Junges
 */
#pragma once

// for code assistance
#include "Term.h"
#include "Monomial_derivative.h"
#include "Monomial_substitute.h"

namespace carl
{

template<typename Coefficient>
Term<Coefficient>::Term() :
    mCoeff((Coefficient)0), mMonomial()
{
    assert(this->isConsistent());
}

template<typename Coefficient>
Term<Coefficient>::Term(const Coefficient& c) :
    mCoeff(c), mMonomial()
{
    assert(this->isConsistent());
}
template<typename Coefficient>
Term<Coefficient>::Term(Variable::Arg v) :
    mCoeff(1), mMonomial(new Monomial(v))
{
    assert(this->isConsistent());
}

template<typename Coefficient>
Term<Coefficient>::Term(const Monomial& m) :
    mCoeff(1), mMonomial(new Monomial(m))
{
    assert(this->isConsistent());
}

template<typename Coefficient>
Term<Coefficient>::Term(const std::shared_ptr<const Monomial>& m) :
    mCoeff(1), mMonomial(m)
{
    assert(this->isConsistent());
}

template<typename Coefficient>
Term<Coefficient>::Term(const Coefficient& c, const Monomial* m) :
    mCoeff(c), mMonomial(std::shared_ptr<const Monomial>(m))
{
    assert(this->isConsistent());
}

template<typename Coefficient>
Term<Coefficient>::Term(const Coefficient& c, const Monomial& m)
: mCoeff(c)
{
    if (c != Coefficient(0)) mMonomial = std::make_shared<const Monomial>(m);
	assert(this->isConsistent());
}

template<typename Coefficient>
Term<Coefficient>::Term(const Coefficient& c, const std::shared_ptr<const Monomial>& m)
: mCoeff(c)
{
    if(c != Coefficient(0)) mMonomial = m;
	assert(this->isConsistent());
}

template<typename Coefficient>
Term<Coefficient>::Term(const Coefficient& c, Variable::Arg v, unsigned e)
: mCoeff(c), mMonomial(std::make_shared<Monomial>(Monomial(v, e)))
{
    assert(this->isConsistent());
}

template<typename Coefficient>
Term<Coefficient>* Term<Coefficient>::divideBy(const Coefficient& c) const
{
    assert(c != Coefficient(0));
    return new Term(mCoeff / c, mMonomial);
}

template<typename Coefficient>
Term<Coefficient>* Term<Coefficient>::divideBy(Variable::Arg v) const
{
    if(mMonomial)
    {
        Monomial* div = mMonomial->divide(v);
        if(div != nullptr)
        {
			if (div->tdeg() == 0) {
				delete div;
				return new Term<Coefficient>(mCoeff);
			}
            return new Term<Coefficient>(mCoeff, div);
        }   
    }
    return nullptr;    
}

template<typename Coefficient>
Term<Coefficient>* Term<Coefficient>::divideBy(const Monomial& m) const
{
    if(mMonomial)
    {
        Monomial* div = mMonomial->divide(m);
        if(div != nullptr)
        {
			if (div->tdeg() == 0) {
				delete div;
				return new Term<Coefficient>(mCoeff);
			}
            return new Term<Coefficient>(mCoeff, div);
        }   
    }
    return nullptr;  
}

template<typename Coefficient>
Term<Coefficient>* Term<Coefficient>::divideBy(const Term& t) const
{
    assert(t.mCoeff != 0);
    if(mMonomial)
    {
        if(!t.mMonomial)
        {
            // Term is just a constant.
            return new Term<Coefficient>(mCoeff / t.mCoeff, mMonomial);
        }
        Monomial* div = mMonomial->divide(*(t.mMonomial));
        if(div != nullptr)
        {
			if (div->tdeg() == 0) {
				delete div;
				return new Term<Coefficient>(mCoeff / t.mCoeff);
			}
            return new Term<Coefficient>(mCoeff / t.mCoeff, div);
        }   
    } 
    else if(!t.mMonomial)
    {
       // Division of constants.
        return new Term<Coefficient>(mCoeff / t.mCoeff);
    }
    return nullptr;  
}

template<typename Coefficient>
Term<Coefficient>* Term<Coefficient>::derivative(Variable::Arg v) const
{
	if(!mMonomial)
	{
		// Derivatives of constants are zero.
		return new Term<Coefficient>((Coefficient)0);
	}
    Term<Coefficient>* t = mMonomial->derivative<Coefficient>(v);
    *t *= mCoeff;
    return t;
}

template<typename Coefficient>
Definiteness Term<Coefficient>::definiteness() const
{
    if(mMonomial)
    {
        if(mMonomial->isSquare())
            return (mCoeff < (Coefficient)0 ? Definiteness::NEGATIVE_SEMI : Definiteness::POSITIVE_SEMI);
    }
    else if(mCoeff != (Coefficient)0)
        return (mCoeff < (Coefficient)0 ? Definiteness::NEGATIVE : Definiteness::POSITIVE);
    return Definiteness::NON;
}

template<typename Coefficient>
template<typename SubstitutionType>
Term<Coefficient>* Term<Coefficient>::substitute(const std::map<Variable,SubstitutionType>& substitutions) const
{
    if(mMonomial)
    {
        return mMonomial->substitute<Coefficient>(substitutions, coeff());
    }
    else
    {
        return new Term<Coefficient>( mCoeff );
    }
}

template<typename Coefficient>
Term<Coefficient>* Term<Coefficient>::substitute(const std::map<Variable, Term<Coefficient>>& substitutions) const
{
    if(mMonomial)
	{
		return mMonomial->substitute<Coefficient>(substitutions, coeff());
	}
	else
	{
		return new Term<Coefficient>(mCoeff);
	}
}


template<typename Coefficient>
Term<Coefficient> Term<Coefficient>::calcLcmAndDivideBy(const Monomial& m) const
{
	Monomial* tmp = monomial()->calcLcmAndDivideBy(m);
	if(tmp->tdeg() == 0)
	{
		delete tmp;
		return Term(1);
	}
	else
	{
		return Term(1, tmp);
	}	
	

}

template<typename Coefficient>
template<bool gatherCoeff, typename CoeffType>
void Term<Coefficient>::gatherVarInfo(Variable::Arg var, VariableInformation<gatherCoeff, CoeffType>& varinfo) const
{
	if(mMonomial)
	{
		varinfo.collect(var, coeff(), *mMonomial);
	}
    else
    {
        varinfo.updateCoeff( 0, *this );
    }
}

template<typename Coefficient>
template<bool gatherCoeff, typename CoeffType>
void Term<Coefficient>::gatherVarInfo(VariablesInformation<gatherCoeff, CoeffType>& varinfo) const
{
	if(mMonomial)
	{
		mMonomial->gatherVarInfo(varinfo, coeff());
	}
	else
	{
		// No change in varinfo.
	}
}


template<typename Coeff>
bool operator==(const Term<Coeff>& lhs, const Term<Coeff>& rhs)
{
    return (lhs.mCoeff == rhs.mCoeff) && // same coefficients
            Term<Coeff>::EqualMonomial(lhs, rhs); // same monomials
}
template<typename Coeff>
bool operator==(const Term<Coeff>& lhs, const Coeff& rhs)
{
    return !lhs.mMonomial && lhs.mCoeff == rhs;
}
template<typename Coeff>
bool operator==(const Coeff& lhs, const Term<Coeff>& rhs)
{
    return rhs == lhs;
}
template<typename Coeff>
bool operator==(const Term<Coeff>& lhs, Variable::Arg rhs)
{
    return lhs.mMonomial && *(lhs.mMonomial) == rhs;
}
template<typename Coeff>
bool operator==(Variable::Arg lhs, const Term<Coeff>& rhs)
{
    return rhs == lhs;
}
template<typename Coeff>
bool operator==(const Term<Coeff>& lhs, const Monomial& rhs)
{
    return lhs.mMonomial && *(lhs.mMonomial) == rhs;
}
template<typename Coeff>
bool operator==(const Monomial& lhs, const Term<Coeff>& rhs)
{
    return (rhs == lhs);
}
template<typename Coeff>
bool operator!=(const Term<Coeff>& lhs, const Term<Coeff>& rhs)
{
    return !(lhs == rhs);
}
template<typename Coeff>
bool operator!=(const Term<Coeff>& lhs, const Coeff& rhs)
{
    return !(lhs == rhs);
}
template<typename Coeff>
bool operator!=(const Coeff& lhs, const Term<Coeff>& rhs)
{
    return !(lhs == rhs);
}
template<typename Coeff>
bool operator!=(const Term<Coeff>& lhs, Variable::Arg rhs)
{
    return !(lhs == rhs);
}
template<typename Coeff>
bool operator!=(Variable::Arg lhs, const Term<Coeff>& rhs)
{
    return !(lhs == rhs);
}
template<typename Coeff>
bool operator!=(const Term<Coeff>& lhs, const Monomial& rhs)
{
    return !(lhs == rhs);
}
template<typename Coeff>
bool operator!=(const Monomial& lhs, const Term<Coeff>& rhs)
{
    return !(lhs == rhs);
}

template<typename Coeff>
bool operator<(const Term<Coeff>& lhs, const Term<Coeff>& rhs)
{
	if (lhs.mMonomial == rhs.mMonomial) return false;
	if (lhs.tdeg() < rhs.tdeg()) return true;
	if (lhs.mMonomial && rhs.mMonomial) {
		if (*(lhs.mMonomial) < *(rhs.mMonomial)) return true;
		if (*(rhs.mMonomial) < *(lhs.mMonomial)) return false;
	}
	return lhs.mCoeff < rhs.mCoeff;
}

template<typename Coefficient>
const Term<Coefficient> Term<Coefficient>::operator-() const
{
    return Term<Coefficient>(-mCoeff,mMonomial);
}

template<typename Coefficient>
Term<Coefficient>& Term<Coefficient>::operator*=(const Coefficient& rhs)
{
    if(rhs == 0) 
    {
        clear();
        return *this;
    }
    assert(mCoeff == 0 || mCoeff * rhs != 0);
    mCoeff *= rhs;
    return *this;
}
template<typename Coefficient>
Term<Coefficient>& Term<Coefficient>::operator*=(Variable::Arg rhs)
{
    if(mCoeff == 0)
    {
        return *this;
    }
    if(mMonomial)
    {
        mMonomial = std::make_shared<const Monomial>(*mMonomial * rhs);
    }
    else
    {
        mMonomial = std::make_shared<const Monomial>(rhs);
    }
    return *this;
}

template<typename Coefficient>
Term<Coefficient>& Term<Coefficient>::operator*=(const Monomial& rhs)
{
    if(mCoeff == 0) return *this;
    
    if(mMonomial)
    {
        mMonomial = std::make_shared<const Monomial>((*mMonomial) * (rhs));
        
    }
    else
    {
        mMonomial = std::make_shared<const Monomial>(rhs);
    }
    return *this;   
}


template<typename Coefficient>
Term<Coefficient>& Term<Coefficient>::operator*=(const Term& rhs)
{
    if(mCoeff == 0) return *this;
    if(rhs.mCoeff == 0) 
    {
        clear();
        return *this;
    }
    
    if(mMonomial)
    {
        if(rhs.mMonomial)
        {
            mMonomial = std::make_shared<const Monomial>((*mMonomial) * (*rhs.mMonomial));
        }
    }
    else
    {
        mMonomial = rhs.mMonomial;
    }
	mCoeff *= rhs.mCoeff;
    return *this;   
}

template<typename Coeff>
const Term<Coeff> operator*(const Term<Coeff>& lhs, const Term<Coeff>& rhs)
{
    Term<Coeff> result(lhs);
    result *= rhs;
    return result;
}
template<typename Coeff>
const Term<Coeff> operator*(const Term<Coeff>& lhs, const Coeff& rhs)
{
    return Term<Coeff>(lhs.mCoeff * rhs, lhs.mMonomial);
}
template<typename Coeff>
const Term<Coeff> operator*(const Term<Coeff>& lhs, const int& rhs)
{
    return Term<Coeff>(lhs.mCoeff * rhs, lhs.mMonomial);
}
template<typename Coeff>
const Term<Coeff> operator*(const Coeff& lhs, const Term<Coeff>& rhs)
{
    return rhs * lhs;
}
template<typename Coeff>
const Term<Coeff> operator*(const Term<Coeff>& lhs, Variable::Arg rhs)
{
    Term<Coeff> result(lhs);
    result *= rhs;
    return result;
}
template<typename Coeff>
const Term<Coeff> operator*(Variable::Arg lhs, const Term<Coeff>& rhs)
{
    return rhs * lhs;
}
template<typename Coeff>
const Term<Coeff> operator*(const Coeff& lhs, Variable::Arg rhs)
{
    return Term<Coeff>(lhs, Monomial(rhs));
}
template<typename Coeff>
const Term<Coeff> operator*(Variable::Arg lhs, const Coeff& rhs)
{
    return rhs * lhs;
}
template<typename Coeff>
const Term<Coeff> operator*(const Coeff& lhs, const Monomial& rhs)
{
    return Term<Coeff>(lhs, rhs);
}
template<typename Coeff>
const Term<Coeff> operator*(const Monomial& lhs, const Coeff& rhs)
{
    return rhs * lhs;
}


template<typename Coeff>
const Term<Coeff> operator/(const Term<Coeff>& lhs, unsigned long rhs)
{
	return Term<Coeff>(lhs.coeff()/rhs, lhs.monomial());
}

template<typename Coeff>
std::ostream& operator<<(std::ostream& os, const Term<Coeff>& rhs)
{
    return (os << rhs.toString(true, true));
}

template<typename Coefficient>
std::string Term<Coefficient>::toString(bool infix, bool friendlyVarNames) const
{ 
    if(mMonomial)
    {
        if(mCoeff != Coefficient(1))
        {
            std::stringstream s;
            if(!infix) s << " ";
            bool negative = (mCoeff < 0);
            if(negative) s << "(-" << (infix ? "" : " ");
            if(infix) s << carl::abs(mCoeff);
            else
            {
                typename IntegralType<Coefficient>::type d = getDenom(mCoeff);
                if(d != typename IntegralType<Coefficient>::type(1)) s << "(/ " << carl::abs(getNum(mCoeff)) << " " << carl::abs(d) << ")";
                else s << carl::abs(mCoeff);
            }
            if(negative) 
                s << ")";
            if(infix) return s.str() + "*" + mMonomial->toString(true, friendlyVarNames);
            else return "(*" + s.str() + " " + mMonomial->toString(infix, friendlyVarNames) + ")";
        }
        else
        {
            if(infix) return mMonomial->toString(true, friendlyVarNames);
            else return mMonomial->toString(infix, friendlyVarNames);
        }
    }
    else 
    {
        std::stringstream s;
        bool negative = (mCoeff < 0);
        if(negative)
            s << "(-" << (infix ? "" : " ");
        if(infix) s << carl::abs(mCoeff);
        else
        {
            typename IntegralType<Coefficient>::type d = getDenom(mCoeff);
            if(d != typename IntegralType<Coefficient>::type(1)) s << "(/ " << carl::abs(getNum(mCoeff)) << " " << carl::abs(d) << ")";
            else s << carl::abs(mCoeff);
        }
        if(negative)
            s << ")";
        return s.str();
    }
}

template<typename Coefficient>
bool Term<Coefficient>::isConsistent() const {
	if (this->mMonomial) {
		assert(!this->mMonomial->isConstant());
	}
	return true;
}

}