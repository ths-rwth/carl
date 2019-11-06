/* 
 * File:   ThomEncoding.h
 * Author: tobias
 *
 * Created on 19. August 2016, 10:22
 */

#pragma once

#include "SignDetermination/SignDetermination.h"
#include "ThomRootFinder.h"
#include "../core/polynomialfunctions/Substitution.h"

// some settings
#define INITIAL_OFFSET (Number(1)/10)


namespace carl {

template<typename Number>
class ThomEncoding {
	
	using Polynomial = MultivariatePolynomial<Number>;
	
	mutable SignCondition mSc; // this can grow when comparing
	Polynomial mP;
	Variable mMainVar;
	
	std::shared_ptr<ThomEncoding<Number>> mPoint;
	mutable std::shared_ptr<SignDetermination<Number>> mSd;
	
	uint mRelevant;
	
public:
	
	ThomEncoding(
		SignCondition sc,
		const Polynomial& p,
		Variable mainVar,
		std::shared_ptr<ThomEncoding<Number>> point, 
		std::shared_ptr<SignDetermination<Number>> sd,
		uint mRelevant
	):
		mSc(std::move(sc)),
		mP(p),
		mMainVar(mainVar),
		mPoint(std::move(point)),
		mSd(std::move(sd)),
		mRelevant(mRelevant)
	{}
		
	ThomEncoding(
		const Number& n,
		Variable mainVar,
		std::shared_ptr<ThomEncoding<Number>> point = nullptr
	):
		mMainVar(mainVar), mPoint(std::move(point))
	{
		Polynomial p = Polynomial(mainVar) - n;
		std::list<ThomEncoding<Number>> roots = realRootsThom(p, mainVar, point);
		CARL_LOG_ASSERT("carl.thom", roots.size() == 1, "");
		mSc = roots.front().mSc;
		mP = roots.front().mP;
		mSd = roots.front().mSd;
		mRelevant = roots.front().mRelevant;      
	}
	
	ThomEncoding(
		const ThomEncoding<Number>& te,
		std::shared_ptr<ThomEncoding<Number>> point
	)
	{
		CARL_LOG_ASSERT("carl.thom", te.mP.isUnivariate(), "");
		CARL_LOG_ASSERT("carl.thom", te.mPoint == nullptr, "");
		CARL_LOG_ASSERT("carl.thom", point != nullptr, "");
		std::list<ThomEncoding<Number>> roots = realRootsThom(te.mP, te.mMainVar, point);
		for(const auto& r : roots) {
			if(te.relevantSignCondition() == r.mSc) {
				mSc = r.mSc;
				mP = r.mP;
				mMainVar = r.mMainVar;
				mPoint = r.mPoint;
				mSd = r.mSd;
				mRelevant = r.mRelevant;
				return;
			}
		}
		CARL_LOG_ASSERT("carl.thom", false, "should never get here");
	}

	bool is_number() const {
		return false;
	}

	const auto& get_number() const {
		return carl::constant_zero<Number>::get();
	}

	bool containedIn(const Interval<Number>& i) const {
		if(i.lowerBoundType() != BoundType::INFTY) {
			if(i.lowerBoundType() == BoundType::STRICT && *this <= i.lower()) return false;
			if(i.lowerBoundType() == BoundType::WEAK && *this < i.lower()) return false;
		}
		if(i.upperBoundType() != BoundType::INFTY) {
			if(i.upperBoundType() == BoundType::STRICT && *this >= i.upper()) return false;
			if(i.upperBoundType() == BoundType::WEAK && *this > i.upper()) return false;
		}
		return true;
	}

	inline SignCondition signCondition() const { return mSc; }
	inline SignCondition relevantSignCondition() const { return mSc.trailingPart(mRelevant); }
	inline Variable::Arg mainVar() const { return mMainVar; }
	inline const Polynomial& polynomial() const { return mP; } 
	inline const ThomEncoding<Number>& point() const {assert(mPoint); return *mPoint; }
	inline SignDetermination<Number> sd() const {assert(mSd); return *mSd; }
	
	std::list<Polynomial> relevantDerivatives() const {
		std::list<Polynomial> derivatives = der(mP, mMainVar, 0, mP.degree(mMainVar));
		std::reverse(derivatives.begin(), derivatives.end());
		derivatives.resize(mRelevant);
		std::reverse(derivatives.begin(), derivatives.end());
		assert(derivatives.size() == mRelevant);
		return derivatives;
	}
	
