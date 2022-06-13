#include "FLOAT_T.h"


#ifdef USE_MPFR_FLOAT
namespace carl {


template<>
class FLOAT_T<mpfr_t>
{
	private:
		mpfr_t mValue;
		static precision_t mDefaultPrecision;

	public:

		/**
		 * Constructors & Destructors
		 */

		FLOAT_T()
		{
			mpfr_init2(mValue, mDefaultPrecision);
			mpfr_set_zero(mValue, 1);
		}

		// Default precision is initially set to 53 bits in mpfr implementation
		FLOAT_T(const double _double, const CARL_RND _rnd=CARL_RND::N, precision_t _prec=mDefaultPrecision)
		{
			mpfr_init2(mValue,_prec);
			mpfr_set_d(mValue,_double,mpfr_rnd_t(_rnd));
		}

		// Default precision is initially set to 53 bits in mpfr implementation
		FLOAT_T(const float _float, const CARL_RND _rnd=CARL_RND::N, precision_t _prec=mDefaultPrecision)
		{
			mpfr_init2(mValue,_prec);
			mpfr_set_flt(mValue, _float, mpfr_rnd_t(_rnd));
		}

		// Default precision is initially set to 53 bits in mpfr implementation
		FLOAT_T(const int _int, const CARL_RND _rnd=CARL_RND::N, precision_t _prec=mDefaultPrecision)
		{
			mpfr_init2(mValue,_prec);
			mpfr_set_si(mValue,_int,mpfr_rnd_t(_rnd));
		}

		// Default precision is initially set to 53 bits in mpfr implementation
		FLOAT_T(const unsigned _int, const CARL_RND _rnd=CARL_RND::N, precision_t _prec=mDefaultPrecision)
		{
			mpfr_init2(mValue,_prec);
			mpfr_set_ui(mValue,_int,mpfr_rnd_t(_rnd));
		}

		// Default precision is initially set to 53 bits in mpfr implementation

		FLOAT_T(const long _long, const CARL_RND _rnd=CARL_RND::N, precision_t _prec=mDefaultPrecision)
		{
			mpfr_init2(mValue,_prec);
			mpfr_set_si(mValue,_long,mpfr_rnd_t(_rnd));
		}

		// Default precision is initially set to 53 bits in mpfr implementation
		FLOAT_T(const unsigned long _long, const CARL_RND _rnd=CARL_RND::N, precision_t _prec=mDefaultPrecision)
		{
			mpfr_init2(mValue,_prec);
			mpfr_set_ui(mValue,_long,mpfr_rnd_t(_rnd));
		}

		FLOAT_T(const mpfr_t& _mpfrNumber)
		{
			mpfr_init2(mValue,mpfr_get_prec(_mpfrNumber));
			mpfr_set(mValue, _mpfrNumber, MPFR_RNDN);
		}

		FLOAT_T(const FLOAT_T<mpfr_t>& _float)
		{
			mpfr_init2(mValue, mpfr_get_prec(_float.mValue));
			mpfr_set(mValue, _float.mValue, MPFR_RNDN);
		}

		FLOAT_T(FLOAT_T<mpfr_t>&& _float)
		{
			if(this->mValue == _float.value()){
				mpfr_swap(mValue,_float.mValue);
			} else {
				mpfr_init2(mValue, mpfr_get_prec(_float.value()));
				mpfr_swap(mValue,_float.mValue);
			}
		}

		FLOAT_T(const std::string& _string)
		{
			mpfr_init_set_str(mValue, _string.c_str(), 10, MPFR_RNDN);
		}

		template<typename F, DisableIf< std::is_same<F, mpfr_t> > = dummy>
		FLOAT_T(const FLOAT_T<F>& _float, const CARL_RND _rnd=CARL_RND::N, precision_t _prec=mDefaultPrecision)
		{
			mpfr_init2(mValue,_prec);
			mpfr_set_d(mValue,_float.to_double(),mpfr_rnd_t(_rnd));
		}

		~FLOAT_T()
		{
			mpfr_clear(mValue);
		}

		static inline const FLOAT_T<mpfr_t> const_infinity(){
			mpfr_t tmp;
			mpfr_init2(tmp, FLOAT_T<mpfr_t>::defaultPrecision());
			mpfr_set_inf(tmp,1);
			return FLOAT_T<mpfr_t>(tmp);
		}

		inline const FLOAT_T<mpfr_t> setNan(){
			mpfr_set_nan(mValue);
			return *this;
		}

		inline FLOAT_T<mpfr_t> increment() const {
			FLOAT_T<mpfr_t> tmp(*this);
			mpfr_nextabove(tmp.mValue);
			return tmp;
		}

		inline FLOAT_T<mpfr_t> decrement() const {
			FLOAT_T<mpfr_t> tmp(*this);
			mpfr_nextbelow(tmp.mValue);
			return tmp;
		}

		static inline FLOAT_T<mpfr_t> machine_epsilon(precision_t prec)
		{
			/* the smallest eps such that 1 + eps != 1 */
			return machine_epsilon(FLOAT_T<mpfr_t>(1,CARL_RND::N, prec));
		}

		static inline FLOAT_T<mpfr_t> machine_epsilon(const FLOAT_T<mpfr_t>& x)
		{
			/* the smallest eps such that x + eps != x */
			FLOAT_T<mpfr_t> tmp(x);
			if( x < 0)
			{
				return -tmp.increment() + tmp;
			}else{
				return tmp.increment() - tmp;
			}
		}

