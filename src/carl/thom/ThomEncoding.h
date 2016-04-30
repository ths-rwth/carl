/* 
 * File:   ThomEncoding.h
 * Author: tobias
 *
 * Created on 24. April 2016, 20:04
 */

#pragma once

#include <Eigen/Dense>
#include <cmath>

#include "../core/UnivariatePolynomial.h"
#include "../core/Sign.h"
#include "TarskiQuery.h"

namespace carl {

using namespace Eigen;

typedef std::vector<Sign> SignCondition; // a list of sign conditions that a list of polynomials realizes at a point
                                         // we also view this as a mapping from the polynomials to a Sign


// todo make this a global (or static) constant and give it a better name
Matrix3f getStandardMatrixInverse() {
        Matrix3f res;
        res << 1, 1, 1,
               0, 1, -1,
               0, 1, 1;
        return res.inverse();
}

float sigmaToTheAlpha(const std::vector<unsigned>& alpha, const SignCondition& sigma) {
        assert(alpha.size() == sigma.size());
        float res = 1.0f;
        for(unsigned i = 0; i < alpha.size(); i++) {
                res *= (float)std::pow((float)sigma[i], alpha[i]);
        }
        return res;
}


MatrixXf adaptedMat(const std::vector<std::vector<unsigned>>& A, const std::vector<SignCondition>& sign) {
        MatrixXf res(A.size(), sign.size());
        for(unsigned i = 0; i < A.size(); i++) {
                for(unsigned j = 0; j < sign.size(); j++) {
                        res(i, j) = sigmaToTheAlpha(A[i], sign[j]);
                }
        }
        return res;
}

#define LINE std::cout << "-----------------------------------------------" << std::endl;
#define PRINT(x) std::cout << #x << " = " << x << std::endl;
#define PRINT2(x) std::cout << #x << " = " << std::endl << x << std::endl;

// https://forum.kde.org/viewtopic.php?f=74&t=50952
MatrixXf kroneckerProduct(const MatrixXf& m1, const MatrixXf& m2) {
        MatrixXf m3(m1.rows() * m2.rows(), m1.cols() * m2.cols());
        for (unsigned i = 0; i < m1.cols(); i++) {
                for (unsigned j = 0; j < m1.rows(); j++) {
                        m3.block(i * m2.rows(), j * m2.cols(), m2.rows(), m2.cols()) =  m1(i, j) * m2;
                }
        }
        return m3;
}

const Matrix3f mmm = getStandardMatrixInverse();

template<typename Coeff>
VectorXf compute_dprime(const std::vector<UnivariatePolynomial<Coeff>>& P, 
                        const std::vector<std::vector<unsigned>>& B, 
                        const std::vector<std::vector<unsigned>>& Ada,
                        const UnivariatePolynomial<Coeff>& z) {
        // asserts
        assert(B.front().size() == 1); // must hold for all vectors in B
        assert(P.size() == Ada.front().size() + 1);
        
        std::cout << "Input to compute_dprime:" << std::endl;
        PRINT(P); PRINT(B); PRINT(Ada);
        
        // first calculate the list of mappings B x Ada
        std::vector<std::vector<unsigned>> bxada;
        for(unsigned i = 0; i < Ada.size(); i++) {
                for(unsigned j = 0; j < B.size(); j++) {
                        std::vector<unsigned> newMapping(B[j]);
                        auto it = newMapping.end();
                        newMapping.insert(it, Ada[i].begin(), Ada[i].end());
                        bxada.push_back(newMapping);
                }
        }
        PRINT(bxada);
        assert(P.size() == bxada.front().size());
        VectorXf res(bxada.size()); // we already know the length of the resulting vector
        for(unsigned i = 0; i < bxada.size(); i++) {
                // evaluate the i-th element of bxada on P
                UnivariatePolynomial<Coeff> poly = P.front().one();
                for(unsigned j = 0; j < P.size(); j++) {
                        poly *= P[j].pow(bxada[i][j]);                       
                }
                PRINT(poly);
                int taq = tarskiQuery(poly, z);
                //std::cout << "taq = " << taq << std::endl;
                res(i) = (float)taq;
                 
        }
        PRINT2(res);
        LINE;
        return res;
}

// states that tau extends sigma (see Algorithms for RAG, p.387)
bool extends(const SignCondition& tau, const SignCondition& sigma) {
        assert(tau.size() == sigma.size() +1);
        for(unsigned i = 1; i < tau.size(); i++) {
                if(tau[i] != sigma[i-1]) return false;
        }
        std::cout << "EXTENDS: " << tau << " extends " << sigma << std::endl;
        return true;
}

std::vector<SignCondition> calculateR(unsigned which, const std::vector<SignCondition>& sign, const std::vector<SignCondition>& newSign) {
        assert(which == 2 || which == 3);
        std::vector<SignCondition> res;
        for(const SignCondition& sigma : sign) {
                unsigned count = 0;
                for(const SignCondition& tau : newSign) {
                        if(extends(tau, sigma)) {
                                count++;
                                // TODO: early break
                        }
                }
                if(count >= which) {
                        res.push_back(sigma);
                }
        }
        return res;
}

std::vector<std::vector<unsigned>> firstNLines(const unsigned n, const MatrixXf& mat) {
        assert(n > 0);
        // first n lines as a matrix
        MatrixXf firstLines(n, mat.cols());
        for(unsigned i = 0; i < n; i++) {
                for(unsigned j = 0; j < mat.cols(); j++) {
                        firstLines(i, j) = mat(i, j);
                }
        }
        PRINT2(firstLines);
        FullPivLU<MatrixXf> dec(firstLines);
        PRINT(dec.rank());
        assert(dec.rank() == n);
        std::vector<std::vector<unsigned>> res;
        res.reserve(firstLines.rows());
        for(unsigned i = 0; i < firstLines.rows(); i++) {
                std::vector<unsigned> tmp;
                tmp.reserve(firstLines.cols());
                for(unsigned j = 0; j < firstLines.cols(); j++) {
                        tmp.push_back((unsigned)firstLines(i, j));
                }
                res.push_back(tmp);
        }
        return res;
}


std::vector<std::vector<unsigned>> adaptedFamily(const std::vector<SignCondition>& sign,                // Sign(Q,Z)
                                                 const std::vector<SignCondition>& newSign,             // Sign({P} u Q, Z)
                                                 const std::vector<std::vector<unsigned>>& ada) {       // Ada(Q, Z)
        assert(sign.front().size() > 0);
        // TODO implement for the case where Q is empty
        unsigned r_1 = (unsigned)sign.size();
        PRINT(r_1);
        std::vector<std::vector<unsigned>> res(ada);
        for(auto& a : res) {
                a.insert(a.begin(), 0); // NOT SURE WHICH ONE!!
                //a.push_back(0);
        }
        
        if(newSign.size() != r_1) {
                // this means r_2 != 0
                // compute r_2
                std::vector<SignCondition> sign2 = calculateR(2, sign, newSign);
                unsigned r_2 = (unsigned)sign2.size();
                PRINT(sign2);
                PRINT(r_2);
                MatrixXf mat2 = adaptedMat(ada, sign2);
                PRINT2(mat2);
                firstNLines(r_2, mat2);
                std::vector<std::vector<unsigned>> A_2(ada.begin(), ada.begin() + r_2);
                for(const auto& a : A_2) {
                        std::vector<unsigned> tmp(a);
                        tmp.insert(tmp.begin(), 1);
                        res.push_back(tmp); // NOT SURE WHICH ONE!!
                        //a.push_back(0);
                }
                PRINT(res);
                if((unsigned)newSign.size() != r_1 + r_2) {
                        // then also calculate r_3
                        
                }
                
        }
        
        
        return res;
        
}



template<typename Coeff>
void signDetermination(const std::vector<UnivariatePolynomial<Coeff>>& p, const UnivariatePolynomial<Coeff>& z) {
        assert(!z.isZero()); // because otherwise the set Zeros(z) is infinite
        const unsigned s = (unsigned)p.size();
        assert(s > 0);
        
        int r = tarskiQuery(p.front().one(), z); // this is just the total number of roots of z
        PRINT(r);
        if(r == 0) return; // then z has no roots, so there are no realisable sign conditions on the roots of z
        LINE;
        
        // main loop
        std::vector<SignCondition> lastSign; // this is an "ordered set" (lexicografically with 0 < 1 < -1)
        std::vector<std::vector<unsigned>> lastAda;
        for(unsigned i = s; i >= 1; i--) {
                std::cout << "i = " << i << std::endl;
                LINE;
                std::vector<SignCondition> sign; // our first aim in this step is to compute this
                
                int taq0 = r;
                int taq1 = tarskiQuery(p[i-1], z);
                int taq2 = tarskiQuery(p[i-1] * p[i-1], z);
                Vector3f t((float)taq0, (float)taq1, (float)taq2); // right hand side of equation system
                Vector3f c = (Vector3f) (mmm * t);
                PRINT2(c);
                
                if(c(0) != 0.0f) sign.push_back(SignCondition(1, Sign::ZERO));
                if(c(1) != 0.0f) sign.push_back(SignCondition(1, Sign::POSITIVE));
                if(c(2) != 0.0f) sign.push_back(SignCondition(1, Sign::NEGATIVE));
                std::cout << "sign = " << sign << std::endl;
                assert(sign.size() > 0 && sign.size() <= 3);
                
                std::vector<std::vector<unsigned>> B;
                B.push_back(std::vector<unsigned>(1, 0));
                if(sign.size() > 1) {
                        B.push_back(std::vector<unsigned>(1, 1));
                        if(sign.size() > 2) {
                                B.push_back(std::vector<unsigned>(1, 2));
                        }
                }
                assert(B.size() == sign.size());
                std::cout << "B = " << B << std::endl;
                MatrixXf M = adaptedMat(B, sign);
                std::cout << "M = " << std::endl << M << std::endl;
                
                LINE;
                
                if(i == s) {
                        lastSign = sign;
                        lastAda = B;
                }
                else { // i < s
                        VectorXf d_prime = compute_dprime(std::vector<UnivariatePolynomial<Coeff>>(p.begin() + i - 1, p.end()), B, lastAda, z);
                        MatrixXf M_prime = kroneckerProduct(adaptedMat(lastAda, lastSign), M);
                        PRINT2(M_prime);
                        assert(M_prime.rows() == M_prime.cols() && M_prime.rows() == d_prime.size());
                        
                        // do the linear algebra stuff
                        PartialPivLU<MatrixXf> dec(M_prime);
                        VectorXf c_prime = dec.solve(d_prime);
                        PRINT2(c_prime);
                        
                        std::vector<SignCondition> newSign;
                        assert(c_prime.size() == sign.size() * lastSign.size());
                        unsigned k = 0;
                        for(unsigned i = 0; i < lastSign.size(); i++) {
                                for(unsigned j = 0; j < sign.size(); j++) {
                                       if((int)c_prime(k) != 0) {
                                               SignCondition newCond(sign[j]);
                                               newCond.insert(newCond.end(), lastSign[i].begin(), lastSign[i].end());
                                               newSign.push_back(newCond);
                                       }
                                       k++;
                                }
                        }
                        PRINT(newSign);
                        if(i == 1) { // then we are done!
                                //return;
                        }
                        
                        LINE;
                        std::cout << "compute the ADAPTED FAMILY now ... inputs to this are:" << std::endl;
                        PRINT(lastSign);
                        PRINT(newSign);
                        PRINT(lastAda);
                        
                        std::vector<std::vector<unsigned>> newAda = adaptedFamily(lastSign, newSign, lastAda);
                        PRINT(newAda);
                        
                        LINE;
                        
                        lastSign = newSign;
                        lastAda = newAda;
                        
                        LINE;
                }
                
                
        } // end of main loop
        
}


} // namespace carl




