/**
 * @file CADPolynomials.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <algorithm>
#include <list>
#include <unordered_map>

#include "CADTypes.h"

namespace carl {
namespace cad {

template<typename Number>
class CADPolynomials : public carl::cad::PolynomialOwner<Number>  {
public:
	/// Type of univariate polynomials.
	typedef carl::cad::UPolynomial<Number> UPolynomial;
	/// Type of multivariate polynomials.
	typedef carl::cad::MPolynomial<Number> MPolynomial;
private:
	/**
	 * List of all polynomials for elimination.
	 */
	std::list<const UPolynomial*> polynomials;

	/**
	 * Maps multivariate polynomials given as input to the univariate polynomials that are used internally.
	 */
	std::unordered_map<const MPolynomial, const UPolynomial*, std::hash<MPolynomial>> map;

	/**
	 * list of polynomials scheduled for elimination
	 */
	std::vector<const UPolynomial*> scheduled;
public:
	CADPolynomials(): cad::PolynomialOwner<Number>() {}
	CADPolynomials(cad::PolynomialOwner<Number>* parent): cad::PolynomialOwner<Number>(parent) {}
	
	bool hasScheduled() const {
		return !scheduled.empty();
	}
	bool isScheduled(const UPolynomial* up) const {
		return std::find(scheduled.begin(), scheduled.end(), up) != scheduled.end();
	}
	void schedule(const UPolynomial* up, bool take = true) {
		if (take) scheduled.push_back(this->take(up));
		else scheduled.push_back(up);
	}
	void schedule(const MPolynomial& p, const UPolynomial* up, bool take = true) {
		map[p] = up;
		schedule(up, take);
	}
	void clearScheduled() {
		scheduled.clear();
	}
	auto getScheduled() const -> const decltype(scheduled)& {
		return scheduled;
	}
	
	void addPolynomial(const UPolynomial* up) {
		polynomials.push_back(up);
	}
	auto getPolynomials() const -> const decltype(polynomials)& {
		return polynomials;
	}
	
	const UPolynomial* removePolynomial(const MPolynomial& p) {
		auto it = map.find(p);
		if (it == map.end()) return nullptr;
		
		auto up = it->second;
		map.erase(it);
		
		for (auto sit = scheduled.begin(); sit != scheduled.end(); ++sit) {
			if (**sit == *up) {
				scheduled.erase(sit);
				return up;
			}
		}
		
		for (auto pit = polynomials.begin(); pit != polynomials.end(); ++pit) {
			if (**pit == *up) {
				polynomials.erase(pit);
				return up;
			}
		}
		CARL_LOG_ERROR("carl.cad", "Removing polynomial " << p << " that is neither scheduled nor in polynomials.");
		assert(false);
		return up;
	}
	
	void clear() {
		polynomials.clear();
	}
};

template<typename Number>
inline std::ostream& operator<<(std::ostream& os, const CADPolynomials<Number>& p) {
	os << p.getPolynomials() << " // " << p.getScheduled();
	return os;
}

}
}
