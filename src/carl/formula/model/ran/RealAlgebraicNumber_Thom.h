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
const Number& get_number(const ThomContent<Number>& n) {
	return n.thom_encoding().get_number();
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
bool operator==(const ThomContent<Number>& lhs, const Number& rhs) {
	return lhs.thom_encoding() == rhs;
}

template<typename Number>
bool operator<(const ThomContent<Number>& lhs, const ThomContent<Number>& rhs) {
	if (lhs.mContent.get() == rhs.mContent.get()) return false;
	return lhs.thom_encoding() < rhs.thom_encoding();
}

template<typename Number>
bool operator<(const ThomContent<Number>& lhs, const Number& rhs) {
	return lhs.thom_encoding() < rhs;
}
template<typename Number>
bool operator<(const Number& lhs, const ThomContent<Number>& rhs) {
	return lhs < rhs.thom_encoding();
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