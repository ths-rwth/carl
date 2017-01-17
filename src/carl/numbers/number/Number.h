#pragma once

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
#endif

#include <string>
#include "../numbers.h"

namespace carl {


//TODO: was ist der Unterschied zwischen den einstelligen und zweistelligen Operatoren und welche brauchen wir?
//      auch bei Sachen wie gcd, soll das innerhalb oder außerhalb der Klasse sein?

	template<typename T, typename N>
	class BaseNumber {
	protected:
		T mData = carl::constant_zero<T>::get();
	public:
		BaseNumber() = default;
		BaseNumber(const BaseNumber& n) = default;
		BaseNumber(BaseNumber&& n) = default; // NOLINT
		BaseNumber& operator=(const BaseNumber& n) = default;
		BaseNumber& operator=(BaseNumber&& n) = default; // NOLINT
		~BaseNumber() = default;
				
		explicit BaseNumber(const T& t): mData(t) {}
		explicit BaseNumber(T&& t): mData(std::move(t)) {}
		BaseNumber(long long int n) { std::string s = std::to_string(n); mData = T(s); } // NOLINT
		BaseNumber(unsigned long long int n) { std::string s = std::to_string(n); mData = T(s); } // NOLINT
		BaseNumber(int i) : mData(i) {} // NOLINT
		explicit BaseNumber(const std::string& s) : mData(s) {}


		const T& getValue() const noexcept {
			return mData;
		}

		inline bool isZero() const {
			return mData == constant_zero<T>::get();
		}

		inline bool isOne() const {
			return mData  == constant_one<T>().get();
		}


		inline bool isPositive() const {
			return mData > constant_zero<T>().get();
		}

		inline bool isNegative() const {
			return mData < constant_zero<T>().get();
		}
			
		operator const T&() const noexcept {
			return mData;
		}

		operator T&() noexcept {
			return mData;
		}

		template<typename Other>
		BaseNumber& operator=(const Other& n) {
			mData = n;
			return *this;
		}


		bool operator==(const BaseNumber& rhs) const {
			return this->mData == rhs.mData;
		}

		bool operator!=(const BaseNumber<T,N>& rhs) const {
			return this->mData != rhs.mData;
		}

		bool operator<(const BaseNumber<T,N>& rhs) const {
			return this->mData < rhs.mData;;
		}

		bool operator<=(const BaseNumber<T,N>& rhs) const {
			return this->mData <= rhs.mData;
		}

		bool operator>=(const BaseNumber<T,N>& rhs) const {
			return this->mData >= rhs.mData;
		}

		bool operator>(const BaseNumber<T,N>& rhs) const {
			return this->mData > rhs.mData;
		}


		N operator+(const BaseNumber<T,N>& rhs) const {
			return N(this->mData + rhs.mData);
		}

		N operator-(const BaseNumber<T,N>& rhs) const {
			return N(this->mData - rhs.mData);
		}

		N operator*(const BaseNumber<T,N>& rhs) const {
			return N(this->mData * rhs.mData);
		}

		N operator/(const BaseNumber<T,N>& rhs) const {
			return N(this->mData / rhs.mData);
		}

		N operator-() const {
			return N(-(this->mData));
		}

		//TODO: do we want these operators? Or is Number immutable?
		N operator+=(const BaseNumber<T,N>& rhs) {
			return N(mData += rhs.mData);
		} 

		N operator-=(const BaseNumber<T,N>& rhs) {
			return N(mData -= rhs.mData);
		} 


		N operator*=(const BaseNumber<T,N>& rhs) {
			return N(mData *= rhs.mData);
		} 


		N operator/=(const BaseNumber<T,N>& rhs) {
			return N(mData /= rhs.mData);
		} 



	};

	template<typename T>
	class Number: public BaseNumber<T,Number<T>> {};	



	template <typename T, typename N>
	std::ostream& operator <<(std::ostream& os, const BaseNumber<T,N>& n) {
		return os << n.getValue();
	} 

	

		
}

/* template<typename T>


int, unsigned, float, double
mpfr

https://gmplib.org/manual/
http://www.ginac.de/CLN/cln.html

*/
