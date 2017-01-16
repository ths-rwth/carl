#pragma once



#include "Number.h"



namespace carl {




	template<>
	class Number<Interval<cln::cl_I>> : public BaseNumber<Interval<cln::cl_I>,Number<Interval<cln::cl_I>>> {
	public:

		Number(): BaseNumber() {}
		explicit Number(const Interval<cln::cl_I>& t): BaseNumber(t) {}
		explicit Number(Interval<cln::cl_I>&& t): BaseNumber(t) {}
		Number(const Number<Interval<cln::cl_I>>& n): BaseNumber(n) {}
		Number(Number<Interval<cln::cl_I>>&& n) noexcept : BaseNumber(n) {}
		//explicit Number(const std::string& s) { mData = cln::cl_I(s.c_str()); }
		//Number(int n) : BaseNumber(n) {}
		//Number(long long int n) { mData = cln::cl_I(n); }
		//Number(unsigned long long int n) { mData = cln::cl_I(n);}

		
		Number<Interval<cln::cl_I>>& operator=(const Number<Interval<cln::cl_I>>& n) {
			this->mData = n.mData;
			return *this;
		}

		template<typename Other>
		Number<Interval<cln::cl_I>>& operator=(const Other& n) {
			this->mData = n;
			return *this;
		}

		Number<Interval<cln::cl_I>>& operator=(Number<Interval<cln::cl_I>>&& n) noexcept {
			this->mData = std::move(n.mData);
			return *this;
		}

		std::string toString() const {
			return mData.toString();
		}

		inline bool isZero() const {
			return mData.isZero();
		}


		inline bool isOne() const {
			return mData.isOne();
		}


		inline bool isPositive() const {
			return mData.isPositive();
		}


		inline bool isNegative() const {
			return mData.isNegative();
		}
	};


	template<>
	class Number<Interval<cln::cl_RA>> : public BaseNumber<Interval<cln::cl_RA>,Number<Interval<cln::cl_RA>>> {
	public:

		Number(): BaseNumber() {}
		explicit Number(const Interval<cln::cl_RA>& t): BaseNumber(t) {}
		explicit Number(Interval<cln::cl_RA>&& t): BaseNumber(t) {}
		Number(const Number<Interval<cln::cl_RA>>& n): BaseNumber(n) {}
		Number(Number<Interval<cln::cl_RA>>&& n) noexcept : BaseNumber(n) {}
		//explicit Number(const std::string& s) { mData = cln::cl_I(s.c_str()); }
		//Number(int n) : BaseNumber(n) {}
		//Number(long long int n) { mData = cln::cl_I(n); }
		//Number(unsigned long long int n) { mData = cln::cl_I(n);}

		
		Number<Interval<cln::cl_RA>>& operator=(const Number<Interval<cln::cl_RA>>& n) {
			this->mData = n.mData;
			return *this;
		}

		template<typename Other>
		Number<Interval<cln::cl_RA>>& operator=(const Other& n) {
			this->mData = n;
			return *this;
		}

		Number<Interval<cln::cl_RA>>& operator=(Number<Interval<cln::cl_RA>>&& n) noexcept {
			this->mData = std::move(n.mData);
			return *this;
		}

		std::string toString() const {
			return mData.toString();
		}

		inline bool isZero() const {
			return mData.isZero();
		}


		inline bool isOne() const {
			return mData.isOne();
		}


		inline bool isPositive() const {
			return mData.isPositive();
		}


		inline bool isNegative() const {
			return mData.isNegative();
		}
	};

	template<>
	class Number<Interval<mpq_class>> : public BaseNumber<Interval<mpq_class>,Number<Interval<mpq_class>>> {
	public:

		Number(): BaseNumber() {}
		explicit Number(const Interval<mpq_class>& t): BaseNumber(t) {}
		explicit Number(Interval<mpq_class>&& t): BaseNumber(t) {}
		Number(const Number<Interval<mpq_class>>& n): BaseNumber(n) {}
		Number(Number<Interval<mpq_class>>&& n) noexcept : BaseNumber(n) {}
		//explicit Number(const std::string& s) { mData = cln::cl_I(s.c_str()); }
		//Number(int n) : BaseNumber(n) {}
		//Number(long long int n) { mData = cln::cl_I(n); }
		//Number(unsigned long long int n) { mData = cln::cl_I(n);}

		
		Number<Interval<mpq_class>>& operator=(const Number<Interval<mpq_class>>& n) {
			this->mData = n.mData;
			return *this;
		}

		template<typename Other>
		Number<Interval<mpq_class>>& operator=(const Other& n) {
			this->mData = n;
			return *this;
		}

		Number<Interval<mpq_class>>& operator=(Number<Interval<mpq_class>>&& n) noexcept {
			this->mData = std::move(n.mData);
			return *this;
		}

		std::string toString() const {
			return mData.toString();
		}

