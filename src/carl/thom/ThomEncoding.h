/* 
 * File:   ThomEncoding.h
 * Author: tobias
 *
 * Created on 24. April 2016, 20:04
 */

#pragma once


#include "../core/UnivariatePolynomial.h"
#include "../core/Sign.h"

#include "ThomUtil.h"
#include "TarskiQuery/TarskiQuery.h"


namespace carl {

enum ComparisonResult {LESS, EQUAL, GREATER};

/*
 * A class for both one - and multidimensional thom encodings.
 * This is the heart of the classes RealAlgebraicNumber and RealAlgebraicPoint
 */
template<typename Coeff>
class ThomEncoding {
        
private:
        
             
        /*
         * Stores a pointer to the polynomial
         */
        std::shared_ptr<MultivariatePolynomial<Coeff>> p; 
        
        /*
         * The list of sign conditions realized by the derivatives
         * We only need to store the sign up to the P^(deg(P) - 1) since P^(deg(P)) is constant
         */
        SignCondition signs;
        
        /*
         * the main variable on this level. The derivatives are partial w.r.t. this variable.
         */
        Variable mainVar;
        
        /*
         * The point of dimension d-1 represented by the encoding on the level below
         */
        std::shared_ptr<ThomEncoding<Coeff>> point;
        
public:
        
        //////////////////
        // Constructors //
        //////////////////
        
        
        /*
         * Constructs a univariate thom encoding with the given polynomial and the sign conditions
         */
        ThomEncoding(
                std::shared_ptr<UnivariatePolynomial<Coeff>>,
                const SignCondition&          
        );
        
        /*
         * Constructs a univariate thom encoding with the given polynomial and the sign conditions
         * The polynomial is asserted to be univariate
         */
        ThomEncoding(
                std::shared_ptr<MultivariatePolynomial<Coeff>>,
                const SignCondition&          
        );
        
        
        
        /*
         * Constructs a (multivariate) thom encoding with the given polynomial
         * with the given main variable, the sign conditions and the preceding point.
         */
        ThomEncoding(
                std::shared_ptr<MultivariatePolynomial<Coeff>>,
                Variable::Arg, 
                const SignCondition&,               
                std::shared_ptr<ThomEncoding<Coeff>>
        );
        
        /*
         * Constructs a trivial univariate thom encoding for the given rational number.
         */
        ThomEncoding(const Coeff& rational, Variable::Arg var);
        
        /*
         * Constructs the thom encoding of the n-th root of p (if existent).
         * n = 1 for the first root, n = 2 for the second and so on.
         */
        ThomEncoding(const UnivariatePolynomial<Coeff>& p, uint n);
                

        
        /////////////////////////////////////
        // general public member functions //
        /////////////////////////////////////
        
        /*
         * returns the polynomial
         */
        inline MultivariatePolynomial<Coeff> polynomial() const { return *p; }
        
        /*
         * returns the main variable
         */
        inline Variable::Arg getMainVar() const { return mainVar; }
        
        /*
         * returns the sign condition
         */
        inline SignCondition getSigns() const { return signs; }
        
        /*
         * 
         */
        SignCondition accumulateSigns() const;
        
        /*
         * query if the encoding is one or multidimensional
         */
        inline bool isOneDimensional() const { return point.get() == nullptr; }
        inline bool isMultiDimensional() const { return !isOneDimensional(); }
        
        /*
         * returns the dimension of the represented point
         * this is done recursively
         */
        uint dimension() const;
        
        /*
         * returns a list of all the underlying polynomials, including the lower levels
         * this is done recursively
         */
        std::vector<MultivariatePolynomial<Coeff>> accumulatePolynomials() const;
        
        /*
         * checks some basic invariants of a thom encoding object
         */
        bool isConsistent() const;
        
        /*
         * Returns true if and only if the given rational number is encoded by this Thom encoding.
         * (for debugging)
         */
        bool represents(const Coeff& rational) const;
        
        /*
         * Returns the sign condition of the represented number realized on der(p) = p,p',...,p^{deg(p)}
         */
        SignCondition fullSignCondition() const;
        
        /*
         * constructs the (reduced) list of derivatives the internal sign cond. is related to
         * this is precisely the list p',...,p^{deg(p) - 1}
         */
        std::vector<MultivariatePolynomial<Coeff>> reducedDer() const;
        
        /*
         * recursive version of reducedDer()
         */
        std::vector<MultivariatePolynomial<Coeff>> accumulateDer() const;
        
