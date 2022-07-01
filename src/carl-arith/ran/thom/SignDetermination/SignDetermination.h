/* 
 * File:   SignDetermination.h
 * Author: tobias
 *
 * Created on 15. August 2016, 11:38
 */

#pragma once

#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>
#include "../TarskiQuery/TarskiQueryManager.h"
#include "SignCondition.h"

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/LU>
#include <cmath>
#include <iterator>
#include <list>
#include <queue>


namespace carl {

template<typename Number>
class SignDetermination {
	
private:
	using Polynomial = MultivariatePolynomial<Number>;
	using TaQResType = typename TarskiQueryManager<Number>::QueryResultType;
	
	// an alpha is a mapping from a set of polynomials to the set
	// {0,1,2} in order to perform sign determination
	using Alpha = std::list<uint>;
	
	std::list<Polynomial> mP;
	TarskiQueryManager<Number> mTaQ;
	std::list<SignCondition> mSigns;
	std::list<Polynomial> mProducts;
	std::list<Alpha> mAda;
	std::list<uint> mAdaHelper;
	Eigen::MatrixXf mMatrix;
	bool mNeedsUpdate = false;
	
	
	
public:
	template<typename InputIt>
	SignDetermination(InputIt zeroSet_first, InputIt zeroSet_last) :
		mTaQ(zeroSet_first, zeroSet_last)
	{}

	
	/*
	 * Copy constructor
	 */
	SignDetermination(const SignDetermination& other) :
		mP(other.mP),
		mTaQ(other.mTaQ),
		mSigns(other.mSigns),
		mProducts(other.mProducts),
		mAda(other.mAda),
		mAdaHelper(other.mAdaHelper),
		mMatrix(other.mMatrix),
		mNeedsUpdate(other.mNeedsUpdate)
	{}
	
	uint sizeOfZeroSet() const {
		TaQResType res = mTaQ(Number(1));
		CARL_LOG_ASSERT("carl.thom.sing", res >= 0, "");
		return uint(res);
	}
	
	const auto& processedPolynomials() const { return mP; }
	const auto& signs() const { return mSigns; }
	const auto& products() const { return mProducts; }
	const auto& adaptedList() const { return mAda; }
	const auto& matrix() const { return mMatrix; }
	bool needsUpdate() const { return mNeedsUpdate; }
	
	
private:
	static int sigmaToTheAlpha(const Alpha& alpha, const SignCondition& sigma) {
		CARL_LOG_ASSERT("carl.thom.sign", alpha.size() == sigma.size(), "invalid arguments");
		int res = 1;
		auto it_alpha = alpha.begin();
		auto it_sigma = sigma.begin();
		for(; it_alpha != alpha.end(); ) {
			// there are only these two cases where res can actually change
			if(*it_sigma == Sign::NEGATIVE && *it_alpha == 1) {
				res = -res;
			}
			else if(*it_sigma == Sign::ZERO && *it_alpha != 0) {
				return 0;
			}
			it_alpha++;
			it_sigma++;
		}
		return res;
	}       
	static Eigen::MatrixXf adaptedMat(const std::list<Alpha>& ada, const std::list<SignCondition>& signs) {
		Eigen::MatrixXf res(ada.size(), signs.size());
		Eigen::Index i = 0;
		Eigen::Index j = 0;
		for (const auto& alpha : ada) {
			for (const auto& sigma : signs) {
				res(i, j) = float(sigmaToTheAlpha(alpha, sigma));
				j++;
			}
			j = 0;
			i++;
		}
		return res;
	} 
	static Eigen::MatrixXf kroneckerProduct(const Eigen::MatrixXf& m1, const Eigen::MatrixXf& m2) {
		Eigen::MatrixXf m3(m1.rows() * m2.rows(), m1.cols() * m2.cols());
		for(int i = 0; i < m1.cols(); i++) {
			for(int j = 0; j < m1.rows(); j++) {
				m3.block(i * m2.rows(), j * m2.cols(), m2.rows(), m2.cols()) =  m1(i, j) * m2;
			}
		}
		return m3;
	}
	static void removeColumn(Eigen::MatrixXf& matrix, Eigen::Index colToRemove) {
		Eigen::Index numRows = matrix.rows();
		Eigen::Index numCols = matrix.cols()-1;

		if (colToRemove < numCols) {
			matrix.block(0, colToRemove, numRows, numCols - colToRemove) = matrix.block(0, colToRemove + 1 ,numRows, numCols - colToRemove);
		}
		matrix.conservativeResize(numRows, numCols);
	} 
	static void removeRow(Eigen::MatrixXf& matrix, Eigen::Index rowToRemove) {
		Eigen::Index numRows = matrix.rows()-1;
		Eigen::Index numCols = matrix.cols();

		if (rowToRemove < numRows) {
			matrix.block(rowToRemove,0,numRows-rowToRemove,numCols) = matrix.block(rowToRemove+1,0,numRows-rowToRemove,numCols);
		}

		matrix.conservativeResize(numRows,numCols);
	}
	
