/** 
 * @file:   numbers.h
 * @author: Sebastian Junges
 *
 * @since August 26, 2013
 */

#pragma once
#include <climits>
#include <cln/cln.h>
#include <gmpxx.h>
#include <functional>
#include <array>
#include <map>
#include "../util/SFINAE.h"

namespace carl
{

// 
// Forward declarations
//
template<typename IntegerT>
class GFNumber;

template<typename C>
class UnivariatePolynomial;

template<typename C, typename O, typename P>
class MultivariatePolynomial;
	
//
// Type traits.
// For type traits, we use the notation conventions of std, being lower cases with underscores.
//

/**
 * Type trait  is_field. 
 * Default is false, but certain types which encode algebraic fields should be set to true. 
 * @see UnivariatePolynomial - CauchyBound for example.
 */
template<typename type>
struct is_field
{
	static const bool value = false;
};

template<>
struct is_field<cln::cl_RA>
{
	static const bool value = true;
};

template<>
struct is_field<mpq_class>
{
	static const bool value = true;
};

/**
 * Type trait is_fraction. 
 * Default is false, but certain types which encode fractions should be set to true. 
 * Note that we consider integral types to be fractional.
 */
template<typename type>
struct is_fraction
{
	static const bool value = false;
};

template<>
struct is_fraction<cln::cl_I>
{
	static const bool value = true;
};
template<>
struct is_fraction<cln::cl_RA>
{
	static const bool value = true;
};

template<>
struct is_fraction<mpz_class>
{
	static const bool value = true;
};
template<>
struct is_fraction<mpq_class>
{
	static const bool value = true;
};

/**
 * Type trait for the characteristic of the given field (template argument).
 * Default is 0, but certain types which encode algebraic fields should be set to true. 
 * @see UnivariatePolynomial - squareFreeFactorization for example.
 */
template<typename type>
struct characteristic
{
	static const unsigned value = 0;
};

template<typename C>
struct is_field<GFNumber<C>>
{
	static const bool value = true;
};


template<typename C>
struct is_finite_domain
{
	static constexpr bool value = false;
};

template<typename C>
struct is_finite_domain<GFNumber<C>>
{
	static constexpr bool value = true;
};

/**
 * Type trait is_number
 */
template<typename type>
struct is_number
{
	static const bool value = false;
};

template<>
struct is_number<int>
{
	static const bool value = true;
};

template<>
struct is_number<cln::cl_RA>
{
	static const bool value = true;
};

template<>
struct is_number<cln::cl_I>
{
	static const bool value = true;
};

template<>
struct is_number<mpq_class>
{
	static const bool value = true;
};

template<>
struct is_number<mpz_class>
{
	static const bool value = true;
};

template<typename RationalType>
struct IntegralT
{
	typedef int type;
};


template<>
struct IntegralT<cln::cl_RA>
{
	typedef cln::cl_I type;
};

template<>
struct IntegralT<cln::cl_I>
{
	typedef cln::cl_I type;
};

template<>
struct IntegralT<mpq_class>
{
	typedef mpz_class type;
};

template<>
struct IntegralT<mpz_class>
{
	typedef mpz_class type;
};

template<typename C>
struct IntegralT<GFNumber<C>>
{
	typedef C type;
};

/**
 * Coefficient ring of numbers is just the type of the number. (TODO limit this to numbers)
 */
template<typename C>
struct CoefficientRing
{
	typedef C type;
};

template<typename C>
struct CoefficientRing<UnivariatePolynomial<C>>
{
	typedef C type;
};

template<typename C, typename O, typename P>
struct CoefficientRing<MultivariatePolynomial<C, O, P>>
{
	typedef C type;
};


/**
 * Obtains the underlying number type of a polynomial type.
 */
template<typename C>
struct UnderlyingNumberType
{
	typedef C type;
};

template<typename C>
struct UnderlyingNumberType<UnivariatePolynomial<C>>
{
	typedef typename UnderlyingNumberType<C>::type type;
};

template<typename C, typename O, typename P>
struct UnderlyingNumberType<MultivariatePolynomial<C, O, P>>
{
	typedef typename UnderlyingNumberType<C>::type type;
};


inline cln::cl_I getNum(const cln::cl_RA& rat)
{
	return cln::numerator(rat);
}

inline cln::cl_I getDenom(const cln::cl_RA& rat)
{
	return cln::denominator(rat);
}

inline cln::cl_I gcd(const cln::cl_I& a, const cln::cl_I& b)
{
	return cln::gcd(a,b);
}

inline cln::cl_I lcm(const cln::cl_I& a, const cln::cl_I& b)
{
	return cln::lcm(a,b);
}


inline cln::cl_RA pow(const cln::cl_RA& base, unsigned exp)
{
	return cln::expt(base, (int)exp);
}

inline double getDouble(const cln::cl_RA& rational)
{
	return cln::double_approx(rational);
}

template<typename t>
inline t rationalize(double d)
{
	return cln::rationalize(cln::cl_R(d));
}

template<>
inline cln::cl_RA rationalize<cln::cl_RA>(double d)
{
	return cln::rationalize(cln::cl_R(d));
}

inline mpz_class abs(const mpz_class& i)
{
	mpz_class res;
	mpz_abs(res.get_mpz_t(), i.get_mpz_t());
	return res;
}

inline mpq_class abs(const mpq_class& r)
{
	mpq_class res;
	mpq_abs(res.get_mpq_t(), r.get_mpq_t());
	return res;
}

template<typename i>
inline i toInt(const mpz_class&)
{
	return (i) 0;
}

template<>
inline signed long int toInt(const mpz_class& i)
{
    assert(i <= INT_MAX);
    return mpz_get_si(i.get_mpz_t());
}

template<>
inline unsigned long int toInt(const mpz_class& i)
{
    assert(i <= UINT_MAX);
    return mpz_get_ui(i.get_mpz_t());
}

template<typename i>
inline i toInt(const cln::cl_I&)
{
	return (i) 0;
}

template<>
inline int toInt(const cln::cl_I& i)
{
    assert(i <= INT_MAX);
    return cln::cl_I_to_int(i);
}

template<>
inline unsigned toInt(const cln::cl_I& i)
{
    assert(i <= UINT_MAX);
    return cln::cl_I_to_uint(i);
}

inline const mpz_class& getNum(const mpq_class& rat)
{
	return rat.get_num();
}

inline const mpz_class& getDenom(const mpq_class& rat)
{
	return rat.get_den();
}

inline double getDouble(const mpq_class& rational)
{
	return rational.get_d();
}

template<>
inline mpq_class rationalize<mpq_class>(double d)
{
	return mpq_class(d);
}

inline double getDouble(const int& rational)
{
	return double(rational);
}

inline mpz_class pow(const mpz_class& b, unsigned e)
{
	mpz_class res;
	mpz_pow_ui(res.get_mpz_t(), b.get_mpz_t(), e);
	return res;
}

inline void divide(const mpz_class& dividend, const mpz_class& divisor, mpz_class& quotient, mpz_class& remainder)
{
	mpz_divmod(quotient.get_mpz_t(), remainder.get_mpz_t(), dividend.get_mpz_t(), divisor.get_mpz_t());
}

inline mpz_class mod(const mpz_class& n, const mpz_class& m)
{
	mpz_class res;
	mpz_mod(res.get_mpz_t(), n.get_mpz_t(), m.get_mpz_t());
	return res;
}

inline mpz_class gcd(const mpz_class& v1, const mpz_class& v2)
{
	mpz_class res;
	mpz_gcd(res.get_mpz_t(), v1.get_mpz_t(), v2.get_mpz_t());
	return res;
}

inline mpz_class lcm(const mpz_class& v1, const mpz_class& v2)
{
	mpz_class res;
	mpz_lcm(res.get_mpz_t(), v1.get_mpz_t(), v2.get_mpz_t());
	return res;
} 

inline long mod(const long& n, const long& m)
{
	return n % m;
}

inline unsigned long mod(const unsigned long& n, const unsigned long& m)
{
	return n % m;
}

inline unsigned mod(const unsigned& n, const unsigned& m)
{
	return n % m;
}

inline int mod(const int& n, const int& m)
{
	return n % m;
}

inline cln::cl_I mod(const cln::cl_I& n, const cln::cl_I& m)
{
	return cln::mod(n, m);
}


template<typename C>
inline bool isInteger(const GFNumber<C>&)
{
	return true;
}


inline bool isInteger(const mpq_class& r)
{
	 return 0 != mpz_divisible_p(r.get_num_mpz_t(), r.get_den_mpz_t());
}

inline bool isInteger(const mpz_class&)
{
	return true;
}

inline bool isInteger(const cln::cl_RA& rat)
{
	return cln::denominator(rat) == cln::cl_I(1);
}

inline bool isInteger(const cln::cl_I&)
{
	return true;
}

} // namespace carl    

namespace std
{

template<>
struct hash<cln::cl_RA>
{
	size_t operator()(const cln::cl_RA& cln_rational) const
	{
		return cln::equal_hashcode(cln_rational);
	}
};

template<>
struct hash<mpq_t>
{
	size_t operator()(const mpq_t& gmp_rational) const
	{
		return mpz_get_ui(mpq_numref(gmp_rational)) ^ mpz_get_ui(mpq_denref(gmp_rational));
	}
};
} // namespace std
