/**
 * @file MonomialOrdering.h
 * @ingroup multirp
 */
#pragma once

#include "CompareResult.h"
#include "Monomial.h"
#include "Term.h"

namespace carl 
{

typedef CompareResult( *MonomialOrderingFc )(std::shared_ptr<const Monomial>, std::shared_ptr<const Monomial>);

/**
 * A class for term orderings.
 * @ingroup multirp
 */
template<MonomialOrderingFc f, bool degreeOrdered>
struct MonomialComparator
{
    
    template<typename Coeff>
    bool operator()(const Term<Coeff>& t1, const Term<Coeff>& t2)
    {
        return (compare(t1, t2) == CompareResult::LESS );
    }
    
    template<typename Coeff>
    bool operator()(std::shared_ptr<const Term<Coeff>> t1, std::shared_ptr<const Term<Coeff>> t2)
    {
        return compare(t1, t2) == CompareResult::LESS;
    }
	
	
    bool operator()(std::shared_ptr<const Monomial> m1, std::shared_ptr<const Monomial> m2)
    {
        return (compare(m1, m2) == CompareResult::LESS );
    }

	template<typename Coeff>
    static CompareResult compare(std::shared_ptr<const Monomial> m1, std::shared_ptr<const Monomial> m2)
	{
		return compare(m1, m2);
	}
 
	
	template<typename Coeff>
    static CompareResult compare( const std::shared_ptr<const Term<Coeff>>& t1, const std::shared_ptr<const Term<Coeff>>& t2 )
    {
		if(!t1 && !t2) return CompareResult::EQUAL;
		if(!t1) return CompareResult::LESS;
		if(!t2) return CompareResult::GREATER;
        return compare(*t1, *t2);
    }
    
    template<typename Coeff>
    static CompareResult compare(const Term<Coeff>& t1, const Term<Coeff>& t2)
    {
        if(t1.monomial() && t2.monomial())
        {
            if(t1.monomial() == t2.monomial()) return CompareResult::EQUAL;
            return f(t1.monomial(), t2.monomial());
        }
        else if(!t1.monomial() && t2.monomial())
        {
            return CompareResult::LESS;
        }
        else if(t1.monomial() && !t2.monomial())
        {
            return CompareResult::GREATER;
        }
        else
        {
            return CompareResult::EQUAL;
        }
    }
    
    template<typename Coeff>
    static CompareResult compare(const Term<Coeff>& t1, Variable::Arg v)
    {
        /// @todo: let f have the template parameter specifying the type of its second argument
        #ifdef USE_MONOMIAL_POOL
        return f(t1.monomial(), MonomialPool::getInstance().create( v ));
        #else
        return f(t1.monomial(), std::shared_ptr<const Monomial>( new Monomial( v ) ));
        #endif
    }
    
    template<typename Coeff>
    static bool less(const Term<Coeff>& t1, const Term<Coeff>& t2)
    {
        
        return (compare(t1, t2) == CompareResult::LESS );
    }
    
    template<typename Coeff>
    static bool less(const std::shared_ptr<const Term<Coeff>>& t1, const std::shared_ptr<const Term<Coeff>>& t2)
    {
        if(t1 == t2) return false;
        return (compare(t1, t2) == CompareResult::LESS );
    }
	
	template<typename Coeff>
    static bool greater(const std::shared_ptr<const Term<Coeff>>& t1, const std::shared_ptr<const Term<Coeff>>& t2)
    {
        if(t1 == t2) return false;
        return (compare(t1, t2) == CompareResult::GREATER );
    }
    
    template<typename Coeff>
    static bool equal(const std::shared_ptr<const Term<Coeff>>& t1, const std::shared_ptr<const Term<Coeff>>& t2)
    {
        if(t1 == t2) return true;
        return (compare(t1, t2) == CompareResult::EQUAL );
    }

    static const bool degreeOrder = degreeOrdered;
};


typedef MonomialComparator<Monomial::compareLexical, false > LexOrdering;
typedef MonomialComparator<Monomial::compareGradedLexical, true > GrLexOrdering;
//typedef MonomialComparator<Monomial::GrRevLexCompare, true > GrRevLexOrdering;
}