/**
 * @file MonomialOrdering.h
 * @ingroup multirp
 */
#pragma once

#include <carl-arith/core/CompareResult.h>
#include "Monomial.h"
#include "Term.h"

namespace carl 
{

using MonomialOrderingFunction = CompareResult(*)(const Monomial::Arg&, const Monomial::Arg&);

/**
 * A class for term orderings.
 * @ingroup multirp
 */
template<MonomialOrderingFunction f, bool degreeOrdered>
struct MonomialComparator
{
	static CompareResult compare(const Monomial::Arg& m1, const Monomial::Arg& m2) {
		return f(m1, m2);
	}
    
    template<typename Coeff>
    static CompareResult compare(const Term<Coeff>& t1, const Term<Coeff>& t2) {
        return compare(t1.monomial(), t2.monomial());
    }
    
    template<typename Coeff>
    static bool less(const Term<Coeff>& t1, const Term<Coeff>& t2) {
        return (compare(t1, t2) == CompareResult::LESS );
    }
	
	static bool less(const Monomial::Arg& m1, const Monomial::Arg& m2) {
        return (compare(m1, m2) == CompareResult::LESS );
    }
	
	template<typename Coeff>
    static bool equal(const Term<Coeff>& t1, const Term<Coeff>& t2) {
        return (compare(t1, t2) == CompareResult::EQUAL );
    }
	
	static bool equal(const Monomial::Arg& m1, const Monomial::Arg& m2) {
        return (compare(m1, m2) == CompareResult::EQUAL );
    }
	
	bool operator()(const Monomial::Arg& m1, const Monomial::Arg& m2) const {
		return less(m1, m2);
	}
	template<typename Coeff>
	bool operator()(const Term<Coeff>& t1, const Term<Coeff>& t2) const {
		return less(t1, t2);
	}

    static const bool degreeOrder = degreeOrdered;
};


using LexOrdering = MonomialComparator<Monomial::compareLexical, false >;
using GrLexOrdering = MonomialComparator<Monomial::compareGradedLexical, true >;
//using GrRevLexOrdering = MonomialComparator<Monomial::GrRevLexCompare, true >;
}
