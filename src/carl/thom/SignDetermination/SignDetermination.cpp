/* 
 * File:   SignDetermination.tpp
 * Author: tobias
 *
 * Created on 17. Juli 2016, 23:04
 */

#include "SignDetermination.h"

namespace carl {
    
float sigmaToTheAlpha(const std::vector<unsigned>& alpha, const SignCondition& sigma) {
        CARL_LOG_ASSERT("carl.thom.sign", alpha.size() == sigma.size(), "invalid arguments to call 'sigmaToTheAlpha'");
        int res = 1;
        for(uint i = 0; i < alpha.size(); i++) {
                // there are only these two cases where res can actually change
                if(sigma[i] == Sign::NEGATIVE && alpha[i] == 1) {
                        res = -res;
                }
                else if(sigma[i] == Sign::ZERO && alpha[i] != 0) {
                        return 0.0f;
                }
        }
        return (float)res;
}


MatrixXf adaptedMat(const std::vector<std::vector<unsigned>>& A, const std::vector<SignCondition>& sign) {
        MatrixXf res(A.size(), sign.size());
        for(uint i = 0; i < A.size(); i++) {
                for(uint j = 0; j < sign.size(); j++) {
                        res(i, j) = sigmaToTheAlpha(A[i], sign[j]);
                }
        }
        return res;
}



// https://forum.kde.org/viewtopic.php?f=74&t=50952
MatrixXf kroneckerProduct(const MatrixXf& m1, const MatrixXf& m2) {
        MatrixXf m3(m1.rows() * m2.rows(), m1.cols() * m2.cols());
        for(int i = 0; i < m1.cols(); i++) {
                for(int j = 0; j < m1.rows(); j++) {
                        m3.block(i * m2.rows(), j * m2.cols(), m2.rows(), m2.cols()) =  m1(i, j) * m2;
                }
        }
        return m3;
}

std::vector<SignCondition> calculateR(unsigned which, const std::vector<SignCondition>& sign, const std::vector<SignCondition>& newSign) {
        CARL_LOG_ASSERT("carl.thom.sign", which == 2 || which == 3, "");
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

std::vector<std::vector<unsigned>> firstNLines(const unsigned n, const MatrixXf& mat, const std::vector<std::vector<unsigned>>& ada) {
        CARL_LOG_ASSERT("carl.thom.sign", n > 0, "");
        // first n lines as a matrix
        MatrixXf firstLines(n, mat.cols());
        for(unsigned i = 0; i < n; i++) {
                for(unsigned j = 0; j < mat.cols(); j++) {
                        firstLines(i, j) = mat(i, j);
                }
        }
        FullPivLU<MatrixXf> dec(firstLines);
        // not sure if it is meant to be that the first n lines are always linearly independent
        // or we have to find the "first" lin. ind. n lines.
        CARL_LOG_ASSERT("carl.thom.sign", dec.rank() == n, "");
        std::vector<std::vector<unsigned>> res(ada); // this is just the first n elements of ada
        res.resize(n);
        return res;
}


std::vector<std::vector<unsigned>> adaptedFamily(const std::vector<SignCondition>& sign,                // Sign(Q,Z)
                                                 const std::vector<SignCondition>& newSign,             // Sign({P} u Q, Z)
                                                 const std::vector<std::vector<unsigned>>& ada) {       // Ada(Q, Z)
        CARL_LOG_FUNC("carl.thom.sign", "sign = " << sign << ", newSign = " << newSign << ", ada = " << ada);
        CARL_LOG_ASSERT("carl.thom.sign", sign.front().size() > 0, "");
        // TODO implement for the case where Q is empty
        unsigned r_1 = (unsigned)sign.size();
        std::vector<std::vector<unsigned>> res(ada);
        for(auto& a : res) {
                a.insert(a.begin(), 0); // NOT SURE WHICH ONE!!
                //a.push_back(0);
        }
        
        if(newSign.size() != r_1) {
                // this means r_2 != 0
                // compute r_2
                CARL_LOG_TRACE("carl.thom.sign", "need to compute r_2");
                std::vector<SignCondition> sign2 = calculateR(2, sign, newSign);
                unsigned r_2 = (unsigned)sign2.size();
                MatrixXf mat2 = adaptedMat(ada, sign2);
                std::vector<std::vector<unsigned>> A_2 = firstNLines(r_2, mat2, ada);
                for(const std::vector<unsigned>& a : A_2) {
                        res.push_back((unsigned)1 * a);
                }
                if((unsigned)newSign.size() != r_1 + r_2) {
                        CARL_LOG_TRACE("carl.thom.sign", "need to compute r_3");
                        std::vector<SignCondition> sign3 = calculateR(3, sign, newSign);
                        unsigned r_3 = (unsigned)sign3.size();
                        std::vector<std::vector<unsigned>> A_3 = firstNLines(r_3, mat2, ada);
                        for(const auto& a : A_3) {
                                res.push_back((unsigned)2 * a);
                        }
                }       
        }       
        return res;      
}
    
    
} // namespace carl