	ThomEncoding<Number> lowestInChain() const {
		if(mPoint == nullptr) return ThomEncoding<Number>(*this);
		return mPoint->lowestInChain();
	}
	
	uint dimension() const {
		if(mPoint == nullptr) return 1;
		return 1 + point().dimension(); 
	}
	
	std::list<Polynomial> accumulatePolynomials() const {
		if(mPoint == nullptr) return {polynomial()};
		std::list<Polynomial> res = point().accumulatePolynomials();
		res.push_front(polynomial());
		return res;
	}
	
	std::list<Variable> accumulateVariables() const {
		if(mPoint == nullptr) return {mainVar()};
		std::list<Variable> res = point().accumulateVariables();
		res.push_front(mainVar());
		return res;
	}
	
	SignCondition accumulateSigns() const {
		if(mPoint == nullptr) return signCondition();
		SignCondition res = point().accumulateSigns();
		res.insert(res.begin(), mSc.begin(), mSc.end());
		return res;
	}
	
	SignCondition accumulateRelevantSigns() const {
		if(mPoint == nullptr) return this->relevantSignCondition();
		SignCondition res = point().accumulateRelevantSigns();
		SignCondition mScRel = this->relevantSignCondition();
		res.insert(res.begin(), mScRel.begin(), mScRel.end());
		return res;
	}
	
	Sign signOnPolynomial(const Polynomial& p) const {
		CARL_LOG_ASSERT("carl.thom", carl::variables(p).size() <= this->dimension(), "\np = " << p << "\nthis = " << *this);
		if(carl::isZero(p)) return Sign(0);
		if(p.isConstant()) return Sign(carl::sgn(p.lcoeff()));
		std::list<SignCondition> signs = mSd->getSigns(p);
		SignCondition relevant = accumulateRelevantSigns();
		for(const auto& sigma : signs) {
			if(relevant.isSuffixOf(sigma)) return sigma.front();
		}
		CARL_LOG_ASSERT("carl.thom", false, "we should never get here");
		std::exit(42);
		return Sign(0);
	}
	
	bool makesPolynomialZero(const Polynomial& pol, Variable::Arg pol_mainVar) const {
		assert(!carl::isZero(pol));
		assert(mMainVar != pol_mainVar);
		UnivariatePolynomial<Polynomial> pol_univ = carl::to_univariate_polynomial(pol, pol_mainVar);
		// maybe check first if pol_univ has some constant coefficient...
		for(const auto& t : pol_univ.coefficients()) {
			assert(this->dimension() >= carl::variables(t).size());
			if(this->signOnPolynomial(t) != Sign::ZERO) return false;
		}
		return true;
	}
	
	
	void extendSignCondition() const {
		CARL_LOG_ASSERT("carl.thom", signCondition().size() < polynomial().degree(mMainVar) + 1, "cannot extend sign condition any more");
		if(mSc.size() == mP.degree(mMainVar)) {
			mSc.push_front(Sign::ZERO);
			return;
		}
		Polynomial derivative = der(mP, mMainVar, 1, mP.degree(mMainVar) - mSc.size()).back();
		mSc.push_front(this->signOnPolynomial(derivative));
	}

	Sign sgn(const UnivariatePolynomial<Number>& p) const {
		return sgn(Polynomial(p));
	}
	Sign sgn(const Polynomial& p) const {
		return signOnPolynomial(p);
	}
	Sign sgn() const {
		return signOnPolynomial(Polynomial(mMainVar));
	}

	bool is_integral() const {
		return false;
	}
	Number integer_below() const {
		return carl::constant_zero<Number>::get();
	}
	
	//[[deprecated("Use sgn() instead.")]]
	Sign sgnReprNum() const {
		return signOnPolynomial(Polynomial(mMainVar));
	}
	
	bool is_zero() const {
		return sgnReprNum() == Sign::ZERO;
	}
	
