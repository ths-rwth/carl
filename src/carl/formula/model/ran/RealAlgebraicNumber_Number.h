#pragma once

namespace carl {
namespace ran {

template<typename Number>
struct NumberContent {
	Number value = carl::constant_zero<Number>::get();

	NumberContent* operator->() {
		return this;
	}
	const NumberContent* operator->() const {
		return this;
	}
	NumberContent& operator*() {
		return *this;
	}
	const NumberContent& operator*() const {
		return *this;
	}

	std::size_t size() const {
		return carl::bitsize(value);
	}

	bool is_number() const {
		return true;
	}
	const Number& get_number() const {
		return value;
	}
	bool is_integral() const {
		return carl::isInteger(value);
	}
	bool is_zero() const {
		return carl::isZero(value);
	}

	Number integer_below() const {
		return carl::floor(value);
	}
};

template<typename Num>
std::ostream& operator<<(std::ostream& os, const NumberContent<Num>& ran) {
	return os << "NR " << ran.value;
}

}
}