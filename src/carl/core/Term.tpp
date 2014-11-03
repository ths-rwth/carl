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
    mCoeff(Coefficient(0)), mMonomial()
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
Term<Coefficient>::Term(std::shared_ptr<const Monomial> m) :
    mCoeff(1), mMonomial(m)
{
    assert(this->isConsistent());
}

template<typename Coefficient>
Term<Coefficient>::Term(const Coefficient& c, std::shared_ptr<const Monomial> m) :
    mCoeff(c), mMonomial(m)
{
    assert(this->isConsistent());
}

template<typename Coefficient>
Term<Coefficient>::Term(const Coefficient& c, Variable::Arg v, exponent e): 
    mCoeff(c),
    #ifdef USE_MONOMIAL_POOL
    mMonomial( MonomialPool::getInstance().create( v, e ) )
    #else
    mMonomial( std::shared_ptr<const Monomial>( new Monomial(v, e) ) )
    #endif
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
Term<Coefficient>* Term<Coefficient>::divideBy(std::shared_ptr<const Monomial> m) const
{
    if(mMonomial)
    {
		auto res = mMonomial->divide(m);
        if (res.second) {
			if (res.first != nullptr && res.first->tdeg() == 0) {
				delete res.first;
				return new Term<Coefficient>(mCoeff);
			}
            return new Term<Coefficient>(mCoeff, res.first);
        }   
    }
    return nullptr;  
}