	ThomEncoding<Number> concat(const ThomEncoding<Number>& other) const {
		CARL_LOG_TRACE("carl.thom", "concating: \n" << *this << "\nwith\n" << other);
		ThomEncoding<Number> result = other;
		
		std::list<ThomEncoding<Number>> thisEncodings;
		thisEncodings.push_back(ThomEncoding<Number>(*this));
		std::shared_ptr<ThomEncoding<Number>> curr = mPoint;
		while(curr != nullptr) {
			thisEncodings.push_back(ThomEncoding<Number>(*curr));
			curr = thisEncodings.back().mPoint;
		}
		assert(thisEncodings.size() == this->dimension());
		CARL_LOG_TRACE("carl.thom", "thisEncodings = " << thisEncodings);
		
		SignCondition thisSignCondition = this->accumulateRelevantSigns();
		SignCondition signCondition = other.accumulateRelevantSigns();
		signCondition.insert(signCondition.begin(), thisSignCondition.begin(), thisSignCondition.end());
		
		for(auto itEncoding = thisEncodings.rbegin(); itEncoding != thisEncodings.rend(); itEncoding++) {
			std::list<Variable> vars = result.accumulateVariables();
			if(std::find(vars.begin(), vars.end(), itEncoding->mainVar()) != vars.end()) {
				continue;
			}
			std::shared_ptr<ThomEncoding<Number>> result_ptr = std::make_shared<ThomEncoding<Number>>(result);
			std::list<ThomEncoding<Number>> roots = realRootsThom(itEncoding->polynomial(), itEncoding->mainVar(), result_ptr);
			bool succes = false;
			for(const auto& r : roots) {
				if(r.accumulateRelevantSigns().isSuffixOf(signCondition)) {
					result = r;
					succes = true;
					break;
				}
			}
			CARL_LOG_ASSERT("carl.thom", succes, "");      
		}
		CARL_LOG_TRACE("carl.thom", "result of concat: " << result);
		return result;
		
//		std::list<Polynomial> thisPolynomials = this->accumulatePolynomials();
//		std::list<Variable> thisVars = this->accumulateVariables();
//		assert(thisPolynomials.size() == thisVars.size());
//		
//		SignCondition thisSignCondition = this->accumulateRelevantSigns();
//		SignCondition signCondition = other.accumulateRelevantSigns();
//		signCondition.insert(signCondition.begin(), thisSignCondition.begin(), thisSignCondition.end());
//			
//		auto itVars = thisVars.rbegin();
//		for(auto itPoly = thisPolynomials.rbegin(); itPoly != thisPolynomials.rend(); itPoly++) {
//			std::shared_ptr<ThomEncoding<Number>> result_ptr = std::make_shared<ThomEncoding<Number>>(result);
//			std::list<ThomEncoding<Number>> roots = realRootsThom(*itPoly, *itVars, result_ptr);
//			bool succes = false;
//			for(const auto& r : roots) {
//				if(r.accumulateRelevantSigns().isSuffixOf(signCondition)) {
//					result = r;
//					succes = true;
//					break;
//				}
//			}
//			CARL_LOG_ASSERT("carl.thom", succes, "");
//			itVars++;
//		}
//		
//		CARL_LOG_TRACE("carl.thom", "result of concat: " << result);
		
	}
	
	static ThomEncoding<Number> analyzeTEMap(const std::map<Variable, ThomEncoding<Number>>& m) {
		CARL_LOG_ASSERT("carl.thom", !m.empty(), "called analyzeTEMap with empty map");
		
		// collect variables 
		// the thom ecodings can additionally contain other variables than these!!
		std::list<Variable> vars;
		for(const auto& entry : m) {
			CARL_LOG_ASSERT("carl.thom", std::find(vars.begin(), vars.end(), entry.first) == vars.end(), "variable appears twice in map");
			vars.push_back(entry.first);
		}
		std::list<Variable> originalVars = vars;
		
		// collect thom encodings
		std::list<ThomEncoding<Number>> encodings;
		for(const auto& entry : m) {
			encodings.push_back(entry.second);
		}
		
		// sort thom encodings in descending dimension
		encodings.sort(
			[](const ThomEncoding<Number>& lhs, const ThomEncoding<Number>& rhs) {
				return lhs.dimension() > rhs.dimension();
			}
		);
		
		CARL_LOG_TRACE("carl.thom", "encodings = " << encodings);
		
		ThomEncoding<Number> result = encodings.front();
		for(const auto& v : encodings.front().accumulateVariables()) {
			auto it = std::find(vars.begin(), vars.end(), v);
			if(it != vars.end()) vars.erase(it);
		}
		encodings.erase(encodings.begin());
		
		while(!vars.empty()) {
			CARL_LOG_ASSERT("carl.thom", !encodings.empty(), "");
			bool takeit = false;
			for(const auto& v : encodings.front().accumulateVariables()) {
				if(std::find(originalVars.begin(), originalVars.end(), v) == originalVars.end()) {
					continue;
				}
				auto it = std::find(vars.begin(), vars.end(), v);
				if(it != vars.end()) {
					takeit = true;
					break;
				}
			}
			if(takeit) {
				result = result.concat(encodings.front());
				for(const auto& v : encodings.front().accumulateVariables()) {
					auto it = std::find(vars.begin(), vars.end(), v);
					if(it != vars.end()) vars.erase(it);
				}
			}
			encodings.erase(encodings.begin());
		}
		
		CARL_LOG_TRACE("carl.thom", "result = " << result);
		
		return result;
		
	}
	
