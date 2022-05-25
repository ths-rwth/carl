#include "LPPolynomial.h"

#include <carl/core/Co
mmon.h>


namespace carl {


    /*
    * @brief wrapper for the gcd calculation using libpoly. Asserts that the polynomials have the same context
    * @param LPPolynomial p,q
    * @return gcd(p,q)
    */
    LPPolynomial gcd(const LPPolynomial& p, const LPPolynomial& q){
        return LPPolynomial(std::move(poly::gcd(p.getPolynomial(), q.getPolynomial()))) ; 
    }

     /*
    * @brief wrapper for the lcm calculation using libpoly. Asserts that the polynomials have the same context
    * @param LPPolynomial p,q
    * @return lcm(p,q)
    */
    LPPolynomial lcm(const LPPolynomial& p, const LPPolynomial& q){
        return LPPolynomial(std::move(poly::lcm(p.getPolynomial(), q.getPolynomial()))) ; 
    }

    /*
    * @brief wrapper for the polynomial content calculation using libpoly.
    * @param LPPolynomial p
    * @return content(p)
    */
    LPPolynomial content(const LPPolynomial& p){
        return LPPolynomial(std::move(poly::content(p.getPolynomial()))) ; 
    }

    /*
    * @brief wrapper for the polynomial primitive_part calculation using libpoly.
    * @param LPPolynomial p
    * @return primitive_part(p)
    */
    LPPolynomial primitive_part(const LPPolynomial& p){
        return LPPolynomial(std::move(poly::primitive_part(p.getPolynomial()))) ; 
    }

    /*
    * @brief wrapper for the resultant calculation using libpoly. Asserts that the polynomials have the same context
    * @param LPPolynomial p,q
    * @return resultant(p,q)
    */
    LPPolynomial resultant(const LPPolynomial& p, const LPPolynomial& q){
        return LPPolynomial(std::move(poly::resultant(p.getPolynomial(), q.getPolynomial()))) ; 
    }

    /*
    * @brief wrapper for the discriminant calculation using libpoly.
    * @param LPPolynomial p
    * @return discriminant(p)
    */
    LPPolynomial discriminant(const LPPolynomial& p){
        return LPPolynomial(std::move(poly::discriminant(p.getPolynomial()))) ; 
    }

    
    Factors<LPPolynomial> factorization(const LPPolynomial& p){

    }

} // namespace carl
