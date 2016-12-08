#pragma once

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
#endif

#include <string>
#include "numbers.h"

namespace carl {


//TODO: was ist der Unterschied zwischen den einstelligen und zweistelligen Operatoren und welche brauchen wir?
//      auch bei Sachen wie gcd, soll das innerhalb oder außerhalb der Klasse sein?

	template<typename T, typename N>
	class BaseNumber {
	protected:
		T mData;
	public:

		//TODO: which constructors explicit?
		//TODO: possibly implement setValue(T) as well
		BaseNumber(): mData(carl::constant_zero<T>::get()) {}
		explicit BaseNumber(const T& t): mData(t) {}
		explicit BaseNumber(T&& t): mData(std::move(t)) {}
		BaseNumber(const BaseNumber<T,N>& n): mData(n.mData) {}
		BaseNumber(BaseNumber<T,N>&& n) noexcept : mData(std::move(n.mData)) {}
		//TODO: is this really the best way? one could also create an mpz_class, multiply it by 10^whatever and add the rest...
		BaseNumber(long long int n) { std::string s = std::to_string(n); mData = T(s); }
		BaseNumber(unsigned long long int n) { std::string s = std::to_string(n); mData= T(s); }
		BaseNumber(int i) : mData(i) {}
		BaseNumber(const std::string& s) : mData(s) {}

		const T& getValue() const {
			return mData;
		}


		inline bool isZero() {
			return constant_zero<T>::get() == mData;
		}


		inline bool isOne() {
			return mData  == carl::constant_one<T>().get();
		}


		inline bool isPositive() const {
			return mData > carl::constant_zero<T>().get();
		}


		inline bool isNegative() const {
			return mData < carl::constant_zero<T>().get();
		}
			
		operator T&() const {
			return mData;
		}

/*
		N& operator=(const BaseNumber<T,N>& n) {
			mData = n.mData;
			return *this;
		}

		template<typename Other>
		N& operator=(const Other& n) {
			mData = n;
			return *this;
		}

		N& operator=(BaseNumber<T,N>&& n) noexcept {
			mData = std::move(n.mData);
			return *this;
		} */

		bool operator==(const BaseNumber<T,N>& rhs) const {
			return this->mData == rhs.mData;
		}

		bool operator!=(const BaseNumber<T,N>& rhs) const {
			return this->mData != rhs.mData;
		}

		bool operator<(const BaseNumber<T,N>& rhs) {
			return this->mData < rhs.mData;;
		}

		bool operator<=(const BaseNumber<T,N>& rhs) {
			return this->mData <= rhs.mData;
		}

		bool operator>=(const BaseNumber<T,N>& rhs) {
			return this->mData >= rhs.mData;
		}

		bool operator>(const BaseNumber<T,N>& rhs) {
			return this->mData > rhs.mData;
		}


		N operator+(const BaseNumber<T,N>& rhs) {
			return N(this->mData + rhs.mData);
		}

		N operator-(const BaseNumber<T,N>& rhs) {
			return N(this->mData - rhs.mData);
		}

		N operator*(const BaseNumber<T,N>& rhs) {
			return N(this->mData * rhs.mData);
		}

		N operator/(const BaseNumber<T,N>& rhs) {
			return N(this->mData / rhs.mData);
		}

		N operator-() {
			return N(-(this->mData));
		}

/*		TODO: do we want these operators? Or is Number immutable?
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
		} */



	};

	template<typename T>
	class Number : public BaseNumber<T,Number<T>> {};	



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