	//*******************//
	//    COMPARISON     //
	//*******************//
	
	static ThomComparisonResult compare(const ThomEncoding<Number>& lhs, const ThomEncoding<Number>& rhs) {
		CARL_LOG_INFO("carl.thom.compare", "lhs = " << lhs << ", rhs = " << rhs);
		
		// 1. Encodings from different levels
		if(!areComparable(lhs, rhs)) {
			CARL_LOG_TRACE("carl.thom.compare", "encodings are not comparable");
			// in this case it is not necessary to compare them by the numbers they actually represent
			return compareDifferentLevels(lhs, rhs);
		}
		CARL_LOG_ASSERT("carl.thom.compare", lhs.mainVar() == rhs.mainVar(), "");
		
		// 2. same underlying polynomial
		if(lhs.polynomial() == rhs.polynomial()) {
			CARL_LOG_TRACE("carl.thom.compare", "comparing encodings with same underlying polynomials");
			return SignCondition::compare(lhs.relevantSignCondition(), rhs.relevantSignCondition());
		}
		
		// 3. different underlying polynomial
		// both are nullptr or both are not nullptr
		if( !(lhs.mPoint == nullptr && rhs.mPoint != nullptr) && !(lhs.mPoint != nullptr && rhs.mPoint == nullptr)) {
			CARL_LOG_TRACE("carl.thom.compare", "comparing encodings with different underlying polynomials, !!hopefully with same defining point!!");
			return compareDifferentPoly(lhs, rhs);
		}

		// 4. one of the encodings is "new" on this level and therefore has no underlying point
		CARL_LOG_ASSERT("carl.thom.compare", lhs.mPoint == nullptr || rhs.mPoint == nullptr, "");
		CARL_LOG_ASSERT("carl.thom.compare", lhs.mPoint != nullptr || rhs.mPoint != nullptr, "");
		CARL_LOG_TRACE("carl.thom.compare", "one of the encodings is new on this level");
		if(lhs.mPoint == nullptr) {
			return compare(ThomEncoding<Number>(lhs, rhs.mPoint), rhs);
		}
		if(rhs.mPoint == nullptr) {
			return compare(lhs, ThomEncoding<Number>(rhs, lhs.mPoint));
		}
			
		CARL_LOG_ASSERT("carl.thom.compare", false, "we should never get here ... inputs where lhs = " << lhs << ", rhs = " << rhs);
		return EQUAL;
	}
private:      
	
	/*
	 * This is not the case if
	 *      1. lhs and rhs do not have the same main variable
	 *      2. they both have a different underlying point which is not nullptr
	 */
	static bool areComparable(const ThomEncoding<Number>& lhs, const ThomEncoding<Number>& rhs) {
		if(lhs.mainVar() != rhs.mainVar()) {
			return false;
		}
		// they have the same main var
		if(lhs.mPoint != nullptr && rhs.mPoint != nullptr) {
			CARL_LOG_TRACE("carl.thom.compare", "\nlhs = " << lhs << "\nrhs = " << rhs << "\nlhs.mPoint = " << lhs.mPoint << "\nrhs.mPoint = " << rhs.mPoint);
			if(!lhs.point().equals(rhs.point())) {
				CARL_LOG_TRACE("carl.thom.compare", "underlying points are unequal");
				return false;
			}
			CARL_LOG_TRACE("carl.thom.compare", "underlying points are equal");
		}
		return true;
	}
	
	static ThomComparisonResult compareDifferentLevels(const ThomEncoding<Number>& lhs, const ThomEncoding<Number>& rhs) {
		if(lhs.polynomial() < rhs.polynomial()) return LESS;
		if(lhs.polynomial() > rhs.polynomial()) return GREATER;
		if(lhs.mPoint == nullptr && rhs.mPoint != nullptr) return LESS;
		if(lhs.mPoint != nullptr && rhs.mPoint == nullptr) return GREATER;
		if(lhs.mPoint != nullptr && rhs.mPoint != nullptr) return compareDifferentLevels(lhs.point(), rhs.point());
		// same polynomials, mPoint both nullptr
		CARL_LOG_ASSERT("carl.thom.compare", lhs.mPoint == nullptr && rhs.mPoint == nullptr, "i think this should not happen");
		return SignCondition::compare(lhs.relevantSignCondition(), rhs.relevantSignCondition());
		
	}
public:
	static ThomComparisonResult compareRational(const ThomEncoding<Number>& lhs, const Number& rhs) {
		ThomEncoding<Number> rhs_te(rhs, lhs.mainVar(), lhs.mPoint);
		return compare(lhs, rhs_te);
	}
	
