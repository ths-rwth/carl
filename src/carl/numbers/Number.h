#pragma once

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
#endif

#include <string>
#include "numbers.h"

namespace carl {


//TODO: was ist der Unterschied zwischen den einstelligen und zweistelligen Operatoren und welche brauchen wir?
//      auch bei Sachen wie gcd, soll das innerhalb oder außerhalb der Klasse sein?

	template<typename T>
	class BaseNumber {
	protected:
		T mData;
	public:

		//TODO: which constructors explicit?
		//TODO: check if these constructors actually work for all possible instantiations
		//TODO: possibly implement setValue(T) as well
		BaseNumber(): mData(carl::constant_zero<T>::get()) {}
		explicit BaseNumber(const T& t): mData(t) {}
		explicit BaseNumber(T&& t): mData(std::move(t)) {}
		BaseNumber(const BaseNumber<T>& n): mData(n.mData) {}
		BaseNumber(BaseNumber<T>&& n) noexcept : mData(std::move(n.mData)) {}
		//TODO: is this really the best way? one could also create an mpz_class, multiply it by 10^whatever and add the rest...
		BaseNumber(long long int n) { std::string s = std::to_string(n); mData = T(s); }
		BaseNumber(unsigned long long int n) { std::string s = std::to_string(n); mData= T(s); }
		BaseNumber(int i) : mData(i) {}
		BaseNumber(const std::string& s) : mData(s) {}

		const T& getNumber() const {
			return mData;
		};

		BaseNumber<T>& operator=(const BaseNumber<T>& n) {
			mData = n.mData;
			return *this;
		}

		template<typename Other>
		BaseNumber<T>& operator=(const Other& n) {
			mData = n;
			return *this;
		}

		BaseNumber<T>& operator=(BaseNumber<T>&& n) noexcept {
			mData = std::move(n.mData);
			return *this;
		}

		operator T&() {
			return mData;
		}
		operator const T&() const {
			return mData;
		}

		virtual std::string toString(bool _infix) const = 0;


	};

	

	template<typename T>
	class Number : public BaseNumber<T> {
		public:
		explicit Number(const T& t) : BaseNumber<T>(t) {}
		Number(int i) : BaseNumber<T>(i) {}
		std::string toString(bool _infix=true) const override;
		bool isOne();
		Number<T> abs() const;
		template <typename Integer>
		Integer toInt();



		Number<T> operator+(const Number<T>& rhs) {
			return Number(this->mData + rhs.mData);
		}

		/*BaseNumber<T> operator+=(const BaseNumber<T>& rhs) {
			return T(lhs) += T(rhs);
		}*/

		Number<T> operator-(const Number<T>& rhs) {
			return Number(this->mData - rhs.mData);
		}

		/*BaseNumber<T> operator-=(const BaseNumber<T>& rhs) {
			return T(lhs) -= T(rhs);
		} */

		Number<T> operator*(const Number<T>& rhs) {
			Number(this->mData * rhs.mData);
		}

		/*BaseNumber<T> operator*=(const BaseNumber<T>& rhs) {
			return T(lhs) *= T(rhs);
		} */

		Number<T> operator/(const Number<T>& rhs) {
			Number(this->mData / rhs.mData);
		}

		/*BaseNumber<T> operator/=(const BaseNumber<T>& rhs) {
			return T(lhs) /= T(rhs);
		}*/


		bool operator==(const Number<T>& rhs) {
			return this->mData == rhs.mData;
		}

		bool operator!=(const Number<T>& rhs) {
			return this->mData != rhs.mData;
		}

		bool operator<(const Number<T>& rhs) {
			return this->mData < rhs.mData;;
		}

		bool operator<=(const Number<T>& rhs) {
			return this->mData <= rhs.mData;
		}

		bool operator>=(const Number<T>& rhs) {
			return this->mData >= rhs.mData;
		}

		bool operator>(const Number<T>& rhs) {
			return this->mData > rhs.mData;
		}



	
	};

		template <typename T>
		std::ostream& operator <<(std::ostream& os, const Number<T>& n) {
			return os << T(n);
		} 



		
}

/* template<typename T>
class Number {
public:
        Number(const T&);
        Number(long long int)
        Number(unsigned long long int)
        Number(std::string)
        const T& getNum();
};

template<>
class Number<mpq_class>: public PlusMixin<mpq_class, Number> {
       
        explicit Number(const Number<mpz_class>&);
};



mpz_class
mpq_class
cln::cl_I
cln::cl_RA

int, unsigned, float, double
mpfr

https://gmplib.org/manual/
http://www.ginac.de/CLN/cln.html

*/