		inline bool isZero() const {
			return mData.isZero();
		}


		inline bool isOne() const {
			return mData.isOne();
		}


		inline bool isPositive() const {
			return mData.isPositive();
		}


		inline bool isNegative() const {
			return mData.isNegative();
		}
	};

	template<>
	class Number<Interval<mpz_class>> : public BaseNumber<Interval<mpz_class>,Number<Interval<mpz_class>>> {
	public:

		Number(): BaseNumber() {}
		explicit Number(const Interval<mpz_class>& t): BaseNumber(t) {}
		explicit Number(Interval<mpz_class>&& t): BaseNumber(t) {}
		Number(const Number<Interval<mpz_class>>& n): BaseNumber(n) {}
		Number(Number<Interval<mpz_class>>&& n) noexcept : BaseNumber(n) {}
		//explicit Number(const std::string& s) { mData = cln::cl_I(s.c_str()); }
		//Number(int n) : BaseNumber(n) {}
		//Number(long long int n) { mData = cln::cl_I(n); }
		//Number(unsigned long long int n) { mData = cln::cl_I(n);}

		
		Number<Interval<mpz_class>>& operator=(const Number<Interval<mpz_class>>& n) {
			this->mData = n.mData;
			return *this;
		}

		template<typename Other>
		Number<Interval<mpz_class>>& operator=(const Other& n) {
			this->mData = n;
			return *this;
		}

		Number<Interval<mpz_class>>& operator=(Number<Interval<mpz_class>>&& n) noexcept {
			this->mData = std::move(n.mData);
			return *this;
		}

		std::string toString() const {
			return mData.toString();
		}

		inline bool isZero() const {
			return mData.isZero();
		}


		inline bool isOne() const {
			return mData.isOne();
		}


		inline bool isPositive() const {
			return mData.isPositive();
		}


		inline bool isNegative() const {
			return mData.isNegative();
		}
	};

	template<>
	class Number<Interval<double>> : public BaseNumber<Interval<double>,Number<Interval<double>>> {
	public:

		Number(): BaseNumber() {}
		explicit Number(const Interval<double>& t): BaseNumber(t) {}
		explicit Number(Interval<double>&& t): BaseNumber(t) {}
		Number(const Number<Interval<double>>& n): BaseNumber(n) {}
		Number(Number<Interval<double>>&& n) noexcept : BaseNumber(n) {}
		//explicit Number(const std::string& s) { mData = cln::cl_I(s.c_str()); }
		//Number(int n) : BaseNumber(n) {}
		//Number(long long int n) { mData = cln::cl_I(n); }
		//Number(unsigned long long int n) { mData = cln::cl_I(n);}

		
		Number<Interval<double>>& operator=(const Number<Interval<double>>& n) {
			this->mData = n.mData;
			return *this;
		}

		template<typename Other>
		Number<Interval<double>>& operator=(const Other& n) {
			this->mData = n;
			return *this;
		}

		Number<Interval<double>>& operator=(Number<Interval<double>>&& n) noexcept {
			this->mData = std::move(n.mData);
			return *this;
		}

		std::string toString() const {
			return mData.toString();
		}

		inline bool isZero() const {
			return mData.isZero();
		}


		inline bool isOne() const {
			return mData.isOne();
		}


		inline bool isPositive() const {
			return mData.isPositive();
		}


		inline bool isNegative() const {
			return mData.isNegative();
		}
	};

	template<>
	class Number<Interval<int>> : public BaseNumber<Interval<int>,Number<Interval<int>>> {
	public:

		Number(): BaseNumber() {}
		explicit Number(const Interval<int>& t): BaseNumber(t) {}
		explicit Number(Interval<int>&& t): BaseNumber(t) {}
		Number(const Number<Interval<int>>& n): BaseNumber(n) {}
		Number(Number<Interval<int>>&& n) noexcept : BaseNumber(n) {}
		//explicit Number(const std::string& s) { mData = cln::cl_I(s.c_str()); }
		//Number(int n) : BaseNumber(n) {}
		//Number(long long int n) { mData = cln::cl_I(n); }
		//Number(unsigned long long int n) { mData = cln::cl_I(n);}

		
		Number<Interval<int>>& operator=(const Number<Interval<int>>& n) {
			this->mData = n.mData;
			return *this;
		}

		template<typename Other>
		Number<Interval<int>>& operator=(const Other& n) {
			this->mData = n;
			return *this;
		}

		Number<Interval<int>>& operator=(Number<Interval<int>>&& n) noexcept {
			this->mData = std::move(n.mData);
			return *this;
		}

		std::string toString() const {
			return mData.toString();
		}

		inline bool isZero() const {
			return mData.isZero();
		}


		inline bool isOne() const {
			return mData.isOne();
		}


		inline bool isPositive() const {
			return mData.isPositive();
		}


		inline bool isNegative() const {
			return mData.isNegative();
		}
	};

}