	std::list<Polynomial> computeProducts(const Polynomial& p, const std::list<Alpha>& currAda) const {
		std::list<Polynomial> result;
		for(const auto& alpha : currAda) {
			for(const auto& prod : mProducts) {
				assert(alpha.size() == 1);
				if(alpha.front() == 0) {
					result.push_back(prod);
				}
				else if(alpha.front() == 1) {
					result.push_back(mTaQ.reduceProduct(p, prod));
				}
				else {
					assert(alpha.front() == 2);
					Polynomial psquare = mTaQ.reduceProduct(p, p);
					result.push_back(mTaQ.reduceProduct(psquare, prod));
				}
			}
		}
		return result;
	}
	
	std::list<Alpha> firstNLines(
			const uint n,
			const Eigen::MatrixXf& mat,
			const std::vector<Alpha>& ada,
			std::vector<Polynomial>& products,
			const uint q) const {
		CARL_LOG_ASSERT("carl.thom.sign", n > 0, "");
		std::vector<uint> lines(ada.size(), 0);
		assert(n <= ada.size());
		assert((uint)mat.rows() == ada.size());
		for (std::size_t i = 0; i < n; i++) {
			lines[i] = 1;
		}

		do {
			Eigen::MatrixXf linesMat(n, mat.cols());
			Eigen::Index row = 0;
			for(std::size_t i = 0; i < lines.size(); i++) {
				if (lines[i] == 0) continue;
				for (Eigen::Index j = 0; j < mat.cols(); j++) {
					linesMat(row, j) = mat(Eigen::Index(i), j);
				}
				row++;
			}
			Eigen::FullPivLU<Eigen::MatrixXf> dec(linesMat);
			if (dec.rank() == int(n)) break;

		}
		while(std::prev_permutation(lines.begin(), lines.end()));


		std::list<Alpha> res;
		std::vector<Polynomial> newProducts;
		for(uint i = 0; i < lines.size(); i++) {
			if(lines[i] == 1) {
				res.push_back(ada[i]);
				newProducts.push_back(products[q*ada.size() + i]);
			}
		}
		assert(res.size() == n);
		products = newProducts;
		return res;
	}
       
