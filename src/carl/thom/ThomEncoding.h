/* 
 * File:   ThomEncoding.h
 * Author: tobias
 *
 * Created on 24. April 2016, 20:04
 */

#pragma once


#include "../core/UnivariatePolynomial.h"
#include "../core/Sign.h"

/*
 * TODO list
 * - put sign determinatation in its own header
 */

namespace carl {

// a list of sign conditions that a list of polynomials realizes at a point
// we also view this as a mapping from the polynomials to a Sign
typedef std::vector<Sign> SignCondition; 

/*
 * Calculates the set of sign conditions realized by the polynomials in the list p on the roots of z.
 * This is essential to many algorithms dealing with thom encondings of RANs.
 */
template<typename Coeff>
std::vector<SignCondition> signDetermination(const std::vector<UnivariatePolynomial<Coeff>>& p, const UnivariatePolynomial<Coeff>& z, const bool = false);

/*
 *
 * 
 */
template<typename Coeff>
class ThomEncoding {
        
private:
        
        /*
         * Stores a pointer to the polynomial
         */
        std::shared_ptr<UnivariatePolynomial<Coeff>> p;
        
        /*
         * The list of sign conditions realized by the derivatives
         * We only need to store the sign up to the P^(deg(P) - 1) since P^(deg(P)) is constant
         */
        SignCondition signs;
        
public:
        /*
         * some default constructor (needed?)
         */
        //ThomEncoding();
        
        /*
         * Constructs a thom encoding with the given polynomial and the sign conditions
         */
        ThomEncoding(const std::shared_ptr<UnivariatePolynomial<Coeff>>& ptr, const SignCondition& s);
        
        /*
         * Constructs a trivial thom encoding for the given rational number.
         */
        ThomEncoding(Coeff rational);
        
        /*
         * Constructs the thom encoding of the n-th root of p (if existent).
         * n = 1 for the first root, n = 2 for the second and so on.
         */
        ThomEncoding(const UnivariatePolynomial<Coeff>& p, unsigned n);
                
        // COPY CONSTRUCTOR NEEDED??
        
        /*
         * checks some basic invariants of a thom encoding object (for debugging purpose)
         */
        bool isConsistent() const {
                if(p == nullptr) {                              // p not initialized
                        return signs.size() == 0;
                }
                if(tarskiQuery(p->one(), *p) == 0) {            // p has no roots - this is not ok
                        return false;
                }
                if(signs.size() != p->degree() - 1) {           // SignCondition has correct number of elements
                        return false;
                }
                return true;
        }
        
        /*
         * Return true if and only if the given rational number is encoded by this Thom encoding.
         * (for debugging purposes)
         */
        bool represents(Coeff rational) const {
                std::vector<UnivariatePolynomial<Coeff>> derivatives;
                derivatives.reserve(p->degree() - 1);
                for(uint n = 1; n < p->degree(); n++ ) {
                        derivatives.push_back(p->derivative(n));
                }
                assert(derivatives.size() == p->degree() - 1);
                assert(derivatives.size() == signs.size());
                for(uint n = 1; n < derivatives.size(); n++) {
                        if(Sign(sgn(derivatives[n].evaluate(rational))) != signs[n]) {
                                return false;
                        }
                }
                return true;
        }
        
        /*
         * Returns the sign of the n-th derivative P^(n) (counting starts from 1)
         */
        Sign operator[](const uint n) const;
        
        /*
         * Compares the RANs represented by the given Thom ecodings
         * This is quite expensive if the underlying polynomials are different.
         */
        template<typename C>
        friend bool operator<(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs);
        
        // TODO implement these operators
        template<typename C>
        friend bool operator<=(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs);
        
        template<typename C>
        friend bool operator>(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs);
        
        template<typename C>
        friend bool operator>=(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs);
        
        template<typename C>
        friend bool operator==(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs);
        
        template<typename C>
        friend bool operator!=(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs);
        
        /*
         * output operator
         */
        template <typename C>
	friend std::ostream& operator<<(std::ostream& os, const ThomEncoding<C>& rhs) {
                assert(rhs.p != nullptr);
                os << "(" << *(rhs.p) << ", " << rhs.signs << ")" << std::endl;
                return os;
        }
};


#include "ThomEncoding.tpp"
} // namespace carl




