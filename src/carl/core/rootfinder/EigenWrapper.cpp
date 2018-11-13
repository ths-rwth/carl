#include "EigenWrapper.h"

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Eigenvalues>

#include <vector>

namespace carl {
namespace rootfinder {
namespace eigen {

std::vector<double> root_approximation(const std::vector<double>& coeffs) {
	
	using Index = Eigen::MatrixXd::Index;
	// Create companion matrix
	std::size_t degree = coeffs.size() - 1;
	Eigen::MatrixXd m = Eigen::MatrixXd::Zero(Index(degree + 1), Index(degree + 1));
	m(0, Index(degree)) = -coeffs[0] / coeffs[degree];
	for (std::size_t i = 1; i <= degree; ++i) {
		m(Index(i), Index(i)-1) = 1;
		m(Index(i), Index(degree)) = -coeffs[i] / coeffs[degree];
	}
	
	// Obtain eigenvalues
	Eigen::VectorXcd eigenvalues = m.eigenvalues();
	
	// Save real parts to tmp
	std::vector<double> tmp(std::size_t(eigenvalues.size()));
	for (uint i = 0; i < std::size_t(eigenvalues.size()); ++i) {
		if (eigenvalues[Index(i)].imag() > eigenvalues[Index(i)].real() / 4) tmp[i] = 0;
		else tmp[i] = eigenvalues[Index(i)].real();
	}
	return tmp;
}

}
}
}
