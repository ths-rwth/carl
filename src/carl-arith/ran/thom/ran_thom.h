#pragma once

#include "ThomEncoding.h"
#include "../common/Operations.h""


#include <memory>

namespace carl {

// TODO adapt to new interface

template<typename Number>
struct RealAlgebraicNumberThom {
	template<typename Num>
	friend bool operator==(const RealAlgebraicNumberThom<Num>& lhs, const RealAlgebraicNumberThom<Num>& rhs);
	template<typename Num>
	friend bool operator<(const RealAlgebraicNumberThom<Num>& lhs, const RealAlgebraicNumberThom<Num>& rhs);
private:
	struct Content {
		ThomEncoding<Number> te;

		Content(const ThomEncoding<Number>& t):
			te(t)
		{}
	};
	std::shared_ptr<Content> mContent;
public:
	RealAlgebraicNumberThom(const ThomEncoding<Number>& te):
		mContent(std::make_shared<Content>(te))
	{}

	auto& thom_encoding() {
		return mContent->te;
	}
	const auto& thom_encoding() const {
		return mContent->te;
	}

	const auto& polynomial() const {
		return thom_encoding().polynomial();
	}
	const auto& main_var() const {
		return thom_encoding().main_var();
	}
	auto sign_condition() const {
		return thom_encoding().signCondition();
	}
	const auto& point() const {
		return thom_encoding().point();
	}

	std::size_t size() const {
		return thom_encoding().dimension();
	}

	std::size_t dimension() const {
		return thom_encoding().dimension();
	}

	bool is_integral() const {
		return thom_encoding().is_integral();
	}
	bool is_zero() const {
		return thom_encoding().is_zero();
	}
	bool contained_in(const Interval<Number>& i) const {
		return thom_encoding().containedIn(i);
	}

