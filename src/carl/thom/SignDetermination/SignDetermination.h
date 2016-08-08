/* 
 * File:   SignDeterminationGeneric.h
 * Author: tobias
 *
 * Created on 14. Juni 2016, 19:18
 */

#pragma once

#include <Eigen/Dense>
#include <cmath>

#include "../TarskiQuery/TarskiQuery.h"
#include "../ThomUtil.h"
#include "../ThomSettings.h"

namespace carl {

/*
 * Calculates the set of sign conditions realized by the polynomials in the list p on the roots of z.
 * This is essential to many algorithms dealing with thom encondings of RANs.
 * 
 * z ist the zero set. In the univariate case this can only contain one polynomial.
 * (in theory, it could contain more, but then gcd would have to be computed and we dont want this)
 * in the multivariate case, it should contain as many polynomials as there are variables
 */
template<typename Polynomial, typename Settings = ThomDefaultSettings>
std::vector<SignCondition> signDetermination(const std::vector<Polynomial>& p, const std::vector<Polynomial>& z);

template<typename Polynomial, typename Settings = ThomDefaultSettings>
std::vector<SignCondition> signDetermination(const std::vector<Polynomial>& p, const Polynomial& z) {
        return signDetermination(p, std::vector<Polynomial>(1, z));
}

// todo: offer a version that takes bounds?

        
using Eigen::MatrixXf;
using Eigen::VectorXf;
using Eigen::Matrix3f;
using Eigen::Vector3f;
using Eigen::FullPivLU;
using Eigen::PartialPivLU;

MatrixXf adaptedMat(const std::vector<std::vector<unsigned>>& A, const std::vector<SignCondition>& sign);

// we use some global variables here
const std::vector<SignCondition> basicSignCondition = {{Sign::ZERO}, {Sign::POSITIVE}, {Sign::NEGATIVE}};
const std::vector<std::vector<unsigned>> basicAdaptedList = {{0}, {1}, {2}};
const Matrix3f basicMatrixInverse = adaptedMat(basicAdaptedList, basicSignCondition).inverse();

template<typename T>
std::vector<T> operator*(const T elem, const std::vector<T>& vec) {
        std::vector<T> res(vec);
        res.insert(res.begin(), elem);
        return res;
}

float sigmaToTheAlpha(const std::vector<unsigned>& alpha, const SignCondition& sigma);


MatrixXf kroneckerProduct(const MatrixXf& m1, const MatrixXf& m2);

// the interfaces of univariate/multivariatePolynomial are somewhat incompatible
// we need a way to get the constant 1 polynomial with the correct main var
template<typename Polynomial>
Polynomial constantOne(const Polynomial& p);
template<typename Coeff>
UnivariatePolynomial<Coeff> constantOne(const UnivariatePolynomial<Coeff>& p) { return p.one(); }
template<typename Coeff>
MultivariatePolynomial<Coeff> constantOne(const MultivariatePolynomial<Coeff>& p) { (void)p; return MultivariatePolynomial<Coeff>(Coeff(1)); }

template<typename Polynomial, typename Settings>
VectorXf compute_dprime(const std::vector<Polynomial>& P,
                        const std::vector<std::vector<unsigned>>& B,
                        const std::vector<std::vector<unsigned>>& Ada,
                        const TarskiQueryManager<Polynomial>& tarskiQuery) {
        CARL_LOG_FUNC("carl.thom.sign", "P = " << P << "B = " << B << "Ada = " << Ada);
        CARL_LOG_ASSERT("carl.thom.sign", B.front().size() == 1, ""); // must hold for all vectors in B
        CARL_LOG_ASSERT("carl.thom.sign", P.size() == Ada.front().size() + 1, "");
        
        // first calculate the list of mappings B x Ada
        std::vector<std::vector<unsigned>> bxada;
        for(uint i = 0; i < Ada.size(); i++) {
                for(uint j = 0; j < B.size(); j++) {
                        std::vector<unsigned> newMapping(B[j]);
                        auto it = newMapping.end();
                        newMapping.insert(it, Ada[i].begin(), Ada[i].end());
                        bxada.push_back(newMapping);
                }
        }
        CARL_LOG_ASSERT("carl.thom.sign", P.size() == bxada.front().size(), "");
        VectorXf res(bxada.size()); // we already know the length of the resulting vector
        CARL_LOG_TRACE("carl.thom.sign", "in 'compute_dprime': " << bxada.size() << " tarski queries will be computed now");
        for(uint i = 0; i < bxada.size(); i++) {
                Polynomial poly = constantOne(P.front());
                for(uint j = 0; j < P.size(); j++) {
                        if(Settings::SIGN_DETERMINATION_REDUCE) {
                                poly = tarskiQuery.reduceProduct(poly, P[j].pow(bxada[i][j]));
                        }
                        else {
                                poly *= P[j].pow(bxada[i][j]);
                        }
                }
                CARL_LOG_TRACE("carl.thom.sign", "compute tarski query of " << poly);
                int taq = tarskiQuery(poly);
                res(i) = (float)taq;                
        }
        return res;
}

std::vector<SignCondition> calculateR(unsigned which, const std::vector<SignCondition>& sign, const std::vector<SignCondition>& newSign);

std::vector<std::vector<unsigned>> firstNLines(const unsigned n, const MatrixXf& mat, const std::vector<std::vector<unsigned>>& ada);


std::vector<std::vector<unsigned>> adaptedFamily(const std::vector<SignCondition>& sign,                // Sign(Q,Z)
                                                 const std::vector<SignCondition>& newSign,             // Sign({P} u Q, Z)
                                                 const std::vector<std::vector<unsigned>>& ada);        // Ada(Q,Z)



template<typename Polynomial, typename Settings = ThomDefaultSettings>
std::vector<SignCondition> signDetermination(const std::vector<Polynomial>& p, const std::vector<Polynomial>& z) {
        static_assert(is_polynomial<Polynomial>::value, "only call 'signDetermination' on polynomial types");
        CARL_LOG_DEBUG("carl.thom.sign", "p = " << p << ", z = " << z);
        CARL_LOG_ASSERT("carl.thom.sign", isZeroDimensionalSystem(z), ""); // because otherwise the set Zeros(z) is infinite
        CARL_LOG_ASSERT("carl.thom.sign", p.size() > 0, "");
        
        // set up the tarski query manager for this sign determination
        TarskiQueryManager<Polynomial> tarskiQuery(z);
               
        int r = tarskiQuery(1); // this is just the total number of roots of z
        CARL_LOG_ASSERT("carl.thom.sign", r >= 0, "tarski query on constant 1 was negative!!");
        if(r == 0) return std::vector<SignCondition>(); // then z has no roots, so we return an empty list
        
        // main loop
        std::vector<SignCondition> lastSign; // this is an "ordered set" (lexicografically with 0 < 1 < -1)
        std::vector<std::vector<unsigned>> lastAda;
        for(int i = (int)p.size() - 1; i >= 0; i--) {
                CARL_LOG_TRACE("carl.thom.sign", "main loop: i = " << i);
                std::vector<SignCondition> sign; // our first aim in this step is to compute this
                
                int taq1 = tarskiQuery(p[i]);
                int taq2 = tarskiQuery(p[i] * p[i]);
                CARL_LOG_ASSERT("carl.thom.sign", std::abs(taq1) <= r && std::abs(taq2) <= r, "");
                Vector3f t((float)r, (float)taq1, (float)taq2); // right hand side of equation system
                Vector3f c = (Vector3f) (basicMatrixInverse * t);
                
                if(c(0) != 0.0f) sign.push_back(SignCondition(1, Sign::ZERO));
                if(c(1) != 0.0f) sign.push_back(SignCondition(1, Sign::POSITIVE));
                if(c(2) != 0.0f) sign.push_back(SignCondition(1, Sign::NEGATIVE));
                CARL_LOG_TRACE("carl.thom.sign", "sign = " << sign);
                CARL_LOG_ASSERT("carl.thom.sign", sign.size() > 0 && sign.size() <= 3, "");
                
                std::vector<std::vector<unsigned>> B(basicAdaptedList);
                B.resize(sign.size());
                CARL_LOG_ASSERT("carl.thom.sign", B.size() == sign.size(), "");
                MatrixXf M = adaptedMat(B, sign);
                              
                if(i == ((int)p.size()) - 1) {
                        lastSign = sign;
                        lastAda = B;
                        if(i == 0) { // then we are already done
                                return sign;
                        }
                }
                else { // i < p.size()
                        VectorXf d_prime = compute_dprime<Polynomial,Settings>(std::vector<Polynomial>(p.begin() + i, p.end()), B, lastAda, tarskiQuery);
                        MatrixXf M_prime = kroneckerProduct(adaptedMat(lastAda, lastSign), M);
                        // assert square matrix and d_prime has correct length
                        CARL_LOG_ASSERT("carl.thom.sign", M_prime.rows() == M_prime.cols() && M_prime.rows() == d_prime.size(), "");
                        
                        // do the linear algebra stuff
                        CARL_LOG_ASSERT("carl.thom.sign", FullPivLU<MatrixXf>(M_prime).rank() == M_prime.cols(), "M_prime must be invertible!");
                        CARL_LOG_TRACE("carl.thom.sign", "matrix size is " << M_prime.rows() << " x " << M_prime.cols());
                        PartialPivLU<MatrixXf> dec(M_prime);
                        VectorXf c_prime = dec.solve(d_prime);
                        
                        std::vector<SignCondition> newSign;
                        CARL_LOG_ASSERT("carl.thom.sign", (uint)c_prime.size() == sign.size() * lastSign.size(), "");
                        uint k = 0;
                        for(uint i = 0; i < lastSign.size(); i++) {
                                for(uint j = 0; j < sign.size(); j++) {
                                        if((int)std::round(c_prime(k)) != 0) {
                                                newSign.push_back(SignCondition(sign[j].front() * lastSign[i]));
                                        }
                                        k++;
                                }
                        }
                        if(i == 0) { // then we are done!
                                return newSign;
                        }                                   
                        std::vector<std::vector<unsigned>> newAda = adaptedFamily(lastSign, newSign, lastAda);
                        lastSign = newSign;
                        lastAda = newAda;
                }              
        } // end of main loop
        
        // this is to supress a warning - we should never get here
        CARL_LOG_ASSERT("carl.thom.sign", false, "something went wrong in sign determination");
        return std::vector<SignCondition>();     
}

} // namespace carl