	static ThomComparisonResult compareDifferentPoly(const ThomEncoding<Number>& lhs, const ThomEncoding<Number>& rhs);
	
	// checks if the thom encodings are "literally" the same (this is not the same as operator =)
	bool equals(const ThomEncoding<Number>& other) const {
		if(this->polynomial() != other.polynomial()) return false;
		if(this->relevantSignCondition() !=  other.relevantSignCondition()) return false;
		if(this->dimension() != other.dimension()) return false;
		if(this->mPoint != nullptr && other.mPoint != nullptr) {
			return this->point().equals(other.point());
		}
		return true;
	}
		

public:       
	
	/* Addition */

	ThomEncoding<Number> operator+(const Number& rhs) const {
		Polynomial subs = Polynomial(mMainVar) - rhs;
		Polynomial p = carl::substitute(mP, mMainVar, subs);
		std::list<ThomEncoding<Number>> roots = realRootsThom(p, mMainVar, mPoint);
		for(const auto& r: roots) {
			if (relevantSignCondition() == r.mSc) return r;
		}
		CARL_LOG_ASSERT("carl.thom.samples", false, "we should never get here");
		std::exit(42);
		return *this;
	}
	
	/*
	 * returns a thom encoding representing a number in the interval (lhs, rhs)
	 */ 
	static ThomEncoding<Number> intermediatePoint(const ThomEncoding<Number>& lhs, const ThomEncoding<Number>& rhs) {
		CARL_LOG_INFO("carl.thom.samples", lhs << ", " << rhs);
		CARL_LOG_ASSERT("carl.thom.samples", lhs.mMainVar == rhs.mMainVar, "no intermediate point between encodings on different levels!");
		CARL_LOG_ASSERT("carl.thom.samples", lhs < rhs, "lhs >= rhs !!!");
		Number epsilon(INITIAL_OFFSET);
		std::cout << "epsilon = " << epsilon << std::endl;
		// pick the polynomial with smaller degree here (or lhs, if equal degree)
		if(lhs.mP.degree(lhs.mMainVar) <= rhs.mP.degree(rhs.mMainVar)) {
			ThomEncoding<Number> res = lhs + epsilon;
			while(res >= rhs) {
				epsilon /= 2;
				res = lhs + epsilon;
			}
			CARL_LOG_TRACE("carl.thom.samples", "result: " << res);
			return res;
		}
		else {
			epsilon = -epsilon;
			ThomEncoding<Number> res = rhs + epsilon;
			std::cout << "res = " << res << std::endl;
			while(lhs >= res) {
				epsilon /= 2;
				res = rhs + epsilon;
			}
			CARL_LOG_TRACE("carl.thom.samples", "result: " << res);
			return res;
		}
		
	}
	
	/*
	 * finds an intermediate point between the thom encoding and the rational
	 * always returns a rational
	 */
	static Number intermediatePoint(const ThomEncoding<Number>& lhs, const Number& rhs) {
		CARL_LOG_INFO("carl.thom.samples", lhs << ", " << rhs);
		CARL_LOG_ASSERT("carl.thom.samples", lhs < rhs, "intermediatePoint with wrong order or equal arguments called");
		Number res;
		Number epsilon(INITIAL_OFFSET);
		res = rhs - epsilon;
		while(lhs >= res) {
		    epsilon /= 2;
		    res = rhs - epsilon;
		}
		CARL_LOG_TRACE("carl.thom.samples", "result: " << res);
		return res;
	}
	
	static Number intermediatePoint(const Number& lhs, const ThomEncoding<Number>& rhs) {
		CARL_LOG_INFO("carl.thom.samples", lhs << ", " << rhs);
		CARL_LOG_ASSERT("carl.thom.samples", lhs < rhs, "intermediatePoint with wrong order order in arguments called");
		Number res;
		Number epsilon(INITIAL_OFFSET);
		res = lhs + epsilon;
		while(res >= rhs) {
		    epsilon /= 2;
		    res = lhs + epsilon;
		}
		CARL_LOG_TRACE("carl.thom.samples", "result: " << res);
		return res;
	}
		
	
	/* Output */
	