	Number integer_below() const {
		return thom_encoding().integer_below();
	}
	Sign sgn() const {
		return thom_encoding().sgn();
	}
	Sign sgn(const UnivariatePolynomial<Number>& p) const {
		return thom_encoding().sgn(p);
	}
};

template<typename Number>
Number branching_point(const RealAlgebraicNumberThom<Number>& n) {
	return n.thom_encoding().get_number();
}

template<typename Number>
Number evaluate(const MultivariatePolynomial<Number>& p, std::map<Variable, RealAlgebraicNumberThom<Number>>& m) {
	//using Polynomial = MultivariatePolynomial<Number>;
	
	CARL_LOG_INFO("carl.ran.thom",
			"\n****************************\n"
			<< "Thom evaluate\n"
			<< "****************************\n"
			<< "p = " << p << "\n"
			<< "m = " << m << "\n"
			<< "****************************\n");
	for(const auto& entry : m) { 
		assert(entry.first == entry.second.thom_encoding().main_var());
	}
	assert(m.size() > 0);
	
	std::map<Variable, RealAlgebraicNumberThom<Number>>& m_prime(m);
	auto it = m_prime.begin();
	while(it != m_prime.end()) {
			if(!p.has(it->first)) {
					CARL_LOG_TRACE("carl.thom.evaluation", "removing " << it->first);
					it = m_prime.erase(it);
			} else {
					it++;
			}
	}
	
	std::map<Variable, ThomEncoding<Number>> mTE;
	for(const auto& entry : m_prime) {
			mTE.insert(std::make_pair(entry.first, entry.second.thom_encoding()));
	}
	
	CARL_LOG_ASSERT("carl.thom.evaluation", variables(p).size() == mTE.size(), "p = " << p << ", mTE = " << mTE);
	
	if(mTE.size() == 1) {
			int sgn = int(mTE.begin()->second.signOnPolynomial(p));
			CARL_LOG_TRACE("carl.thom.evaluation", "sign of evaluated polynomial is " << sgn);
			return Number(sgn);
	}
	
	CARL_LOG_TRACE("carl.thom.evaluation", "mTE = " << mTE);
	
	ThomEncoding<Number> point = ThomEncoding<Number>::analyzeTEMap(mTE);
	int sgn = int(point.signOnPolynomial(p));
	CARL_LOG_TRACE("carl.thom.", "sign of evaluated polynomial is " << sgn);
	return Number(sgn);
	
}

template<typename Number, typename Poly>
bool evaluate(const BasicConstraint<Poly>& c, std::map<Variable, RealAlgebraicNumberThom<Number>>& m) {
	auto res = evaluate(c.lhs(), m);
	return evaluate(res, c.relation());
}

template<typename Number>
RealAlgebraicNumberThom<Number> abs(const RealAlgebraicNumberThom<Number>& n) {
	assert(false);
	return n;
}

template<typename Number>
RealAlgebraicNumberThom<Number> sample_above(const RealAlgebraicNumberThom<Number>& n) {
	return n.thom_encoding() + Number(1);
}
template<typename Number>
RealAlgebraicNumberThom<Number> sample_below(const RealAlgebraicNumberThom<Number>& n) {
	return n.thom_encoding() + Number(-1);
}
template<typename Number>
RealAlgebraicNumberThom<Number> sample_between(const RealAlgebraicNumberThom<Number>& lower, const RealAlgebraicNumberThom<Number>& upper) {
	return ThomEncoding<Number>::intermediatePoint(lower.thom_encoding(), upper.thom_encoding());
}
template<typename Number>
Number sample_between(const RealAlgebraicNumberThom<Number>& lower, const Number& upper) {
	return ThomEncoding<Number>::intermediatePoint(lower.thom_encoding(), upper);
}
template<typename Number>
Number sample_between(const Number& lower, const RealAlgebraicNumberThom<Number>& upper) {
	return ThomEncoding<Number>::intermediatePoint(lower, upper.thom_encoding());
}

template<typename Number>
Number floor(const RealAlgebraicNumberThom<Number>& n) {
	return carl::floor(get_interval(n).lower());
}
template<typename Number>
Number ceil(const RealAlgebraicNumberThom<Number>& n) {
	return carl::ceil(get_interval(n).upper());
}

template<typename Number>
bool operator==(const RealAlgebraicNumberThom<Number>& lhs, const RealAlgebraicNumberThom<Number>& rhs) {
	if (lhs.mContent.get() == rhs.mContent.get()) return true;
	return lhs.thom_encoding() == rhs.thom_encoding();
}

template<typename Number>
bool operator==(const RealAlgebraicNumberThom<Number>& lhs, const Number& rhs) {
	return lhs.thom_encoding() == rhs;
}

template<typename Number>
bool operator==(const Number& lhs, const RealAlgebraicNumberThom<Number>& rhs) {
	return lhs == rhs.thom_encoding();
}

template<typename Number>
bool operator<(const RealAlgebraicNumberThom<Number>& lhs, const RealAlgebraicNumberThom<Number>& rhs) {
	if (lhs.mContent.get() == rhs.mContent.get()) return false;
	return lhs.thom_encoding() < rhs.thom_encoding();
}

template<typename Number>
bool operator<(const RealAlgebraicNumberThom<Number>& lhs, const Number& rhs) {
	return lhs.thom_encoding() == rhs;
}

template<typename Number>
bool operator<(const Number& lhs, const RealAlgebraicNumberThom<Number>& rhs) {
	return lhs == rhs.thom_encoding();
}

template<typename Num>
std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumberThom<Num>& rhs) {
	os << "(TE " << rhs.polynomial() << " in " << rhs.main_var() << ", " << rhs.sign_condition();
	if (rhs.dimension() > 1) {
		os << " OVER " << rhs.point();
	}
	os << ")";
	return os;
}

template<typename Number>
struct is_ran_type<RealAlgebraicNumberThom<Number>> { 
  static const bool value = true;
};

}


namespace std {
template<typename Number>
struct hash<carl::RealAlgebraicNumberThom<Number>> {
    std::size_t operator()(const carl::RealAlgebraicNumberThom<Number>& n) const {
		return carl::hash_all(n.integer_below());
	}
};
}