	void update() {
		std::list<Alpha> newAda = mAda;
		for(auto& alpha : newAda) alpha.push_front(0);
		std::list<Polynomial> adaptedProducts = mProducts;
		adaptedProducts.resize(mAda.size());
		uint r1 = mAda.size();
		if(mSigns.size() != r1) {
			CARL_LOG_TRACE("carl.thom.sign", "need to compute r2");
			Eigen::MatrixXf m2 = mMatrix;
			uint r2 = 0;
			long index = 0;
			for(const auto& n : mAdaHelper) {
				if(n >= 2) {
					r2++;
					index++;
				}
				else {
					removeColumn(m2, index);
				}    
			}
			std::vector<Polynomial> products(mProducts.begin(), mProducts.end());
			std::list<Alpha> A_2 = firstNLines(
				r2, 
				m2, 
				std::vector<Alpha>(mAda.begin(), mAda.end()), 
				products, 
				1);
			for(auto& alpha : A_2) {
				alpha.push_front(1);
				newAda.push_back(alpha);
			}
			for(const auto& p : products) {
				adaptedProducts.push_back(p);
			}
			if(mSigns.size() != r1 + r2) {
				CARL_LOG_TRACE("carl.thom.sign", "need to compute r3");
				Eigen::MatrixXf m3 = mMatrix;
				uint r3 = 0;
				index = 0;
				for(const auto& n : mAdaHelper) {
					if(n >= 3) {
						r3++;
						index++;
					}
					else {
						removeColumn(m3, index);
					}      
				}
				products = std::vector<Polynomial>(mProducts.begin(), mProducts.end());
				std::list<Alpha> A_3 = firstNLines(
					r3,
					m3,
					std::vector<Alpha>(mAda.begin(), mAda.end()),
					products,
					2);
				for(auto& alpha : A_3) {
					alpha.push_front(2);
					newAda.push_back(alpha);
				}
				for(const auto& p : products) {
					adaptedProducts.push_back(p);
				}
			}	 
		}
		mAda = newAda;
		mMatrix = adaptedMat(mAda, mSigns);
		CARL_LOG_ASSERT("carl.thom.sign", Eigen::FullPivLU<Eigen::MatrixXf>(mMatrix).rank() == mMatrix.cols(), "mMatrix must be invertible!");
		mProducts = adaptedProducts;
		mNeedsUpdate = false;
		CARL_LOG_DEBUG("carl.thom.sign", *this);
	}
	
	std::list<SignCondition> getSigns (
			const Polynomial& p,
			std::list<Polynomial>& products,
			std::list<Alpha>& ada,
			std::list<uint>& adaHelper,
			Eigen::MatrixXf& matrix
	) {
		if(mNeedsUpdate) this->update();
		
		CARL_LOG_TRACE("carl.thom.sign", "processing " << p);
		std::list<Polynomial> currProducts;
		TaQResType taq0 = mTaQ(Number(1));
		currProducts.push_back(Polynomial(Number(1)));
		if(taq0 == 0) {
			CARL_LOG_WARN("carl.thom.sign", "doing sign determination on an empty zero set");
			return {};
		}
		
		// (1) determine sign conditions realized by p on the zero set
		//     and an corrspoding adapted list
		std::list<SignCondition> currSigns;
		std::list<Alpha> currAda;
		TaQResType taq1 = mTaQ(p);
		currProducts.push_back(p);
		Polynomial psquare = p*p;
		TaQResType taq2 = mTaQ(psquare);
		currProducts.push_back(psquare);
		CARL_LOG_ASSERT("carl.thom.sign",  std::abs(taq1) <= taq0 && std::abs(taq2) <= taq0, "tarski query failure");
		int czer = taq0 - taq2;
		int cpos = (taq1 + taq2) / 2; // ensured to be an exact division
		int cneg = (taq2 - taq1) / 2;
		// the order in which elements are added to currSigns is important
		if(czer != 0) currSigns.emplace_back(1, Sign::ZERO);
		if(cpos != 0) currSigns.emplace_back(1, Sign::POSITIVE);
		if(cneg != 0) currSigns.emplace_back(1, Sign::NEGATIVE);
		currAda = {{0}, {1}, {2}};
		currAda.resize(currSigns.size());
		currProducts.resize(currSigns.size());     
		Eigen::MatrixXf currM = adaptedMat(currAda, currSigns);
		// means that p is the first polynomial ever processed in this sign determination
		if(mP.empty()) {
			products = currProducts;
			ada = currAda;
			matrix = currM;
			mNeedsUpdate = false;
			return currSigns;
		}
		
		// (2)
		products = this->computeProducts(p, currAda);
		
		Eigen::VectorXf dprime(long(products.size()));
		long index = 0;
		for (const auto& prod : products) {
			dprime(index) = float(mTaQ(prod));
			index++;
		}
		
		Eigen::MatrixXf M_prime = kroneckerProduct(currM, mMatrix);
		CARL_LOG_ASSERT("carl.thom.sign", Eigen::FullPivLU<Eigen::MatrixXf>(M_prime).rank() == M_prime.cols(), "M_prime must be invertible!");
		Eigen::PartialPivLU<Eigen::MatrixXf> dec(M_prime);
		Eigen::VectorXf c = dec.solve(dprime);
		CARL_LOG_ASSERT("carl.thom.sign", (uint)c.size() == currSigns.size() * mSigns.size(), "failure in sign determination");
		
		std::list<SignCondition> newSigns;
		adaHelper = std::list<uint>(mSigns.size(), 0);
		long k = 0;
		for(long i = 0; i < long(currSigns.size()); i++) {
			auto helper_it = adaHelper.begin();
			for(const auto& sigma : mSigns) {
				if ((std::round(c(k))) != 0) {
					uint tmp = *helper_it;
					helper_it = adaHelper.erase(helper_it);
					helper_it = adaHelper.insert(helper_it, ++tmp);
					SignCondition newCond = sigma;
					auto it = currSigns.begin();
					std::advance(it, i);
					newCond.push_front(it->front());
					newSigns.push_back(newCond);
				}
				helper_it++;
				k++;
			}
			helper_it = adaHelper.begin();
		}
		
		return newSigns;
	}
	
	
public:
	