		static inline carl::FLOAT_T<mpfr_t> minval(const CARL_RND _rnd=CARL_RND::N, precision_t prec=FLOAT_T<mpfr_t>::defaultPrecision()) {
			/* min = 1/2 * 2^emin = 2^(emin - 1) */
			mpfr_t tmp;
			mpfr_init2(tmp,prec);
			mpfr_set_ui(tmp,1,mpfr_rnd_t(_rnd));
			mpfr_mul_2ui(tmp,tmp,(unsigned)mpfr_get_emin()-1,mpfr_rnd_t(_rnd));
			return FLOAT_T<mpfr_t>(tmp);
		}

		static inline FLOAT_T<mpfr_t> maxval(const CARL_RND _rnd=CARL_RND::N, precision_t prec=FLOAT_T<mpfr_t>::defaultPrecision())
		{
			/* max = (1 - eps) * 2^emax, eps is machine epsilon */
			mpfr_t tmp;
			mpfr_init2(tmp,prec);
			mpfr_set_ui(tmp,1, mpfr_rnd_t(_rnd));
			mpfr_sub(tmp,tmp,machine_epsilon(prec).value(), mpfr_rnd_t(_rnd));
			mpfr_mul_2ui(tmp,tmp,(unsigned)mpfr_get_emax(),mpfr_rnd_t(_rnd));
			return (FLOAT_T<mpfr_t>(tmp));
		}

		/*******************
		 * Getter & Setter *
		 *******************/

		const mpfr_t& value() const
		{
			return mValue;
		}

		static void setDefaultPrecision(precision_t _prec) {
			mDefaultPrecision = _prec;
		}

		static precision_t defaultPrecision () {
			return mDefaultPrecision;
		}

		precision_t precision() const
		{
			return mpfr_get_prec(mValue);
		}

		FLOAT_T<mpfr_t>& setPrecision( const precision_t& _prec, const CARL_RND _rnd=CARL_RND::N )
		{
			mpfr_prec_round(mValue, convPrec(_prec), mpfr_rnd_t(_rnd));
			return *this;
		}

		/*************
		 * Operators *
		 *************/

		FLOAT_T<mpfr_t>& operator = (const FLOAT_T<mpfr_t>& _rhs)
		{
			if(this->mValue == _rhs.value())
				return *this;

			// Note: This is a workaround to get the limb size correct. Instead use free and reallocate.
			mpfr_set_prec(mValue, mpfr_get_prec(_rhs.mValue));
			mpfr_set(mValue, _rhs.mValue, MPFR_RNDN);

			/*
			mValue->_mpfr_prec = _rhs.mValue->_mpfr_prec;
			mValue->_mpfr_sign = _rhs.mValue->_mpfr_sign;
			mValue->_mpfr_exp = _rhs.mValue->_mpfr_exp;

			// deep-copy limbs
			std::size_t limbs = (std::size_t)std::ceil(double(_rhs.mValue->_mpfr_prec)/double(mp_bits_per_limb));

			while( limbs > 0 ){
				mValue->_mpfr_d[limbs-1] = _rhs.mValue->_mpfr_d[limbs-1];
				--limbs;
			}
			*/
			return *this;
		}

