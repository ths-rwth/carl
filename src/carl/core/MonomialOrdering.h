#pragma once

#include "CompareResult.h"
#include "Monomial.h"
#include "Term.h"

namespace carl 
{

typedef CompareResult( *MonomialOrderingFc )(const Monomial&, const Monomial&);

template<MonomialOrderingFc f, bool degreeOrdered>
struct MonomialComparator
{

    bool operator()(const Monomial& m1, const Monomial& m2 )
    {
        return (compare( m1, m2 ) == CompareResult::LESS );
    }
    
    template<typename Coeff>
    bool operator()(const Term<Coeff>& t1, const Term<Coeff>& t2)
    {
        return (compare(*t1.monomial(), *t2.monomial()) == CompareResult::LESS );
    }
    
    template<typename Coeff>
    bool operator()(const std::shared_ptr<const Term<Coeff>>& t1, const std::shared_ptr<const Term<Coeff>>& t2)
    {
        return (compare(*(t1->monomial()), *(t2->monomial())) == CompareResult::LESS );
    }

    static CompareResult compare( const Monomial& m1, const Monomial& m2 )
    {
        return f( m1, m2 );
    }
    
    template<typename Coeff>
    static CompareResult compare(const Term<Coeff>& t1, const Term<Coeff>& t2)
    {
        if(t1.monomial() && t2.monomial())
        {
            if(t1.monomial() == t2.monomial()) return CompareResult::EQUAL;
            return f(*t1.monomial(), *t2.monomial());
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
        if(!t1.monomial()) return CompareResult::LESS;
        return f(*t1.monomial(), v);
    }
    

    static bool less(const Monomial& m1, const Monomial& m2)
    {
        return (compare( m1, m2 ) == CompareResult::LESS );
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
        return (compare(*t1, *t2) == CompareResult::LESS );
    }

    static bool greater(const Monomial& m1, const Monomial& m2)
    {
        return (compare( m1, m2 ) == CompareResult::GREATER );
    }

    static const bool degreeOrder = degreeOrdered;
};

typedef MonomialComparator<Monomial::compareLexical, false > LexOrdering;
typedef MonomialComparator<Monomial::compareGradedLexical, true > GrLexOrdering;
//typedef MonomialComparator<Monomial::GrRevLexCompare, true > GrRevLexOrdering;
}