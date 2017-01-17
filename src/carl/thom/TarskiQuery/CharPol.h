/* 
 * File:   CharPol.h
 * Author: tobias
 *
 * Created on 13. Juni 2016, 14:34
 */

#pragma once

#include <eigen3/Eigen/Dense>
#include <cmath>
#include <vector>

namespace carl {

// allgorithm 8.11, p. 290
template<typename Coeff>
std::vector<Coeff> newtonSums(const std::vector<Coeff>& newtonSums) {
	CARL_LOG_FUNC("carl.thom.tarski", "newtonSums = " << newtonSums);
	CARL_LOG_ASSERT("carl.thom.tarski", newtonSums.size() >= 1, "newtonSums: invalid argument");
	assert(newtonSums.size() > 0);
	std::size_t p = newtonSums.size() - 1;
	std::vector<Coeff> res(p + 1);
	res[p] = Coeff(1);
	for(std::size_t i = 1; i <= p; i++) {
		Coeff sum(0);
		for (std::size_t j = 1; j <= i; j++) {
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
	for(std::size_t i = 0; i < m.rows(); i++) {
		for(std::size_t j = 0; j < m.cols(); j++) {
			std::cout << m(i, j) << "\t";
		}
		std::cout << std::endl;
	}
}

// algorithm 8.17, p. 300
template<typename Coeff>
std::vector<Coeff> charPol(const CoeffMatrix<Coeff>& m) {
	CARL_LOG_FUNC("carl.thom.tarski", "");
	long n = m.cols();
	CARL_LOG_ASSERT("carl.thom.tarski", n == m.rows(), "can only compute characteristic polynomial of square matrix");
	CARL_LOG_INFO("carl.thom.tarski", "input has size " << n << "x" << n);
	
	// calculate r
	std::size_t r = static_cast<std::size_t>(std::ceil(std::sqrt(n)));
	if (r*r == std::size_t(n)) r++; // if n is a square number
	
	CoeffMatrix<Coeff> id = CoeffMatrix<Coeff>::Identity(n, n);
	
	std::vector<CoeffMatrix<Coeff>> B;
	std::vector<Coeff> N((r * r), Coeff(0));
	B.push_back(id);
	N[0] = static_cast<Coeff>(n);
	for(std::size_t i = 0; i < r - 1; i++) {
		B.push_back(m * B.back());
		N[i+1] = B.back().trace();
		//printMatrix(B.back());
	}
	
	std::vector<CoeffMatrix<Coeff>> C;
	C.push_back(m * B.back());
	N[r] = C.back().trace();
	for(std::size_t j = 1; j < r - 1; j++) {
		C.push_back(C.front() * C.back());
		N[(j + 1) * r] = C.back().trace();
		//printMatrix(C.back());
	}
	
	for(std::size_t i = 1; i < r; i++) {
		for(std::size_t j = 1; j < r; j++) {
			CoeffMatrix<Coeff> tmp(B[i] * C[j-1]);
			N[j*r + i] = tmp.trace();
		}
	}
	N.resize(std::size_t(n) + 1);
	std::vector<Coeff> res =  newtonSums(N);
	CARL_LOG_INFO("carl.thom.tarski", "done computing the char pol ... ");
	return res;
}

} // namespace carl
