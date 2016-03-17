#pragma once

namespace carl {

	template<typename T>
	class Number {
	private:
		T mData;
	public:
		Number(): mData(carl::constant_zero<T>::get()) {}
		explicit Number(const T& t): mData(t) {}
		explicit Number(T&& t): mData(std::move(t)) {}
		Number(const Number<T>& n): mData(n.mData) {}
		Number(Number<T>&& n) noexcept : mData(std::move(n.mData)) {}

		Number<T>& operator=(const Number<T>& n) {
			mData = n.mData;
			return *this;
		}

		template<typename Other>
		Number<T>& operator=(const Other& n) {
			mData = n;
			return *this;
		}

		Number<T>& operator=(Number<T>&& n) noexcept {
			mData = std::move(n.mData);
			return *this;
		}

		operator T&() {
			return mData;
		}
		operator const T&() const {
			return mData;
		}
	};

	template<typename T>
	Number<T> operator+(const Number<T>& lhs, const Number<T>& rhs) {
		return T(T(lhs) + T(rhs));
	}
	template<typename T>
	Number<T> operator+=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) += T(rhs);
	}
	template<typename T>
	Number<T> operator-(const Number<T>& lhs, const Number<T>& rhs) {
		return T(T(lhs) - T(rhs));
	}
	template<typename T>
	Number<T> operator-=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) -= T(rhs);
	}
	template<typename T>
	Number<T> operator*(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) * T(rhs);
	}
	template<typename T>
	Number<T> operator*=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) *= T(rhs);
	}
	template<typename T>
	Number<T> operator/(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) / T(rhs);
	}
	template<typename T>
	Number<T> operator/=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) /= T(rhs);
	}

	template<typename T>
	bool operator==(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) == T(rhs);
	}
	template<typename T>
	bool operator!=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) != T(rhs);
	}
	template<typename T>
	bool operator<(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) < T(rhs);
	}
	template<typename T>
	bool operator<=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) <= T(rhs);
	}
	template<typename T>
	bool operator>=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) >= T(rhs);
	}
	template<typename T>
	bool operator>(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) > T(rhs);
	}

	template<typename T>
	std::ostream& operator <<(std::ostream& os, const Number<T>& n) {
		return os << T(n);
	}

}
