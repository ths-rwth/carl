#include "../FLOAT_T.h"
#ifdef USE_MPFR_FLOAT

template<>
class FLOAT_T<mpfr_t>
{
    private:
        mpfr_t mValue;

    public:

        /**
         * Constructors & Destructors
         */

        FLOAT_T()
        {
            mpfr_init(mValue);
            mpfr_set_zero(mValue, 1);
        }

        // Default precision is initially set to 53 bits in mpfr implementation
        FLOAT_T(const double _double, precision_t _prec=53, const CARL_RND _rnd=CARL_RND::N)
        {
            mpfr_init2(mValue,_prec);
            mpfr_set_d(mValue,_double,mpfr_rnd_t(_rnd));
        }

        // Default precision is initially set to 53 bits in mpfr implementation
        FLOAT_T(const float _float, precision_t _prec=53, const CARL_RND _rnd=CARL_RND::N)
        {
            mpfr_init2(mValue, _prec);
            mpfr_set_flt(mValue, _float, mpfr_rnd_t(_rnd));
        }

        // Default precision is initially set to 53 bits in mpfr implementation
        FLOAT_T(const int _int, precision_t _prec=53, const CARL_RND _rnd=CARL_RND::N)
        {
            mpfr_init2(mValue,_prec);
            mpfr_set_si(mValue,_int,mpfr_rnd_t(_rnd));
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
        
        template<typename F, DisableIf< std::is_same<F, mpfr_t> > = dummy>
        FLOAT_T(const FLOAT_T<F>& _float, precision_t _prec=53, const CARL_RND _rnd=CARL_RND::N)
        {
            mpfr_init2(mValue,_prec);
            mpfr_set_d(mValue,_float.toDouble(),mpfr_rnd_t(_rnd));
        }

        ~FLOAT_T()
        {
            mpfr_clear(mValue);
        }
        
        /*******************
         * Getter & Setter *
         *******************/
        
        const mpfr_t& getValue() const
        {
            std::cout << "getValue" << std::endl;
            return mValue;
        }
        
        precision_t getPrec() const
        {
            return mpfr_get_prec(mValue);
        }
        
        FLOAT_T<mpfr_t>& setPrec( const precision_t& _prec, const CARL_RND _rnd=CARL_RND::N )
        {
            mpfr_prec_round(mValue, convPrec(_prec), mpfr_rnd_t(_rnd));
            return *this;
        }
        
        
        /*************
         * Operators *
         *************/
        
        FLOAT_T<mpfr_t>& operator = (const FLOAT_T<mpfr_t>& _rhs)
        {
            if(this == &_rhs)
                return *this;
        		
            mpfr_set(mValue, _rhs.mValue, MPFR_RNDN);
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

        bool operator < ( const FLOAT_T<mpfr_t> & _rhs) const
        {
            return mpfr_less_p(mValue, _rhs.mValue) != 0;
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

        FLOAT_T<mpfr_t>& add_assign( const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd )
        {
            mpfr_add(mValue, mValue, _op2.mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void add( FLOAT_T<mpfr_t>& _result, const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd ) const
        {
            mpfr_add(_result.mValue, this->mValue, _op2.mValue, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& sub_assign( const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd )
        {
            mpfr_sub(mValue, mValue, _op2.mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void sub( FLOAT_T<mpfr_t>& _result, const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd ) const
        {
            mpfr_sub(_result.mValue, this->mValue, _op2.mValue, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& mul_assign(const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd)
        {
            mpfr_mul(mValue, mValue, _op2.mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void mul( FLOAT_T<mpfr_t>& _result, const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd) const
        {
            mpfr_mul(_result.mValue, this->mValue, _op2.mValue, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& div_assign(const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd)
        {
            assert( _op2 != 0 );
            mpfr_div(mValue, mValue, _op2.mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void div( FLOAT_T<mpfr_t>& _result, const FLOAT_T<mpfr_t>& _op2, CARL_RND _rnd) const
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

        void sqrt(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
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

        void cbrt(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
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

        void root(FLOAT_T<mpfr_t>& _result, unsigned long int _k, CARL_RND _rnd = CARL_RND::N)
        {
            assert( *this >= 0);
            mpfr_root(_result.mValue, mValue, _k, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& pow_assign(unsigned long int _exp, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_pow_ui(mValue, mValue, _exp, mpfr_rnd_t(_rnd));
            return *this;
        }

        void pow(FLOAT_T<mpfr_t>& _result, unsigned long int _exp, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_pow_ui(_result.mValue, mValue, _exp, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& abs_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_abs(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void abs(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_abs(_result.mValue, mValue, mpfr_rnd_t(_rnd));
        }
        
        FLOAT_T<mpfr_t>& exp_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_exp(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void exp(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_exp(_result.mValue, mValue, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& sin_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_sin(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void sin(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_sin(_result.mValue, mValue, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& cos_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_cos(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void cos(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_cos(_result.mValue, mValue, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& log_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_log(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void log(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_log(_result.mValue, mValue, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& tan_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_tan(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void tan(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_tan(_result.mValue, mValue, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& asin_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_asin(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void asin(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_asin(_result.mValue, mValue, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& acos_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_acos(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void acos(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_acos(_result.mValue, mValue, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& atan_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_atan(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void atan(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_atan(_result.mValue, mValue, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& sinh_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_sinh(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void sinh(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_sinh(_result.mValue, mValue, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& cosh_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_cosh(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void cosh(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_cosh(_result.mValue, mValue, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& tanh_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_tanh(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void tanh(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_tanh(_result.mValue, mValue, mpfr_rnd_t(_rnd));
        }

        FLOAT_T<mpfr_t>& asinh_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_asinh(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void asinh(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_asinh(_result.mValue, mValue, mpfr_rnd_t(_rnd));
        }
        
        FLOAT_T<mpfr_t>& acosh_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_acosh(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void acosh(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_acosh(_result.mValue, mValue, mpfr_rnd_t(_rnd));
        }
        
        FLOAT_T<mpfr_t>& atanh_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_atanh(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void atanh(FLOAT_T<mpfr_t>& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_atanh(_result.mValue, mValue, mpfr_rnd_t(_rnd));
        }

        void floor(int& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_t result;
            mpfr_init(result);
            mpfr_set_zero(result, 1);
            mpfr_rint_floor(result, mValue, mpfr_rnd_t(_rnd));
            _result = mpfr_integer_p(result);
            mpfr_clear(result);
        }

        FLOAT_T& floor_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_rint_floor(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }

        void ceil(int& _result, CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_t result;
            mpfr_init(result);
            mpfr_set_zero(result, 1);
            mpfr_rint_ceil(result, mValue, mpfr_rnd_t(_rnd));
            _result = mpfr_integer_p(result);
            mpfr_clear(result);
        }

        FLOAT_T& ceil_assign(CARL_RND _rnd = CARL_RND::N)
        {
            mpfr_rint_ceil(mValue, mValue, mpfr_rnd_t(_rnd));
            return *this;
        }
        
        /**
         * conversion operators
         */

        double toDouble(CARL_RND _rnd=CARL_RND::N) const
        {
            return mpfr_get_d(mValue, mpfr_rnd_t(_rnd));
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
	
        friend FLOAT_T<mpfr_t> operator -(const FLOAT_T<mpfr_t>& _lhs, const FLOAT_T<mpfr_t>& _rhs)
        {
            FLOAT_T<mpfr_t> res;
            mpfr_sub(res.mValue, _lhs.mValue, _rhs.mValue, MPFR_RNDN);
            return res;
        }
	
        friend FLOAT_T<mpfr_t> operator *(const FLOAT_T<mpfr_t>& _lhs, const FLOAT_T<mpfr_t>& _rhs)
        {
            FLOAT_T<mpfr_t> res;
            mpfr_mul(res.mValue, _lhs.mValue, _rhs.mValue, MPFR_RNDN);
            return res;
        }
	
        friend FLOAT_T<mpfr_t> operator /(const FLOAT_T<mpfr_t>& _lhs, const FLOAT_T<mpfr_t>& _rhs)
        {
            // TODO: mpfr_div results in infty when dividing by zero, although this should not be defined.
            FLOAT_T<mpfr_t> res;
            mpfr_div(res.mValue, _lhs.mValue, _rhs.mValue, MPFR_RNDN);
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

        FLOAT_T<mpfr_t>& operator -=(const FLOAT_T<mpfr_t>& _rhs)
        {
            mpfr_sub(mValue,mValue, _rhs.mValue, MPFR_RNDN);
            return *this;
        }

        FLOAT_T<mpfr_t>& operator *=(const FLOAT_T<mpfr_t>& _rhs)
        {
            mpfr_mul(mValue, mValue, _rhs.mValue, MPFR_RNDN);
            return *this;
        }

        FLOAT_T<mpfr_t>& operator /=(const FLOAT_T<mpfr_t>& _rhs)
        {
            // TODO: mpfr_div results in infty when dividing by zero, although this should not be defined.
            mpfr_div(mValue, mValue, _rhs.mValue, MPFR_RNDN);
            return *this;
        }
            
        /**
         * Auxiliary Functions
         */

        std::string toString() const
        {
            // TODO: Better rounding mode?
//            std::string out;
            char out[30];
//            str << mpfr_get_d(mValue, MPFR_RNDN);
            mpfr_sprintf(out, "%.10RDe", mValue);
            return std::string(out);
        }
        
    private:
        
        mpfr_prec_t convPrec(precision_t _prec) const
        {
            return _prec;
        }
};
#endif