        /*
         * Returns the sign of the n-th derivative P^(n) (counting starts from 1)
         */
        Sign operator[](const uint n) const;
        
        
        ////////////////
        // Comparison //
        ////////////////
        
        /*
         * Compares the RANs represented by the given Thom ecodings
         * This is quite expensive if the underlying polynomials are different.
         * Note that no encodings of DIFFERENT LEVElS (DIMENSIONS) can ever be compared.
         * 
         */
        template<typename C>
        friend ComparisonResult compareThom(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs);
        template<typename C>
        friend ComparisonResult compareMultivariate(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs);
        
        // compare thom encodings with thom encodings
        template<typename C>
        friend bool operator<(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) { return compareThom(lhs, rhs) == LESS; }
        
        template<typename C>
        friend bool operator<=(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) { return compareThom(lhs, rhs) != GREATER; }
        
        template<typename C>
        friend bool operator>(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) { return compareThom(lhs, rhs) == GREATER; }
        
        template<typename C>
        friend bool operator>=(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) { return compareThom(lhs, rhs) != LESS; }
        
        template<typename C>
        friend bool operator==(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) { return compareThom(lhs, rhs) == EQUAL; }
        
        template<typename C>
        friend bool operator!=(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) { return compareThom(lhs, rhs) != EQUAL; }
        
        // comparing with rational numbers
        template<typename C>
        friend bool operator<(const ThomEncoding<C>& lhs, const C& rhs) { return lhs < ThomEncoding<C>(rhs); }
        
        template<typename C>
        friend bool operator<=(const ThomEncoding<C>& lhs, const C& rhs) { return lhs <= ThomEncoding<C>(rhs); }
        
        template<typename C>
        friend bool operator>(const ThomEncoding<C>& lhs, const C& rhs) { return lhs > ThomEncoding<C>(rhs); }
        
        template<typename C>
        friend bool operator>=(const ThomEncoding<C>& lhs, const C& rhs) { return lhs >= ThomEncoding<C>(rhs); }
        
        template<typename C>
        friend bool operator==(const ThomEncoding<C>& lhs, const C& rhs) { return lhs == ThomEncoding<C>(rhs); }
        
        template<typename C>
        friend bool operator!=(const ThomEncoding<C>& lhs, const C& rhs) { return lhs != ThomEncoding<C>(rhs); }
        
        
        template<typename C>
        friend bool operator<(const C& lhs, const ThomEncoding<C>& rhs) { return ThomEncoding<C>(lhs) < rhs; }
        
        template<typename C>
        friend bool operator<=(const C& lhs, const ThomEncoding<C>& rhs) { return ThomEncoding<C>(lhs) <= rhs; }
        
        template<typename C>
        friend bool operator>(const C& lhs, const ThomEncoding<C>& rhs) { return ThomEncoding<C>(lhs) > rhs; }
        
        template<typename C>
        friend bool operator>=(const C& lhs, const ThomEncoding<C>& rhs) { return ThomEncoding<C>(lhs) >= rhs; }
        
        template<typename C>
        friend bool operator==(const C& lhs, const ThomEncoding<C>& rhs) { return ThomEncoding<C>(lhs) == rhs; }
        
        template<typename C>
        friend bool operator!=(const C& lhs, const ThomEncoding<C>& rhs) { return ThomEncoding<C>(lhs) != rhs; }
        
        
        /////////////////////////
        // Intermediate points //
        /////////////////////////
        
        // returns a thom encoding representing a number in the interval (lhs, rhs)
        template<typename C>
        friend ThomEncoding<C> intermediatePoint(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs);
        
        // adding rational numbers to ran's represented by thom encodings
        template<typename C>
        friend ThomEncoding<C> operator+(const ThomEncoding<C>& rhs, const C& lhs);
        
        template<typename C>
        friend ThomEncoding<C> operator+(const C& lhs, const ThomEncoding<C>& rhs) {
                return rhs + lhs;
        }
        
        
        /*
         * output operator
         */
        template <typename C, typename Settings = ThomDefaultSettings>
	friend std::ostream& operator<<(std::ostream& os, const ThomEncoding<C>& rhs) {
                os << "(" << rhs.polynomial() << " with mainVar " << rhs.getMainVar() << ", " << rhs.fullSignCondition() << ")" << std::endl;
                if(rhs.isMultiDimensional()) {
                        os << *(rhs.point);
                }
                return os;
        }
};


#include "ThomEncoding.tpp"
} // namespace carl




