#pragma once

#include "BasicConstraint.h"

namespace carl {

const signed A_IFF_B = 2;
const signed A_IMPLIES_B = 1;
const signed B_IMPLIES_A = -1;
const signed NOT__A_AND_B = -2;
const signed A_AND_B__IFF_C = -3;
const signed A_XOR_B = -4;

/**
 * Compares _constraintA with _constraintB.
 * @return  2, if it is easy to decide that _constraintA and _constraintB have the same solutions. _constraintA = _constraintB
 *           1, if it is easy to decide that _constraintB includes all solutions of _constraintA;   _constraintA -> _constraintB
 *          -1, if it is easy to decide that _constraintA includes all solutions of _constraintB;   _constraintB -> _constraintA
 *          -2, if it is easy to decide that _constraintA has no solution common with _constraintB; not(_constraintA and _constraintB)
 *          -3, if it is easy to decide that _constraintA and _constraintB can be intersected;      _constraintA and _constraintB = _constraintC
 *          -4, if it is easy to decide that _constraintA is the inverse of _constraintB;           _constraintA xor _constraintB
 *           0, otherwise.
 */
template<typename Pol>
signed compare(const BasicConstraint<Pol>& _constraintA, const BasicConstraint<Pol>& _constraintB) {
	/*
    * Check whether it holds that 
    * 
    *                      _constraintA  =  a_1*m_1+...+a_k*m_k + c ~ 0
    * and 
    *                      _constraintB  =  b_1*m_1+...+b_k*m_k + d ~ 0, 
    * 
    * where a_1,..., a_k, b_1,..., b_k, c, d are rational coefficients, 
    *       m_1,..., m_k are non-constant monomials and 
    *       exists a rational g such that 
    * 
    *                   a_i = g * b_i for all 1<=i<=k 
    *              or   b_i = g * a_i for all 1<=i<=k 
    */
	typename Pol::NumberType one_divided_by_a = _constraintA.lhs().coprimeFactorWithoutConstant();
	typename Pol::NumberType one_divided_by_b = _constraintB.lhs().coprimeFactorWithoutConstant();
	typename Pol::NumberType c = _constraintA.lhs().constantPart();
	typename Pol::NumberType d = _constraintB.lhs().constantPart();
	assert(carl::isOne(carl::getNum(carl::abs(one_divided_by_b))));
	Pol tmpA = (_constraintA.lhs() - c) * one_divided_by_a;
	Pol tmpB = (_constraintB.lhs() - d) * one_divided_by_b;
	//        std::cout << "tmpA = " << tmpA << std::endl;
	//        std::cout << "tmpB = " << tmpB << std::endl;
	if (tmpA != tmpB) return 0;
	bool termACoeffGreater = false;
	bool signsDiffer = (one_divided_by_a < carl::constant_zero<typename Pol::NumberType>::get()) != (one_divided_by_b < carl::constant_zero<typename Pol::NumberType>::get());
	typename Pol::NumberType g;
	if (carl::getDenom(one_divided_by_a) > carl::getDenom(one_divided_by_b)) {
		g = typename Pol::NumberType(carl::getDenom(one_divided_by_a)) / carl::getDenom(one_divided_by_b);
		if (signsDiffer)
			g = -g;
		termACoeffGreater = true;
		d *= g;
	} else {
		g = typename Pol::NumberType(carl::getDenom(one_divided_by_b)) / carl::getDenom(one_divided_by_a);
		if (signsDiffer)
			g = -g;
		c *= g;
	}
	// Apply the multiplication by a negative g to the according relation symbol, which
	// has to be turned around then.
	Relation relA = _constraintA.relation();
	Relation relB = _constraintB.relation();
	if (g < 0) {
		if (termACoeffGreater) {
			switch (relB) {
			case Relation::LEQ:
				relB = Relation::GEQ;
				break;
			case Relation::GEQ:
				relB = Relation::LEQ;
				break;
			case Relation::LESS:
				relB = Relation::GREATER;
				break;
			case Relation::GREATER:
				relB = Relation::LESS;
				break;
			default:
				break;
			}
		} else {
			switch (relA) {
			case Relation::LEQ:
				relA = Relation::GEQ;
				break;
			case Relation::GEQ:
				relA = Relation::LEQ;
				break;
			case Relation::LESS:
				relA = Relation::GREATER;
				break;
			case Relation::GREATER:
				relA = Relation::LESS;
				break;
			default:
				break;
			}
		}
	}
	//        std::cout << "c = " << c << std::endl;
	//        std::cout << "d = " << d << std::endl;
	//        std::cout << "g = " << g << std::endl;
	//        std::cout << "relA = " << relA << std::endl;
	//        std::cout << "relB = " << relB << std::endl;
	// Compare the adapted constant parts.
	switch (relB) {
	case Relation::EQ:
		switch (relA) {
		case Relation::EQ: // p+c=0  and  p+d=0
			if (c == d) return A_IFF_B;
			return NOT__A_AND_B;
		case Relation::NEQ: // p+c!=0  and  p+d=0
			if (c == d) return A_XOR_B;
			return B_IMPLIES_A;
		case Relation::LESS: // p+c<0  and  p+d=0
			if (c < d) return B_IMPLIES_A;
			return NOT__A_AND_B;
		case Relation::GREATER: // p+c>0  and  p+d=0
			if (c > d) return B_IMPLIES_A;
			return NOT__A_AND_B;
		case Relation::LEQ: // p+c<=0  and  p+d=0
			if (c <= d) return B_IMPLIES_A;
			return NOT__A_AND_B;
		case Relation::GEQ: // p+c>=0  and  p+d=0
			if (c >= d) return B_IMPLIES_A;
			return NOT__A_AND_B;
		default:
			return false;
		}
	case Relation::NEQ:
		switch (relA) {
		case Relation::EQ: // p+c=0  and  p+d!=0
			if (c == d) return A_XOR_B;
			return A_IMPLIES_B;
		case Relation::NEQ: // p+c!=0  and  p+d!=0
			if (c == d) return A_IFF_B;
			return 0;
		case Relation::LESS: // p+c<0  and  p+d!=0
			if (c >= d) return A_IMPLIES_B;
			return 0;
		case Relation::GREATER: // p+c>0  and  p+d!=0
			if (c <= d) return A_IMPLIES_B;
			return 0;
		case Relation::LEQ: // p+c<=0  and  p+d!=0
			if (c > d) return A_IMPLIES_B;
			if (c == d) return A_AND_B__IFF_C;
			return 0;
		case Relation::GEQ: // p+c>=0  and  p+d!=0
			if (c < d) return A_IMPLIES_B;
			if (c == d) return A_AND_B__IFF_C;
			return 0;
		default:
			return 0;
		}
	case Relation::LESS:
		switch (relA) {
		case Relation::EQ: // p+c=0  and  p+d<0
			if (c > d) return A_IMPLIES_B;
			return NOT__A_AND_B;
		case Relation::NEQ: // p+c!=0  and  p+d<0
			if (c <= d) return B_IMPLIES_A;
			return 0;
		case Relation::LESS: // p+c<0  and  p+d<0
			if (c == d) return A_IFF_B;
			if (c < d) return B_IMPLIES_A;
			return A_IMPLIES_B;
		case Relation::GREATER: // p+c>0  and  p+d<0
			if (c <= d) return NOT__A_AND_B;
			return 0;
		case Relation::LEQ: // p+c<=0  and  p+d<0
			if (c > d) return A_IMPLIES_B;
			return B_IMPLIES_A;
		case Relation::GEQ: // p+c>=0  and  p+d<0
			if (c < d) return NOT__A_AND_B;
			if (c == d) return A_XOR_B;
			return 0;
		default:
			return 0;
		}
	case Relation::GREATER: {
		switch (relA) {
		case Relation::EQ: // p+c=0  and  p+d>0
			if (c < d) return A_IMPLIES_B;
			return NOT__A_AND_B;
		case Relation::NEQ: // p+c!=0  and  p+d>0
			if (c >= d) return B_IMPLIES_A;
			return 0;
		case Relation::LESS: // p+c<0  and  p+d>0
			if (c >= d) return NOT__A_AND_B;
			return 0;
		case Relation::GREATER: // p+c>0  and  p+d>0
			if (c == d) return A_IFF_B;
			if (c > d) return B_IMPLIES_A;
			return A_IMPLIES_B;
		case Relation::LEQ: // p+c<=0  and  p+d>0
			if (c > d) return NOT__A_AND_B;
			if (c == d) return A_XOR_B;
			return 0;
		case Relation::GEQ: // p+c>=0  and  p+d>0
			if (c > d) return B_IMPLIES_A;
			return A_IMPLIES_B;
		default:
			return 0;
		}
	}
	case Relation::LEQ: {
		switch (relA) {
		case Relation::EQ: // p+c=0  and  p+d<=0
			if (c >= d) return A_IMPLIES_B;
			return NOT__A_AND_B;
		case Relation::NEQ: // p+c!=0  and  p+d<=0
			if (c < d) return B_IMPLIES_A;
			if (c == d) return A_AND_B__IFF_C;
			return 0;
		case Relation::LESS: // p+c<0  and  p+d<=0
			if (c < d) return B_IMPLIES_A;
			return A_IMPLIES_B;
		case Relation::GREATER: // p+c>0  and  p+d<=0
			if (c < d) return NOT__A_AND_B;
			if (c == d) return A_XOR_B;
			return 0;
		case Relation::LEQ: // p+c<=0  and  p+d<=0
			if (c == d) return A_IFF_B;
			if (c < d) return B_IMPLIES_A;
			return A_IMPLIES_B;
		case Relation::GEQ: // p+c>=0  and  p+d<=0
			if (c < d) return NOT__A_AND_B;
			if (c == d) return A_AND_B__IFF_C;
			return 0;
		default:
			return 0;
		}
	}
	case Relation::GEQ: {
		switch (relA) {
		case Relation::EQ: // p+c=0  and  p+d>=0
			if (c <= d) return A_IMPLIES_B;
			return NOT__A_AND_B;
		case Relation::NEQ: // p+c!=0  and  p+d>=0
			if (c > d) return B_IMPLIES_A;
			if (c == d) return A_AND_B__IFF_C;
			return 0;
		case Relation::LESS: // p+c<0  and  p+d>=0
			if (c > d) return NOT__A_AND_B;
			if (c == d) return A_XOR_B;
			return 0;
		case Relation::GREATER: // p+c>0  and  p+d>=0
			if (c < d) return B_IMPLIES_A;
			return A_IMPLIES_B;
		case Relation::LEQ: // p+c<=0  and  p+d>=0
			if (c > d) return NOT__A_AND_B;
			if (c == d) return A_AND_B__IFF_C;
			return 0;
		case Relation::GEQ: // p+c>=0  and  p+d>=0
			if (c == d) return A_IFF_B;
			if (c < d) return A_IMPLIES_B;
			return B_IMPLIES_A;
		default:
			return 0;
		}
	}
	default:
		return 0;
	}
}

}