		FLOAT_T<mpfr_t>& safeSet (const FLOAT_T<mpfr_t>& _rhs, const CARL_RND _rnd=CARL_RND::N)
		{
			mpfr_set_prec(mValue, mpfr_get_prec(_rhs.mValue));
			mpfr_set(mValue, _rhs.mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		/**
		 * Boolean operators
		 */

		bool operator == ( const FLOAT_T<mpfr_t>& _rhs) const
		{
			return mpfr_cmp(mValue,_rhs.mValue) == 0;
		}

		bool operator != ( const FLOAT_T<mpfr_t> & _rhs) const
		{
			return mpfr_equal_p(mValue, _rhs.mValue) == 0;
		}

		bool operator > ( const FLOAT_T<mpfr_t> & _rhs) const
		{
			return mpfr_greater_p(mValue, _rhs.mValue) != 0;
		}

	bool operator > ( int _rhs) const
		{
			return mpfr_cmp_si(mValue, _rhs) > 0;
		}

		bool operator > ( unsigned _rhs) const
		{
			return mpfr_cmp_ui(mValue, _rhs) > 0;
		}

		bool operator < ( const FLOAT_T<mpfr_t> & _rhs) const
		{
			return mpfr_less_p(mValue, _rhs.mValue) != 0;
		}

		bool operator < ( int _rhs) const
		{
			return mpfr_cmp_si(mValue, _rhs) < 0;
		}

		bool operator < ( unsigned _rhs) const
		{
			return mpfr_cmp_ui(mValue, _rhs) < 0;
		}

		bool operator <= ( const FLOAT_T<mpfr_t> & _rhs) const
		{
			return mpfr_lessequal_p(mValue, _rhs.mValue) != 0;
		}

		bool operator >= ( const FLOAT_T<mpfr_t> & _rhs) const
		{
			return mpfr_greaterequal_p(mValue, _rhs.mValue) != 0;
		}

		/**
		 * arithmetic operations
		 */

		FLOAT_T<mpfr_t>& add_assign( const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd=CARL_RND::N )
		{
			mpfr_add(mValue, mValue, _op2.mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void add( FLOAT_T<mpfr_t>& _result, const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd=CARL_RND::N ) const
		{
			mpfr_add(_result.mValue, this->mValue, _op2.mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& sub_assign( const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd=CARL_RND::N )
		{
			mpfr_sub(mValue, mValue, _op2.mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void sub( FLOAT_T<mpfr_t>& _result, const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd=CARL_RND::N ) const
		{
			mpfr_sub(_result.mValue, this->mValue, _op2.mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& mul_assign(const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd=CARL_RND::N)
		{
			mpfr_mul(mValue, mValue, _op2.mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void mul( FLOAT_T<mpfr_t>& _result, const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd=CARL_RND::N) const
		{
			mpfr_mul(_result.mValue, this->mValue, _op2.mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& div_assign(const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd=CARL_RND::N)
		{
			assert( _op2 != 0 );
			mpfr_div(mValue, mValue, _op2.mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void div( FLOAT_T<mpfr_t>& _result, const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd=CARL_RND::N) const
		{
			assert( _op2 != 0 );
			mpfr_div(_result.mValue, this->mValue, _op2.mValue, mpfr_rnd_t(_rnd));
		}

		/**
		 * special operators
		 */

		FLOAT_T<mpfr_t>& sqrt_assign(CARL_RND _rnd = CARL_RND::N)
		{
			assert( *this >= 0);
			mpfr_sqrt(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void sqrt(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			assert( *this >= 0);
			mpfr_sqrt(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& cbrt_assign(CARL_RND _rnd = CARL_RND::N)
		{
			assert( *this >= 0);
			mpfr_cbrt(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void cbrt(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			assert( *this >= 0);
			mpfr_cbrt(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& root_assign(unsigned long int _k, CARL_RND _rnd = CARL_RND::N)
		{
			assert( *this >= 0);
			mpfr_root(mValue, mValue, _k, mpfr_rnd_t(_rnd));
			return *this;
		}

		void root(FLOAT_T<mpfr_t>& _result, unsigned long int _k, CARL_RND _rnd = CARL_RND::N) const
		{
			assert( *this >= 0);
			mpfr_root(_result.mValue, mValue, _k, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& pow_assign(unsigned long int _exp, CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_pow_ui(mValue, mValue, _exp, mpfr_rnd_t(_rnd));
			return *this;
		}

		void pow(FLOAT_T<mpfr_t>& _result, unsigned long int _exp, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_pow_ui(_result.mValue, mValue, _exp, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& abs_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_abs(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void abs(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_abs(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& exp_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_exp(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void exp(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_exp(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& sin_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_sin(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void sin(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_sin(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& cos_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_cos(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void cos(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_cos(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& log_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_log(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void log(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_log(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& tan_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_tan(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void tan(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_tan(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& asin_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_asin(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void asin(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_asin(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& acos_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_acos(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void acos(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_acos(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& atan_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_atan(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void atan(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_atan(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& sinh_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_sinh(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void sinh(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_sinh(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& cosh_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_cosh(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void cosh(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_cosh(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& tanh_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_tanh(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void tanh(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_tanh(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& asinh_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_asinh(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void asinh(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_asinh(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& acosh_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_acosh(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void acosh(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_acosh(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		FLOAT_T<mpfr_t>& atanh_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_atanh(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void atanh(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_atanh(_result.mValue, mValue, mpfr_rnd_t(_rnd));
		}

		void floor(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_init(_result.mValue);
			mpfr_set_zero(_result.mValue, 1);
			mpfr_rint_floor(_result.mValue, mValue, mpfr_rnd_t(_rnd));
			//_result = mpfr_integer_p(result);
			//mpfr_clear(result);
		}

		FLOAT_T& floor_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_rint_floor(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		void ceil(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N) const
		{
			mpfr_init(_result.mValue);
			mpfr_set_zero(_result.mValue, 1);
			mpfr_rint_ceil(_result.mValue, mValue, mpfr_rnd_t(_rnd));
			//_result = mpfr_integer_p(result);
			//mpfr_clear(result);
		}

		FLOAT_T& ceil_assign(CARL_RND _rnd = CARL_RND::N)
		{
			mpfr_rint_ceil(mValue, mValue, mpfr_rnd_t(_rnd));
			return *this;
		}

		/**
		 * conversion operators
		 */

		double to_double(CARL_RND _rnd=CARL_RND::N) const
		{
			return mpfr_get_d(mValue, mpfr_rnd_t(_rnd));
		}

		/**
		 * Explicit typecast operator to integer.
		 * @return Integer representation of this.
		 */
		explicit operator int() const
		{
			return (int)this->to_double();
		}

		/**
		 * Explicit typecast operator to long.
		 * @return Long representation of this.
		 */
		explicit operator long() const
		{
			return (long)(this->to_double());
		}

		/**
		 * Explicit typecast operator to double.
		 * @return Double representation of this.
		 */
		explicit operator double() const
		{
			return this->to_double();
		}


		friend std::ostream & operator<< (std::ostream& ostr, const FLOAT_T<mpfr_t> & p) {
			ostr << p.toString();
			return ostr;
		}



		friend bool operator== (const FLOAT_T<mpfr_t>& _lhs, const int _rhs)
		{
			return mpfr_cmp_si(_lhs.mValue, _rhs) == 0;
		}

		friend bool operator== (const int _lhs, const FLOAT_T<mpfr_t>& _rhs)
		{
			return _rhs == _lhs;
		}

		friend bool operator== (const FLOAT_T<mpfr_t>& _lhs, const double _rhs)
		{
			return mpfr_cmp_d(_lhs.mValue,_rhs) == 0;
		}

		friend bool operator== (const double _lhs, const FLOAT_T<mpfr_t>& _rhs)
		{
			return _rhs == _lhs;
		}

		friend bool operator== (const FLOAT_T<mpfr_t>& _lhs, const float _rhs)
		{
			return mpfr_cmp_d(_lhs.mValue, _rhs);
		}

		friend bool operator== (const float _lhs, const FLOAT_T<mpfr_t>& _rhs)
		{
			return _rhs == _lhs;
		}

		/**
		* Operators
		*/

		friend FLOAT_T<mpfr_t> operator +(const FLOAT_T<mpfr_t>& _lhs, const FLOAT_T<mpfr_t>& _rhs)
		{
			FLOAT_T<mpfr_t> res;
			mpfr_add(res.mValue, _lhs.mValue, _rhs.mValue, MPFR_RNDN);
			return res;
		}

		friend FLOAT_T<mpfr_t> operator +(const mpfr_t& _lhs, const FLOAT_T<mpfr_t>& _rhs)
		{
			FLOAT_T<mpfr_t> res;
			mpfr_add(res.mValue, _lhs, _rhs.mValue, MPFR_RNDN);
			return res;
		}

		friend FLOAT_T<mpfr_t> operator +(const FLOAT_T<mpfr_t>& _lhs, const mpfr_t& _rhs)
		{
			FLOAT_T<mpfr_t> res;
			mpfr_add(res.mValue, _lhs.mValue, _rhs, MPFR_RNDN);
			return res;
		}

		template<typename Other, EnableIf< is_number_type<Other>, Not<is_interval_type<Other>> > = dummy>
		friend FLOAT_T<mpfr_t> operator +(const Other& _lhs, const FLOAT_T<mpfr_t>& _rhs)
		{
			FLOAT_T<mpfr_t> res;
			mpfr_add(res.mValue, FLOAT_T<mpfr_t>(_lhs).mValue, _rhs.mValue, MPFR_RNDN);
			return res;
		}

		template<typename Other, EnableIf< is_number_type<Other>, Not<is_interval_type<Other>> > = dummy>
		friend FLOAT_T<mpfr_t> operator +(const FLOAT_T<mpfr_t>& _lhs, const Other& _rhs)
		{
			return _rhs+_lhs;
		}

		friend FLOAT_T<mpfr_t> operator -(const FLOAT_T<mpfr_t>& _lhs, const FLOAT_T<mpfr_t>& _rhs)
		{
			FLOAT_T<mpfr_t> res;
			mpfr_sub(res.mValue, _lhs.mValue, _rhs.mValue, MPFR_RNDN);
			return res;
		}

		friend FLOAT_T<mpfr_t> operator -(const mpfr_t& _lhs, const FLOAT_T<mpfr_t>& _rhs)
		{
			FLOAT_T<mpfr_t> res;
			mpfr_sub(res.mValue, _lhs, _rhs.mValue, MPFR_RNDN);
			return res;
		}

		friend FLOAT_T<mpfr_t> operator -(const FLOAT_T<mpfr_t>& _lhs, const mpfr_t& _rhs)
		{
			FLOAT_T<mpfr_t> res;
			mpfr_sub(res.mValue, _lhs.mValue, _rhs, MPFR_RNDN);
			return res;
		}

		template<typename Other, EnableIf< is_number_type<Other>, Not<is_interval_type<Other>> > = dummy>
		friend FLOAT_T<mpfr_t> operator -(const Other& _lhs, const FLOAT_T<mpfr_t>& _rhs)
		{
			FLOAT_T<mpfr_t> res;
			mpfr_sub(res.mValue, FLOAT_T<mpfr_t>(_lhs).mValue, _rhs.mValue, MPFR_RNDN);
			return res;
		}

		template<typename Other, EnableIf< is_number_type<Other>, Not<is_interval_type<Other>> > = dummy>
		friend FLOAT_T<mpfr_t> operator -(const FLOAT_T<mpfr_t>& _lhs, const Other& _rhs)
		{
			FLOAT_T<mpfr_t> res;
			mpfr_sub(res.mValue, _lhs.mValue, FLOAT_T<mpfr_t>(_rhs).mValue, MPFR_RNDN);
			return res;
		}

		friend FLOAT_T<mpfr_t> operator -(const FLOAT_T<mpfr_t>& _lhs)
		{
			FLOAT_T<mpfr_t> res;
			mpfr_mul_si(res.mValue, _lhs.mValue, -1, MPFR_RNDN);
			return res;
		}

		friend FLOAT_T<mpfr_t> operator *(const FLOAT_T<mpfr_t>& _lhs, const FLOAT_T<mpfr_t>& _rhs)
		{
			FLOAT_T<mpfr_t> res;
			mpfr_mul(res.mValue, _lhs.mValue, _rhs.mValue, MPFR_RNDN);
			return res;
		}

		template<typename Other, EnableIf< is_number_type<Other>, Not<is_interval_type<Other>> > = dummy>
		friend FLOAT_T<mpfr_t> operator *(const Other& _lhs, const FLOAT_T<mpfr_t>& _rhs)
		{
			FLOAT_T<mpfr_t> res;
			FLOAT_T<mpfr_t> lhs = FLOAT_T<mpfr_t>(_lhs);
			mpfr_mul(res.mValue, lhs.mValue, _rhs.mValue, MPFR_RNDN);
			return res;
		}

		template<typename Other, EnableIf< is_number_type<Other>, Not<is_interval_type<Other>> > = dummy>
		friend FLOAT_T<mpfr_t> operator *(const FLOAT_T<mpfr_t>& _lhs, const Other& _rhs)
		{
			return _rhs*_lhs;
		}

		friend FLOAT_T<mpfr_t> operator *(const mpfr_t& _lhs, const FLOAT_T<mpfr_t>& _rhs)
		{
			FLOAT_T<mpfr_t> res;
			mpfr_mul(res.mValue, _lhs, _rhs.mValue, MPFR_RNDN);
			return res;
		}

		friend FLOAT_T<mpfr_t> operator *(const FLOAT_T<mpfr_t>& _lhs, const mpfr_t& _rhs)
		{
			FLOAT_T<mpfr_t> res;
			mpfr_mul(res.mValue, _lhs.mValue, _rhs, MPFR_RNDN);
			return res;
		}

		friend FLOAT_T<mpfr_t> operator /(const FLOAT_T<mpfr_t>& _lhs, const FLOAT_T<mpfr_t>& _rhs)
		{
			// TODO: mpfr_div results in infty when dividing by zero, although this should not be defined.
			FLOAT_T<mpfr_t> res;
			mpfr_div(res.mValue, _lhs.mValue, _rhs.mValue, MPFR_RNDN);
			return res;
		}

		friend FLOAT_T<mpfr_t> operator /(const mpfr_t& _lhs, const FLOAT_T<mpfr_t>& _rhs)
		{
			// TODO: mpfr_div results in infty when dividing by zero, although this should not be defined.
			FLOAT_T<mpfr_t> res;
			mpfr_div(res.mValue, _lhs, _rhs.mValue, MPFR_RNDN);
			return res;
		}

		friend FLOAT_T<mpfr_t> operator /(const FLOAT_T<mpfr_t>& _lhs, const mpfr_t& _rhs)
		{
			// TODO: mpfr_div results in infty when dividing by zero, although this should not be defined.
			FLOAT_T<mpfr_t> res;
			mpfr_div(res.mValue, _lhs.mValue, _rhs, MPFR_RNDN);
			return res;
		}

		template<typename Other, EnableIf< is_number_type<Other>, Not<is_interval_type<Other>> > = dummy>
		friend FLOAT_T<mpfr_t> operator /(const Other& _lhs, const FLOAT_T<mpfr_t>& _rhs)
		{
			// TODO: mpfr_div results in infty when dividing by zero, although this should not be defined.
			FLOAT_T<mpfr_t> res;
			mpfr_div(res.mValue, FLOAT_T<mpfr_t>(_lhs).mValue, _rhs.mValue, MPFR_RNDN);
			return res;
		}

		template<typename Other, EnableIf< is_number_type<Other>, Not<is_interval_type<Other>> > = dummy>
		friend FLOAT_T<mpfr_t> operator /(const FLOAT_T<mpfr_t>& _lhs, const Other& _rhs)
		{
			// TODO: mpfr_div results in infty when dividing by zero, although this should not be defined.
			FLOAT_T<mpfr_t> res;
			mpfr_div(res.mValue, _lhs.mValue, FLOAT_T<mpfr_t>(_rhs).mValue, MPFR_RNDN);
			return res;
		}

		friend FLOAT_T<mpfr_t>& operator++(FLOAT_T<mpfr_t>& _num)
		{
			mpfr_add_ui(_num.mValue, _num.mValue, 1, MPFR_RNDN);
			return _num;
		}

		friend FLOAT_T<mpfr_t>& operator--(FLOAT_T<mpfr_t>& _num)
		{
			mpfr_sub_ui(_num.mValue, _num.mValue, 1, MPFR_RNDN);
			return _num;
		}

		FLOAT_T<mpfr_t>& operator +=(const FLOAT_T<mpfr_t>& _rhs)
		{
			mpfr_add(mValue, mValue, _rhs.mValue, MPFR_RNDN);
			return *this;
		}

		FLOAT_T<mpfr_t>& operator +=(const mpfr_t& _rhs)
		{
			mpfr_add(mValue, mValue, _rhs, MPFR_RNDN);
			return *this;
		}

		FLOAT_T<mpfr_t>& operator -=(const FLOAT_T<mpfr_t>& _rhs)
		{
			mpfr_sub(mValue,mValue, _rhs.mValue, MPFR_RNDN);
			return *this;
		}

		FLOAT_T<mpfr_t>& operator -=(const mpfr_t& _rhs)
		{
			mpfr_sub(mValue,mValue, _rhs, MPFR_RNDN);
			return *this;
		}

		FLOAT_T<mpfr_t> operator-()
		{
			FLOAT_T<mpfr_t> res;
			mpfr_mul_si(res.mValue, this->mValue, -1, MPFR_RNDN);
			return res;
		}

		FLOAT_T<mpfr_t>& operator *=(const FLOAT_T<mpfr_t>& _rhs)
		{
			mpfr_mul(mValue, mValue, _rhs.mValue, MPFR_RNDN);
			return *this;
		}

		FLOAT_T<mpfr_t>& operator *=(const mpfr_t& _rhs)
		{
			mpfr_mul(mValue, mValue, _rhs, MPFR_RNDN);
			return *this;
		}

		FLOAT_T<mpfr_t>& operator /=(const FLOAT_T<mpfr_t>& _rhs)
		{
			// TODO: mpfr_div results in infty when dividing by zero, although this should not be defined.
			mpfr_div(mValue, mValue, _rhs.mValue, MPFR_RNDN);
			return *this;
		}

		FLOAT_T<mpfr_t>& operator /=(const mpfr_t& _rhs)
		{
			// TODO: mpfr_div results in infty when dividing by zero, although this should not be defined.
			mpfr_div(mValue, mValue, _rhs, MPFR_RNDN);
			return *this;
		}

		/**
		 * Auxiliary Functions
		 */

		std::string toString() const
		{
			// TODO: Better rounding mode?
//            std::string out;
			char out[80];
//            str << mpfr_get_d(mValue, MPFR_RNDN);
			mpfr_sprintf(out, "%.50RDe", mValue);
			return std::string(out);
		}

		static void integerDistance(const FLOAT_T<mpfr_t>& a, const FLOAT_T<mpfr_t>& b, mpz_t& dist) {
			distance(a.value(), b.value(), dist);
		}

		inline static std::size_t bits2digits(precision_t b) {
			const double LOG10_2 = 0.30102999566398119;
			return std::size_t(std::floor( double(b) * LOG10_2 ));
		}

	private:

		void clear() {
			mpfr_clear(mValue);
		}

		mpfr_prec_t convPrec(precision_t _prec) const
		{
			return _prec;
		}

		/**
		 * @brief Converts a given float to its integer representation
		 * @details This function converts a mpfr float to a corresponding integer representation. This is done as follows:
		 * We can use a transformation similar to the one used for c++ doubles (with modifications). We use the mantissa and extend
		 * it by the exponent (we left-shift the exponent in front of the mantissa) and add the sign. This can be interpreted as an
		 * integer. Note that zero is a special case, which should be handled separately.
		 *
		 * @param intRep Parameter where we write the integer to.
		 * @param a input mpfr float.
		 */
		static void to_int(mpz_t& intRep, const mpfr_t a) {

			//std::cout << "Bits per limb " << mp_bits_per_limb << std::endl;
			//std::cout << "Number limbs " << std::ceil(double(a->_mpfr_prec)/double(mp_bits_per_limb)) << std::endl;
			//std::cout << "Precision is " << a->_mpfr_prec << std::endl;
			//std::cout << "Sign is " << a->_mpfr_sign << std::endl;
			//std::cout << "Exponent is " << carl::binary(a->_mpfr_exp) << std::endl;
			//std::cout << "Exponent is " << a->_mpfr_exp << std::endl;
			//std::cout << "Min Exponent is " << mpfr_get_emin() << std::endl;
			//std::cout << "Min Exponent is " << carl::binary(mpfr_get_emin()) << std::endl;
			//std::cout << "Scaled exponent: " << (a->_mpfr_exp + std::abs(mpfr_get_emin())) << std::endl;
			//std::cout << "Scaled exponent: " << carl::binary((a->_mpfr_exp + std::abs(mpfr_get_emin()))) << std::endl;

			// mpfr mantissa is stored in limbs (usually 64-bit words) - the number of those depends on the precision.
			std::size_t limbs = (std::size_t)std::ceil(double(a->_mpfr_prec)/double(mp_bits_per_limb));
			/*
			std::cout << "Mantissa is ";
			while( limbs > 0 ){
				std::cout << carl::binary(h->_mpfr_d[limbs-1]) << " " << std::endl;
				--limbs;
			}
			std::cout << std::endl;
			limbs = std::ceil(double(h->_mpfr_prec)/double(mp_bits_per_limb));
			*/
			mpz_t mant;
			mpz_t tmp;
			mpz_init(mant);
			mpz_init(tmp);
			mpz_set_ui(mant,0);
			mpz_set_ui(tmp,0);
			// as mpfr uses whole limbs (64-bit) we can cut away the additional zeroes (offset), if there are any
			long offset = mp_bits_per_limb - (a->_mpfr_prec % mp_bits_per_limb);
			//std::cout << "Offset is " << offset << " bits" << std::endl;
			//std::cout << "Mantissa is ";
			//char outStr[1024];

			// assemble the integer representation of the mantissa. The limbs are stored in reversed order, least significant first.
			while( limbs > 0 ){
				mpz_set_ui(tmp, a->_mpfr_d[limbs-1]);
				//std::cout << "Shift: " << (mp_bits_per_limb*(limbs-1)) << " bits" << std::endl;
				mpz_mul_2exp(tmp, tmp, ((std::size_t)mp_bits_per_limb*(limbs-1)));
				mpz_add(mant, mant, tmp);
				--limbs;
			}
			// cut away unnecessary zeroes at the end (divide by 2^offset -> left-shift).
			mpz_cdiv_q_2exp(mant, mant, offset);

			//mpz_get_str(outStr, 2,mant);
			//std::cout << "Mantissa: " << std::string(outStr) << std::endl;

			// set the exponent (8-bit), as it is in 2s complement, subtract the minimum to shift the exponent and get exponents ordered,
			// right shift to put it before the mantissa later.
			mpz_set_ui(tmp, (a->_mpfr_exp + std::abs(mpfr_get_emin())));
			//std::cout << "Shift by " << (std::ceil(double(h->_mpfr_prec)/double(mp_bits_per_limb))*64+64-offset) << " bits" << std::endl;
			mpz_mul_2exp(tmp,tmp,a->_mpfr_prec);

			// assemble the whole representation by addition and finally add sign.
			mpz_add(mant,mant,tmp);
			mpz_mul_si(mant,mant,a->_mpfr_sign);
			mpz_set(intRep, mant);
			/*
			mpz_get_str(outStr, 2,mant);
			std::cout << std::string(outStr) << std::endl;
			mpz_get_str(outStr, 10,mant);
			std::cout << std::string(outStr) << std::endl;
			*/

			// cleanup.
			mpz_clear(mant);
			mpz_clear(tmp);
		}

		/**
		 * @brief Calculates the distance in ULPs between two mpfr floats a and b.
		 * @details We use an integer representation for calculating the distance. Special cases are whenever we reach or cross
		 * zero. TODO: Include cases for NaN and INFTY.
		 *
		 * @param a
		 * @param b
		 *
		 * @return [description]
		 */
		static void distance(const mpfr_t& a, const mpfr_t& b, mpz_t& dist) {
			// initialize variables
			mpz_t intRepA;
			mpz_t intRepB;
			mpz_init(intRepA);
			mpz_init(intRepB);

			// the offset is used to cope with the exponent differences
			long offset = a->_mpfr_exp - b->_mpfr_exp;
			//std::cout << "Offset " << offset << std::endl;

			// get integer representations, we use absolute values for simplicity.
			to_int(intRepA, a);
			to_int(intRepB, b);
			mpz_abs(intRepA, intRepA);
			mpz_abs(intRepB, intRepB);

			// case distinction to cope with zero.
			if(mpfr_zero_p(a) != 0) { // a is zero
				if(mpfr_zero_p(b) != 0){ // b is also zero
					mpz_set_ui(dist,0);
				}else{

					// b is not zero -> we compute the distance from close to zero to b and add 1.
					mpfr_t zero;
					mpz_t intRepZero;
					mpfr_init2(zero,mpfr_get_prec(a));
					mpz_init(intRepZero);

					mpfr_set_ui(zero,0, MPFR_RNDZ);
					if(b->_mpfr_sign > 0) {
						mpfr_nextabove(zero);
					}else{
						mpfr_nextbelow(zero);
					}

					to_int(intRepZero, zero);
					mpz_abs(intRepZero, intRepZero);
					mpz_sub(dist, intRepB,intRepZero);
					mpz_add_ui(dist,dist, 1);

					mpfr_clear(zero);
					mpz_clear(intRepZero);
				}
			} else if(mpfr_zero_p(b) != 0) { // a is not zero, b is zero
				mpfr_t zero;
				mpz_t intRepZero;
				mpfr_init2(zero,mpfr_get_prec(a));
				mpz_init(intRepZero);

				mpfr_set_ui(zero,0, MPFR_RNDZ);
				if(a->_mpfr_sign > 0) {
					mpfr_nextabove(zero);
				}else{
					mpfr_nextbelow(zero);
				}

				to_int(intRepZero, zero);
				mpz_abs(intRepZero, intRepZero);
				mpz_sub(dist, intRepA,intRepZero);
				mpz_add_ui(dist,dist, 1);

				mpfr_clear(zero);
				mpz_clear(intRepZero);
			} else if(a->_mpfr_sign == b->_mpfr_sign) { // both are not zero and at the same side
				mpz_sub(dist, intRepA, intRepB);
				mpz_abs(dist,dist);
			} else { // both are not zero and one is larger, the other one is less zero, compute both dists to zero and add 2.
				mpfr_t zeroA;
				mpfr_init2(zeroA,mpfr_get_prec(a));
				mpfr_t zeroB;
				mpfr_init2(zeroB,mpfr_get_prec(a));

				mpfr_set_ui(zeroA,0, MPFR_RNDZ);
				mpfr_set_ui(zeroB,0, MPFR_RNDZ);

				if(a->_mpfr_sign > 0) {
					mpfr_nextabove(zeroA);
					mpfr_nextbelow(zeroB);
				}else{
					mpfr_nextbelow(zeroA);
					mpfr_nextabove(zeroB);
				}
				mpz_t intRepZeroA;
				mpz_init(intRepZeroA);
				mpz_t intRepZeroB;
				mpz_init(intRepZeroB);
				mpz_t d2;
				mpz_init(d2);

				to_int(intRepZeroA, zeroA);
				mpz_abs(intRepZeroA, intRepZeroA);
				to_int(intRepZeroB, zeroB);
				mpz_abs(intRepZeroB, intRepZeroB);

				mpz_sub(dist, intRepA,intRepZeroA);
				mpz_sub(d2, intRepB,intRepZeroB);
				mpz_add(dist, dist, d2);
				mpz_add_ui(dist,dist, 2);

				mpfr_clear(zeroA);
				mpfr_clear(zeroB);
				mpz_clear(intRepZeroA);
				mpz_clear(intRepZeroB);
				mpz_clear(d2);
			}
			//std::cout << "Modify by " << 2*std::abs(offset)*a->_mpfr_prec << std::endl;

			// shift by offset (exponent differences).
			long shift = 2*std::abs(offset)*unsigned(a->_mpfr_prec);
			mpz_sub_ui(dist, dist, shift);
			// cleanup.
			mpz_clear(intRepA);
			mpz_clear(intRepB);
		}
};

template<>
inline bool isInfinity(const FLOAT_T<mpfr_t>& _in) {
	return (mpfr_inf_p(_in.value()) != 0);
}

template<>
inline bool isNan(const FLOAT_T<mpfr_t>& _in) {
	return (mpfr_nan_p(_in.value()) != 0);
}

template<>
inline bool AlmostEqual2sComplement<FLOAT_T<mpfr_t>>(const FLOAT_T<mpfr_t>& A, const FLOAT_T<mpfr_t>& B, unsigned maxUlps)
{
	//std::cout << "Distance: " << FLOAT_T<mpfr_t>::integerDistance(A,B) << std::endl;
	mpz_t distance;
	mpz_init(distance);
	FLOAT_T<mpfr_t>::integerDistance(A,B,distance);
	mpz_sub_ui(distance, distance, maxUlps);
	bool less = (mpz_sgn(distance) <= 0);
	mpz_clear(distance);
	return less;
}

}// namespace

namespace std{

	template<>
	struct hash<carl::FLOAT_T<mpfr_t>> {
		size_t operator()(const carl::FLOAT_T<mpfr_t>& _in) const {

			__mpfr_struct numStruct = *_in.value();
			std::size_t limbs = (std::size_t)std::ceil(double(numStruct._mpfr_prec)/double(mp_bits_per_limb));

			size_t seed = 0;
			//std::cout << "start hash" << std::endl;
			//std::cout << "seed: " << seed << std::endl;
			if(mpfr_number_p(_in.value()) && mpfr_zero_p(_in.value()) == 0){
				while(limbs > 0) {
				    carl::hash_add(seed,numStruct._mpfr_d[limbs-1]);
				    //std::cout << "seed: " << seed << std::endl;
				    --limbs;
				}
			}
			//std::cout << "seed: " << seed << std::endl;
			if(mpfr_nan_p(_in.value()) == 0){
				carl::hash_add(seed, size_t(numStruct._mpfr_sign));
			}
			//std::cout << "seed: " << seed << std::endl;
			carl::hash_add(seed, size_t(numStruct._mpfr_prec));
			//std::cout << "seed: " << seed << std::endl;
			carl::hash_add(seed, size_t(numStruct._mpfr_exp));
			//std::cout << "seed: " << seed << std::endl;
			//std::cout << "end hash" << std::endl;
			return seed;
		}
	};

	template<>
	class numeric_limits<carl::FLOAT_T<mpfr_t>>
	{
	public:
		static const bool is_specialized	= true;
		static const bool is_signed			= true;
		static const bool is_integer		= false;
		static const bool is_exact			= false;
		static const int  radix				= 2;

		static const bool has_infinity		= true;
		static const bool has_quiet_NaN		= true;
		static const bool has_signaling_NaN	= true;

		static const bool is_iec559			= true;	// = IEEE 754
		static const bool is_bounded		= true;
		static const bool is_modulo			= false;
		static const bool traps				= true;
		static const bool tinyness_before	= true;

		static const float_denorm_style has_denorm  = denorm_absent;


		inline static carl::FLOAT_T<mpfr_t> (min) (carl::CARL_RND _rnd = carl::CARL_RND::N, carl::precision_t precision = carl::FLOAT_T<mpfr_t>::defaultPrecision()) { return  carl::FLOAT_T<mpfr_t>::minval(_rnd, precision); }
		inline static carl::FLOAT_T<mpfr_t> (max) (carl::CARL_RND _rnd = carl::CARL_RND::N, carl::precision_t precision = carl::FLOAT_T<mpfr_t>::defaultPrecision()) { return  carl::FLOAT_T<mpfr_t>::maxval(_rnd, precision); }
		inline static carl::FLOAT_T<mpfr_t> lowest (carl::CARL_RND _rnd = carl::CARL_RND::N, carl::precision_t precision = carl::FLOAT_T<mpfr_t>::defaultPrecision()) { return -carl::FLOAT_T<mpfr_t>::maxval(_rnd, precision); }

		// Returns smallest eps such that 1 + eps != 1 (classic machine epsilon)
		inline static carl::FLOAT_T<mpfr_t> epsilon(carl::precision_t precision = carl::FLOAT_T<mpfr_t>::defaultPrecision()) {  return  carl::FLOAT_T<mpfr_t>::machine_epsilon(precision); }

		// Returns smallest eps such that x + eps != x (relative machine epsilon)
		inline static carl::FLOAT_T<mpfr_t> epsilon(const carl::FLOAT_T<mpfr_t>& x) { return carl::FLOAT_T<mpfr_t>::machine_epsilon(x); }

		inline static carl::FLOAT_T<mpfr_t> round_error(carl::CARL_RND _rnd = carl::CARL_RND::N, carl::precision_t precision = carl::FLOAT_T<mpfr_t>::defaultPrecision()) { return carl::FLOAT_T<mpfr_t>(0.5, _rnd, precision); }

		inline static const carl::FLOAT_T<mpfr_t> infinity() { return carl::FLOAT_T<mpfr_t>::const_infinity(); }

		inline static const carl::FLOAT_T<mpfr_t> quiet_NaN() { return carl::FLOAT_T<mpfr_t>().setNan(); }
		inline static const carl::FLOAT_T<mpfr_t> signaling_NaN() { return carl::FLOAT_T<mpfr_t>().setNan(); }
		inline static const carl::FLOAT_T<mpfr_t> denorm_min() { return (min)(); }

		// Please note, exponent range is not fixed in MPFR
		static const int min_exponent = MPFR_EMIN_DEFAULT;
		static const int max_exponent = MPFR_EMAX_DEFAULT;
		static const int min_exponent10 = (int) (MPFR_EMIN_DEFAULT * 0.3010299956639811);
		static const int max_exponent10 = (int) (MPFR_EMAX_DEFAULT * 0.3010299956639811);

		// Following members should be constant according to standard, but they can be variable in MPFR
		// So we define them as functions here.
		//
		// This is preferable way for std::numeric_limits<carl::FLOAT_T<mpfr_t>> specialization.
		// But it is incompatible with standard std::numeric_limits and might not work with other libraries, e.g. boost.
		// See below for compatible implementation.
		inline static float_round_style round_style() { return round_to_nearest; }

		inline static std::size_t digits() { return std::size_t(carl::FLOAT_T<mpfr_t>::defaultPrecision()); }
		inline static std::size_t digits( const carl::FLOAT_T<mpfr_t>& x ) { return std::size_t(x.precision()); }

		inline static std::size_t digits10( carl::precision_t precision = carl::FLOAT_T<mpfr_t>::defaultPrecision() ) { return carl::FLOAT_T<mpfr_t>::bits2digits(precision); }
		inline static std::size_t digits10( const carl::FLOAT_T<mpfr_t>& x ) { return carl::FLOAT_T<mpfr_t>::bits2digits(x.precision()); }
		inline static std::size_t max_digits10( carl::precision_t precision = carl::FLOAT_T<mpfr_t>::defaultPrecision() ) { return digits10(precision); }
	};
}

#endif
