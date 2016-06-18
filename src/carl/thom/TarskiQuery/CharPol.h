/* 
 * File:   CharPol.h
 * Author: tobias
 *
 * Created on 13. Juni 2016, 14:34
 */

#pragma once

#include <Eigen/Dense>
#include <vector>
#include <cmath>


namespace carl {

// allgorithm 8.11, p. 290
template<typename Coeff>
std::vector<Coeff> newtonSums(const std::vector<Coeff>& newtonSums) {
        assert(newtonSums.size() >= 1);
        int p = (int)newtonSums.size() - 1;
        std::vector<Coeff> res(p + 1);
        res[p] = Coeff(1);
        for(int i = 1; i <= p; i++) {
                Coeff sum(0);
                for(int j = 1; j <= i; j++) {
                        sum += res[p - i + j] * newtonSums[j];
                        
                }
                sum *= -(Coeff(1) / Coeff(i));
                res[p-i] = sum;
        }
        return res;      
}

template<typename Coeff>
using CoeffMatrix = Eigen::Matrix<Coeff, Eigen::Dynamic, Eigen::Dynamic>;

template<typename Coeff>
void printMatrix(const CoeffMatrix<Coeff>& m) {
        for(int i = 0; i < m.rows(); i++) {
                for(int j = 0; j < m.cols(); j++) {
                        std::cout << m(i, j) << "\t";
                }
                std::cout << std::endl;
        }
}

// algorithm 8.17, p. 300
template<typename Coeff>
std::vector<Coeff> charPol(const CoeffMatrix<Coeff>& m) {
        long int n = m.cols();
        assert(n == m.rows());
        
        // calculate r
        int r = static_cast<int>(std::ceil(std::sqrt(static_cast<double>(n)))); 
        if(r*r == n) r++; // if n is a square number
        std::cout << "n = " << n << std::endl;
        std::cout << "r = " << r << std::endl;
        
        CoeffMatrix<Coeff> id = CoeffMatrix<Coeff>::Identity(n, n);
        
        std::vector<CoeffMatrix<Coeff>> B;
        std::vector<Coeff> N((r * r), Coeff(0));
        B.push_back(id);
        N[0] = static_cast<Coeff>(n);
        for(int i = 0; i < r - 1; i++) {
                B.push_back(m * B.back());
                N[i+1] = B.back().trace();
                printMatrix(B.back());
        }
        std::cout << N << std::endl;
        
        std::vector<CoeffMatrix<Coeff>> C;
        C.push_back(m * B.back());
        N[r] = C.back().trace();
        for(int j = 1; j < r - 1; j++) {
                C.push_back(C.front() * C.back());
                N[(j + 1) * r] = C.back().trace();
                printMatrix(C.back());
        }
        std::cout << N << std::endl;
        
        for(int i = 1; i < r; i++) {
                for(int j = 1; j < r; j++) {
                        CoeffMatrix<Coeff> tmp(B[i] * C[j-1]);
                        N[j*r + i] = tmp.trace();
                }
        }
        std::cout << N << std::endl;
        N.resize(n + 1);
        std::cout << N << std::endl;
        
        return newtonSums(N);
        
}

} // namespace carl