	void print(std::ostream& os) const {
		os << "\n---------------------------------------------------" << std::endl;
		os << "Thom encoding" << std::endl;
		os << "---------------------------------------------------" << std::endl;
		os << "sign condition:\t\t\t" << mSc << std::endl;
		os << "relevant:\t\t\t" << mRelevant << std::endl;
		os << "polynomial:\t\t\t" << mP << std::endl;
		os << "main variable: \t\t\t" << mMainVar  << std::endl;
		os << "point below:\t\t\t" << mPoint << std::endl;
		os << "sign determination obejct:\t" << mSd << std::endl;
		os << "dimension:\t\t\t" << dimension() << std::endl;
		os << "---------------------------------------------------" << std::endl;
	}
	
}; // class ThomEncoding



	
// compare thom encodings with thom encodings
template<typename N>
bool operator<(const ThomEncoding<N>& lhs, const ThomEncoding<N>& rhs) { return ThomEncoding<N>::compare(lhs, rhs) == LESS; }
template<typename N>
bool operator<=(const ThomEncoding<N>& lhs, const ThomEncoding<N>& rhs) { return ThomEncoding<N>::compare(lhs, rhs) != GREATER; } 
template<typename N>
bool operator>(const ThomEncoding<N>& lhs, const ThomEncoding<N>& rhs) { return ThomEncoding<N>::compare(lhs, rhs) == GREATER; }    
template<typename N>
bool operator>=(const ThomEncoding<N>& lhs, const ThomEncoding<N>& rhs) { return ThomEncoding<N>::compare(lhs, rhs) != LESS; } 
template<typename N>
bool operator==(const ThomEncoding<N>& lhs, const ThomEncoding<N>& rhs) { return ThomEncoding<N>::compare(lhs, rhs) == EQUAL; }
template<typename N>
bool operator!=(const ThomEncoding<N>& lhs, const ThomEncoding<N>& rhs) { return ThomEncoding<N>::compare(lhs, rhs) != EQUAL; }

// comparing with rational numbers
template<typename N>
bool operator<(const ThomEncoding<N>& lhs, const N& rhs) { return ThomEncoding<N>::compareRational(lhs, rhs) == LESS; }   
template<typename N>
bool operator<=(const ThomEncoding<N>& lhs, const N& rhs) { return ThomEncoding<N>::compareRational(lhs, rhs) != GREATER; }
template<typename N>
bool operator>(const ThomEncoding<N>& lhs, const N& rhs) { return ThomEncoding<N>::compareRational(lhs, rhs) == GREATER; }
template<typename N>
bool operator>=(const ThomEncoding<N>& lhs, const N& rhs) { return ThomEncoding<N>::compareRational(lhs, rhs) != LESS; }    
template<typename N>
bool operator==(const ThomEncoding<N>& lhs, const N& rhs) { return ThomEncoding<N>::compareRational(lhs, rhs) == EQUAL; }  
template<typename N>
bool operator!=(const ThomEncoding<N>& lhs, const N& rhs) { return ThomEncoding<N>::compareRational(lhs, rhs) != EQUAL; }

template<typename N>
bool operator<(const N& lhs, const ThomEncoding<N>& rhs) { return rhs > lhs; }      
template<typename N>
bool operator<=(const N& lhs, const ThomEncoding<N>& rhs) { return rhs >= lhs; }    
template<typename N>
bool operator>(const N& lhs, const ThomEncoding<N>& rhs) { return rhs < lhs; }   
template<typename N>
bool operator>=(const N& lhs, const ThomEncoding<N>& rhs) { return rhs <= lhs; }
template<typename N>
bool operator==(const N& lhs, const ThomEncoding<N>& rhs) { return rhs == lhs; } 
template<typename N>
bool operator!=(const N& lhs, const ThomEncoding<N>& rhs) { return rhs != lhs; }

template<typename N>
ThomEncoding<N> operator+(const N& lhs, const ThomEncoding<N>& rhs) { return rhs + lhs; }

template<typename N>
std::ostream& operator<<(std::ostream& os, const ThomEncoding<N>& rhs) {
	os << "TE " << rhs.polynomial() << " in " << rhs.mainVar() << ", " << rhs.signCondition();
	if(rhs.dimension() > 1) {
		os << " OVER " << rhs.point();
	}
	return os;
}

} // namespace carl

#include "ThomEncoding.tpp"