template<typename Coefficient>
Term<Coefficient>* Term<Coefficient>::divideBy(const Term& t) const
{
    assert(t.mCoeff != Coefficient(0));
    if(mMonomial)
    {
        if(!t.mMonomial)
        {
            // Term is just a constant.
            return new Term<Coefficient>(mCoeff / t.mCoeff, mMonomial);
        }
		auto res = mMonomial->divide(*(t.mMonomial));
		if (!res.second) return nullptr;
		if (res.first != nullptr) {
			if (res.first->tdeg() == 0) {
				delete res.first;
				return new Term<Coefficient>(mCoeff / t.mCoeff);
			}
            return new Term<Coefficient>(mCoeff / t.mCoeff, res.first);
        } else {
			return new Term<Coefficient>(mCoeff / t.mCoeff);
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
		return new Term<Coefficient>(Coefficient(0));
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
            return (mCoeff < Coefficient(0) ? Definiteness::NEGATIVE_SEMI : Definiteness::POSITIVE_SEMI);
    }
    else if(mCoeff != Coefficient(0))
        return (mCoeff < Coefficient(0) ? Definiteness::NEGATIVE : Definiteness::POSITIVE);
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
Term<Coefficient> Term<Coefficient>::calcLcmAndDivideBy(std::shared_ptr<const Monomial> m) const
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
template<typename C, EnableIf<is_field<C>>>
bool Term<Coefficient>::divisible(const Term& t) const {
	if (this->monomial() == nullptr) return t.monomial() == nullptr;
	if (t.monomial() == nullptr) return true;
	return this->monomial()->divisible(*t.monomial());
}

template<typename Coefficient>
template<typename C, DisableIf<is_field<C>>>
bool Term<Coefficient>::divisible(const Term& t) const {
	if (carl::remainder(this->coeff(), t.coeff()) != Coefficient(0)) return false;
	if (this->monomial() == nullptr) return t.monomial() == nullptr;
	if (t.monomial() == nullptr) return true;
	return this->monomial()->divisible(*t.monomial());
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
bool operator==(const Term<Coeff>& lhs, const Term<Coeff>& rhs) {
	if (lhs.coeff() != rhs.coeff()) return false;
	return std::equal_to<std::shared_ptr<const Monomial>>()(lhs.monomial(), rhs.monomial());
}
template<typename Coeff>
bool operator==(const Term<Coeff>& lhs, const Monomial& rhs) {
	if (lhs.coeff() != Coeff(1)) return false;
    return lhs.monomial() && *(lhs.monomial()) == rhs;
}
template<typename Coeff>
bool operator==(const Term<Coeff>& lhs, Variable::Arg rhs) {
	if (lhs.coeff() != Coeff(1)) return false;
    return lhs.monomial() && *(lhs.monomial()) == rhs;
}
template<typename Coeff>
bool operator==(const Term<Coeff>& lhs, const Coeff& rhs) {
    return !lhs.monomial() && lhs.coeff() == rhs;
}

template<typename Coeff>
bool operator<(const Term<Coeff>& lhs, const Term<Coeff>& rhs) {
	if (lhs.monomial() == rhs.monomial()) return lhs.coeff() < rhs.coeff();
	if (lhs.monomial() == nullptr) return true;
	if (rhs.monomial() == nullptr) return false;
	if (*(lhs.monomial()) < *(rhs.monomial())) return true;
	if (*(rhs.monomial()) < *(lhs.monomial())) return false;
	return lhs.coeff() < rhs.coeff();
}

template<typename Coeff>
bool operator<(const Term<Coeff>& lhs, const Monomial& rhs) {
	if (lhs.monomial() == nullptr) return true;
	if (*(lhs.monomial()) == rhs) return lhs.coeff() < Coeff(1);
	return *(lhs.monomial()) < rhs;
}

template<typename Coeff>
bool operator<(const Term<Coeff>& lhs, Variable::Arg rhs) {
	if (lhs.monomial() == nullptr) return true;
	if (*(lhs.monomial()) == rhs) return lhs.coeff() < Coeff(1);
	return *(lhs.monomial()) < rhs;
}

template<typename Coeff>
bool operator<(const Term<Coeff>& lhs, const Coeff& rhs) {
	if (lhs.monomial() == nullptr) return lhs.coeff() < rhs;
	return false;
}

template<typename Coeff>
bool operator<(const Monomial& lhs, const Term<Coeff>& rhs) {
	if (rhs.monomial() == nullptr) return false;
	if (lhs == *(rhs.monomial())) return Coeff(1) < rhs.coeff();
	return lhs < *(rhs.monomial());
}

template<typename Coeff>
bool operator<(Variable::Arg lhs, const Term<Coeff>& rhs) {
	if (rhs.monomial() == nullptr) return false;
	if (lhs == *(rhs.monomial())) return Coeff(1) < rhs.coeff();
	return lhs < *(rhs.monomial());
}

template<typename Coeff>
bool operator<(const Coeff& lhs, const Term<Coeff>& rhs) {
	if (rhs.monomial() == nullptr) return lhs < rhs.coeff();
	return true;
}

template<typename Coefficient>
const Term<Coefficient> Term<Coefficient>::operator-() const
{
    return Term<Coefficient>(-mCoeff,mMonomial);
}

template<typename Coefficient>
Term<Coefficient>& Term<Coefficient>::operator*=(const Coefficient& rhs)
{
    if(rhs == Coefficient(0)) 
    {
        clear();
        return *this;
    }
    assert(mCoeff == Coefficient(0) || mCoeff * rhs != Coefficient(0));
    mCoeff *= rhs;
    return *this;
}
template<typename Coefficient>
Term<Coefficient>& Term<Coefficient>::operator*=(Variable::Arg rhs)
{
    if(mCoeff == Coefficient(0))
    {
        return *this;
    }
    if(mMonomial)
    {
        mMonomial = std::shared_ptr<const Monomial>(mMonomial * rhs);
    }
    else
    {
        mMonomial = std::shared_ptr<const Monomial>(new Monomial(rhs));
    }
    return *this;
}

template<typename Coefficient>
Term<Coefficient>& Term<Coefficient>::operator*=(std::shared_ptr<const Monomial> rhs)
{
    if(mCoeff == Coefficient(0)) return *this;
    
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
    if(mCoeff == Coefficient(0)) return *this;
    if(rhs.mCoeff == Coefficient(0)) 
    {
        clear();
        return *this;
    }
    
    mMonomial = mMonomial * rhs.mMonomial;
	mCoeff *= rhs.mCoeff;
    return *this;   
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

template<typename Coeff>
std::ostream& operator<<(std::ostream& os, const std::shared_ptr<const Term<Coeff>>& rhs)
{
	if (rhs) return os << *rhs;
	else return os << "nullptr";
}

template<typename Coefficient>
template<typename C, DisableIf<is_interval<C>>>
std::string Term<Coefficient>::toString(bool infix, bool friendlyVarNames) const
{ 
    if(mMonomial)
    {
        if(mCoeff != Coefficient(1))
        {
            std::stringstream s;
            if(!infix) s << " ";
            bool negative = (mCoeff < Coefficient(0));
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
        bool negative = (mCoeff < Coefficient(0));
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
template<typename C, EnableIf<is_interval<C>>>
std::string Term<Coefficient>::toString(bool infix, bool friendlyVarNames) const
{ 
    if(mMonomial)
    {
        if(mCoeff != C(1))
        {
            std::stringstream s;
            s << mCoeff;
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
        s << mCoeff;
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



template<typename Coefficient>
Term<Coefficient> Term<Coefficient>::gcd(std::shared_ptr<const Term<Coefficient>> t1, std::shared_ptr<const Term<Coefficient>> t2)
{
	static_assert(is_field<Coefficient>::value, "Not yet defined for other coefficients");
	assert(t1);
	assert(t2);
	return gcd(*t1, *t2);
	
}

template<typename Coefficient>
Term<Coefficient> Term<Coefficient>::gcd(const Term<Coefficient>& t1, const Term<Coefficient>& t2)
{
	static_assert(is_field<Coefficient>::value, "Not yet defined for other coefficients");
	assert(!t1.isZero());
	assert(!t2.isZero());
	if(t1.isConstant() || t2.isConstant()) return Term(Coefficient(1));
	return Term(Coefficient(carl::gcd(t1.coeff(), t2.coeff())), Monomial::gcd(t1.monomial(), t2.monomial()));
}


}