	/*
	 * MAIN INTERFACES
	 */
	std::list<SignCondition> getSigns(const Polynomial& p) {
		std::list<Polynomial> dummyProducts;
		std::list<Alpha> dummyAda;
		std::list<uint> dummyHelper;
		Eigen::MatrixXf dummyMatrix;
		std::list<SignCondition> newSigns = getSigns(p, dummyProducts, dummyAda, dummyHelper, dummyMatrix);
		return newSigns;
	}
	
	std::list<SignCondition> getSignsAndAdd(const Polynomial& p) {
		std::list<Polynomial> newProducts;
		std::list<Alpha> newAda;
		std::list<uint> newHelper;
		Eigen::MatrixXf newMatrix;
		std::list<SignCondition> newSigns = getSigns(p, newProducts, newAda, newHelper, newMatrix);
		mNeedsUpdate = true;
		if(mP.empty()) {
			mAda = newAda;
			mMatrix = newMatrix;
			mNeedsUpdate = false;
		}
		mP.push_front(p);
		mSigns = newSigns;
		mProducts = newProducts;
		mAdaHelper = newHelper;
		CARL_LOG_DEBUG("carl.thom.sign", *this);
		return newSigns;
	}
	
	template<typename InputIt>
	std::list<SignCondition> getSignsAndAddAll(InputIt first, InputIt last) {
		for(; first != last; first++) getSignsAndAdd(*first);
		return mSigns;
	}
}; // class SignDetermination

template<typename N>
std::ostream& operator<<(std::ostream& os, const SignDetermination<N>& rhs) {
	os << "\n---------------------------------------------------------------------" << std::endl;
	os << "Status of sign determination object:" << std::endl;
	os << "---------------------------------------------------------------------" << std::endl;
	os << "processed polynomials:\t\t" << rhs.processedPolynomials() << std::endl;
	os << "sign conditions:\t\t" << rhs.signs() << std::endl;
	os << "adapted list:\t\t\t" << rhs.adaptedList() << std::endl;
	os << "products:\t\t\t" << rhs.products() << std::endl;
	os << "matrix dimension:\t\t" << rhs.matrix().rows() << " x " << rhs.matrix().cols() << std::endl;
	os << "needs update:\t\t\t" << rhs.needsUpdate() << std::endl;
	os << "---------------------------------------------------------------------" << std::endl;
	return os;
}
	
} // namespace carl
