#pragma once

#include "../../../thom/ThomEncoding.h"

#include <memory>

namespace carl {
namespace ran {

template<typename Number>
struct ThomContent {
	template<typename Num>
	friend bool operator==(const ThomContent<Num>& lhs, const ThomContent<Num>& rhs);
	template<typename Num>
	friend bool operator<(const ThomContent<Num>& lhs, const ThomContent<Num>& rhs);
private:
	struct Content {
		ThomEncoding<Number> te;

		Content(const ThomEncoding<Number>& t):
			te(t)
		{}
	};
	std::shared_ptr<Content> mContent;
public:
	ThomContent(const ThomEncoding<Number>& te):
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
	const auto& mainVar() const {
		return thom_encoding().mainVar();
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
Number branching_point(const ThomContent<Number>& n) {
	return n.thom_encoding().get_number();
}

template<typename Number>
Number evaluate(const MultivariatePolynomial<Number>& p, std::map<Variable, ThomContent<Number>>& m) {
	//using Polynomial = MultivariatePolynomial<Number>;
	
	CARL_LOG_INFO("carl.ran.thom",
			"\n****************************\n"
			<< "Thom evaluate\n"
			<< "****************************\n"
			<< "p = " << p << "\n"
			<< "m = " << m << "\n"
			<< "****************************\n");
	for(const auto& entry : m) { 
		assert(entry.first == entry.second.thom_encoding().mainVar());
	}
	assert(m.size() > 0);
	
	std::map<Variable, ThomContent<Number>>& m_prime(m);
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
	
	CARL_LOG_ASSERT("carl.thom.evaluation", p.gatherVariables().size() == mTE.size(), "p = " << p << ", mTE = " << mTE);
	
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
bool evaluate(const Constraint<Poly>& c, std::map<Variable, ThomContent<Number>>& m) {
	auto res = evaluate(c.lhs(), m);
	return evaluate(res, c.relation());
}

template<typename Number>
ThomContent<Number> abs(const ThomContent<Number>& n) {
	assert(false);
	return n;
}

template<typename Number>
Interval<Number> get_interval(const ThomContent<Number>& n) {
	return Interval<Number>(n.thom_encoding().get_number());
}

template<typename Number>
const Number& get_number(const ThomContent<Number>& n) {
	return n.thom_encoding().get_number();
}

template<typename Number>
UnivariatePolynomial<Number> get_polynomial(const ThomContent<Number>& n) {
	return UnivariatePolynomial<Number>(Variable::NO_VARIABLE);
}

template<typename Number>
bool is_number(const ThomContent<Number>& n) {
	return n.thom_encoding().is_number();
}

template<typename Number>
ThomContent<Number> sample_above(const ThomContent<Number>& n) {
	return n.thom_encoding() + Number(1);
}
template<typename Number>
ThomContent<Number> sample_below(const ThomContent<Number>& n) {
	return n.thom_encoding() + Number(-1);
}
template<typename Number>
ThomContent<Number> sample_between(const ThomContent<Number>& lower, const ThomContent<Number>& upper) {
	return ThomEncoding<Number>::intermediatePoint(lower.thom_encoding(), upper.thom_encoding());
}
template<typename Number>
Number sample_between(const ThomContent<Number>& lower, const NumberContent<Number>& upper) {
	return ThomEncoding<Number>::intermediatePoint(lower.thom_encoding(), upper.value());
}
template<typename Number>
Number sample_between(const NumberContent<Number>& lower, const ThomContent<Number>& upper) {
	return ThomEncoding<Number>::intermediatePoint(lower.value(), upper.thom_encoding());
}

template<typename Number>
bool operator==(const ThomContent<Number>& lhs, const ThomContent<Number>& rhs) {
	if (lhs.mContent.get() == rhs.mContent.get()) return true;
	return lhs.thom_encoding() == rhs.thom_encoding();
}

template<typename Number>
bool operator==(const ThomContent<Number>& lhs, const NumberContent<Number>& rhs) {
	return lhs.thom_encoding() == rhs.value();
}

template<typename Number>
bool operator==(const NumberContent<Number>& lhs, const ThomContent<Number>& rhs) {
	return lhs.value() == rhs.thom_encoding();
}

template<typename Number>
bool operator<(const ThomContent<Number>& lhs, const ThomContent<Number>& rhs) {
	if (lhs.mContent.get() == rhs.mContent.get()) return false;
	return lhs.thom_encoding() < rhs.thom_encoding();
}

template<typename Number>
bool operator<(const ThomContent<Number>& lhs, const NumberContent<Number>& rhs) {
	return lhs.thom_encoding() == rhs.value();
}

template<typename Number>
bool operator<(const NumberContent<Number>& lhs, const ThomContent<Number>& rhs) {
	return lhs.value() == rhs.thom_encoding();
}

template<typename Num>
std::ostream& operator<<(std::ostream& os, const ThomContent<Num>& rhs) {
	os << "TE " << rhs.polynomial() << " in " << rhs.mainVar() << ", " << rhs.sign_condition();
	if (rhs.dimension() > 1) {
		os << " OVER " << rhs.point();
	}
	return os;
}

}
}