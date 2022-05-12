/** 
 * @file   VariableInformation.h
 * @ingroup multirp
 * @author Sebastian Junges
 * @since September 3, 2013
 */

#pragma once

#include "../MonomialPool.h"
#include <carl/core/Variable.h>

#include <algorithm>
#include <map>
#include <memory>
#include <vector>

namespace carl {

template<bool collectCoeff, typename CoeffType>
struct VariableInformation {
};

template<typename CoeffType>
class VariableInformation<false, CoeffType> {

	/// Maximal degree variable occurs with.
	std::size_t mMaxDegree = 0;
	/// Minimal non-zero degree variable occurs with.
	std::size_t mMinDegree = 0;
	/// Number of terms a variable occurs in.
	std::size_t mOccurence = 0;

public:
	VariableInformation() = default;

	explicit VariableInformation(std::size_t degreeOfOccurence)
		: mMaxDegree(degreeOfOccurence),
		  mMinDegree(degreeOfOccurence),
		  mOccurence(1) {}

	VariableInformation(std::size_t maxDegree, std::size_t minDegree, std::size_t occurence)
		: mMaxDegree(maxDegree),
		  mMinDegree(minDegree),
		  mOccurence(occurence) {}

	VariableInformation(const VariableInformation& varInfo) = default;

	bool hasCoeff() const {
		return false;
	}

	std::size_t maxDegree() const {
		return mMaxDegree;
	}

	std::size_t minDegree() const {
		return mMinDegree;
	}

	std::size_t occurence() const {
		return mOccurence;
	}

	/**
		 * If degree is larger than maxDegree, we set the maxDegree to degree.
         * @param degree
		 * @return true if degree was larger.
         */
	bool raiseMaxDegree(std::size_t degree) {
		if (mMaxDegree < degree) {
			mMaxDegree = degree;
			return true;
		}
		return false;
	}

	/**
		 * If degree is smaller than minDegree, we set the minDegree to degree.
         * @param degree
		 * @return true if degree was smaller.
         */
	bool lowerMinDegree(std::size_t degree) {
		if (mMinDegree > degree) {
			mMinDegree = degree;
			return true;
		}
		return false;
	}

	void increaseOccurence() {
		++mOccurence;
	}

	template<typename Term>
	void updateCoeff(std::size_t, const Term&) {
		// Empty function, we do not save the coefficient here.
		// TODO there might be a better solution to this.
	}

	void collect(Variable::Arg var, const typename CoeffType::CoeffType&, const typename CoeffType::MonomType& monomial) {
		std::size_t e = monomial.exponentOfVariable(var);
		if (e > 0) {
			// One more term in which the variable occurs.
			increaseOccurence();
			// Update minimal/maximal degree.
			if (!raiseMaxDegree(e)) {
				// Only if raising failed, lowering can be successful.
				lowerMinDegree(e);
			}
		}
	}
};

template<typename CoeffType>
class VariableInformation<true, CoeffType> : public VariableInformation<false, CoeffType> {
	std::map<std::size_t, CoeffType> mCoeffs;

public:
	VariableInformation()
		: VariableInformation<false, CoeffType>(),
		  mCoeffs() {
	}

	explicit VariableInformation(std::size_t degreeOfOccurence)
		: VariableInformation<false, CoeffType>(degreeOfOccurence),
		  mCoeffs() {
	}

	VariableInformation(std::size_t maxDegree, std::size_t minDegree, std::size_t occurence, std::map<std::size_t, CoeffType>&& coeffs)
		: VariableInformation<false, CoeffType>(maxDegree, minDegree, occurence),
		  mCoeffs(coeffs) {
	}

	VariableInformation(const VariableInformation<false, CoeffType>& varInfo)
		: VariableInformation<false, CoeffType>(varInfo),
		  mCoeffs() {
	}

	bool hasCoeff() const {
		return !mCoeffs.empty();
	}

	const std::map<std::size_t, CoeffType>& coeffs() const {
		return mCoeffs;
	}

	template<typename Term>
	void updateCoeff(std::size_t exponent, const Term& t) {
		auto it = mCoeffs.find(exponent);
		if (it == mCoeffs.end()) {
			mCoeffs.emplace(exponent, CoeffType(t));

		} else {
			it->second += t;
		}
	}

	void collect(Variable::Arg v, const typename CoeffType::CoeffType& termCoeff, const typename CoeffType::MonomType& monomial) {
		exponent e = 0;
		std::vector<std::pair<Variable, exponent>> exps;
		exps.reserve(monomial.nrVariables() - 1);
		exponent totalDegree = monomial.tdeg();
		for (std::size_t i = 0; i < monomial.nrVariables(); ++i) {
			if (monomial[i].first == v) {
				totalDegree -= monomial[i].second;
				e = monomial[i].second;
			} else {
				exps.push_back(monomial[i]);
			}
		}
		if (totalDegree != 0) {
			auto m = createMonomial(std::move(exps), totalDegree);
			updateCoeff(e, typename CoeffType::TermType(termCoeff, m));
		} else {
			updateCoeff(e, typename CoeffType::TermType(termCoeff));
		}
		if (e > 0) {
			// One more term in which the variable occurs.
			this->increaseOccurence();
			// Update minimal/maximal degree.
			if (!this->raiseMaxDegree(e)) {
				// Only if raising failed, lowering can be successful.
				this->lowerMinDegree(e);
			}
		}
	}
};

} // namespace carl
