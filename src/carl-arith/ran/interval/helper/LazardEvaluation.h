#pragma once

#include "FieldExtensions.h"
#include <carl-arith/poly/umvpoly/functions/Quotient.h>
#include <carl-arith/poly/umvpoly/functions/Substitution.h>

namespace carl::ran::interval {

template<typename Rational, typename Poly>
class LazardEvaluation {
private:
	FieldExtensions<Rational,Poly> mFieldExtensions;
	Poly mLiftingPoly;
	
public:
	LazardEvaluation(const Poly& p): mLiftingPoly(p) {}
	
	auto substitute(Variable v, const IntRepRealAlgebraicNumber<Rational>& r, bool divideZeroFactors = true) {
		auto red = mFieldExtensions.extend(v, r);
		Poly newPoly;
		if (red.first) {
			CARL_LOG_DEBUG("carl.lazard", "Substituting " << v << " by " << red.second);
			newPoly = carl::substitute(mLiftingPoly, v, red.second);
		} else {
			CARL_LOG_DEBUG("carl.lazard", "Obtained reductor " << red.second);
			newPoly = carl::remainder(mLiftingPoly, red.second);
		}
		// TODO newPoly = mFieldExtensions.embed(newPoly);
		while (carl::is_zero(newPoly) && divideZeroFactors) {
			if (red.first) {
				mLiftingPoly = carl::quotient(mLiftingPoly, v - red.second);
				newPoly = carl::substitute(mLiftingPoly, v, red.second);
			} else {
				mLiftingPoly = carl::quotient(mLiftingPoly, red.second);
				newPoly = carl::remainder(mLiftingPoly, red.second);
			}
			// TODO newPoly = mFieldExtensions.embed(newPoly);
			CARL_LOG_DEBUG("carl.lazard", "Reducing to " << mLiftingPoly);
		}
		mLiftingPoly = newPoly;
		CARL_LOG_DEBUG("carl.lazard", "Remaining poly: " << mLiftingPoly);
		return red;
	}
	
	const auto& getLiftingPoly() const {
		return mLiftingPoly;
	}
};

}