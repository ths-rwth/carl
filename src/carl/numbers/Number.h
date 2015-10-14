#pragma once

namespace carl {
	
	template<typename T>
	class Number {
	private:
		T mData;
	public:
		Number(): mData(carl::constant_one<T>::get()) {}
		Number(const T& t): mData(t) {}
		Number(T&& t): mData(std::move(t)) {}
		
		operator T&() {
			return mData;
		}
		operator const T&() const {
			return mData;
		}
	};
	
	template<typename T>
	Number<T> operator+(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) + T(rhs);
	}
	template<typename T>
	Number<T> operator+=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) += T(rhs);
	}
	template<typename T>
	Number<T> operator-(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) - T(rhs);
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
	Number<T> operator==(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) == T(rhs);
	}
	template<typename T>
	Number<T> operator!=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) != T(rhs);
	}
	template<typename T>
	Number<T> operator<(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) < T(rhs);
	}
	template<typename T>
	Number<T> operator<=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) <= T(rhs);
	}
	template<typename T>
	Number<T> operator>=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) >= T(rhs);
	}
	template<typename T>
	Number<T> operator>(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) > T(rhs);
	}
	
}
