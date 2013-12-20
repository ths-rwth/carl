/**
 * The implementation for the templated interval class.
 *
 * @file Interval.tpp
 * @autor Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 * 
 * @since	2013-12-20
 * @version 2013-12-20
 */

#include "Interval.h"

/*******************************************************************************
 * Constructors & Destructor
 ******************************************************************************/


template<typename DisableIf<std::is_same<Number, double>::value>>
Interval<Number>::Interval(double n) :
	mContent(Interval<Number>::BoostInterval(n),
	mLowerBoundType(BoundType::WEAK),
	mUpperBoundType(BoundType::WEAK)
{}

template<typename DisableIf<std::is_same<Number, double>::value>>
Interval<Number>::Interval(double lower, double upper) :
	mContent(Interval<Number>::BoostInterval(lower, upper),
	mLowerBoundType(BoundType::WEAK),
	mUpperBoundType(BoundType::WEAK)
{}

template<typename DisableIf<std::is_same<Number, double>::value>>
Interval<Number>::Interval(double lower, BoundType lowerBoundType, double upper, BoundType upperBoundType);

template<typename Ratinal>
Interval<Number>::Interval(Rational n);

template<typename Rational>
Interval<Number>::Interval(Rational lower, Rational upper);

template<typename Rational>
Interval<Number>::Interval(Rational lower, BoundType lowerBoundType, Rational upper, BoundType upperBoundType);

/*******************************************************************************
 * Transformations and advanced getters/setters
 ******************************************************************************/

template<typename Number>
Number Interval<Number>diameter() const;

template<typename Number>
void diameter_assign();

template<typename Number>
Number diameterRatio(const Interval<Number>& rhs) const;

template<typename Number>
void diameterRatio_assign(const Interval<Number>& rhs);

template<typename Number>
Number magnitude() const;

template<typename Number>
void magnitude_assign();

template<typename Number>
Number center() const;

template<typename Number>
void center_assign();

template<typename Number>
void bloat_by(const Number& width);

template<typename Number>
void bloat_times(const Number& factor);

template<typename Number>
void shrink_by(const Number& width);

template<typename Number>
void shrink_times(const Number& factor);

template<typename Number>
std::pair<Interval<Number>, Interval<Number>> split() const;

template<typename Number>
std::list<Interval<Number>> split(unsigned n) const;

/*******************************************************************************
 * Arithmetic functions
 ******************************************************************************/

template<typename Number>
Interval<Number>& add(const Interval<Number>& rhs) const;

template<typename Number>
void add_assign(const interval<Number>& rhs);

template<typename Number>
Interval<Number>& sub(const Interval<Number>& rhs) const;

template<typename Number>
void sub_assign(const Interval<Number>& rhs);

template<typename Number>
Interval<Number>& mul(const Interval<Number>& rhs) const;

template<typename Number>
void mul_assign(const Interval<Number>& rhs);

template<typename Number>
Interval<Number>& div(const Interval<Number>& rhs) const;

template<typename Number>
void div_assign(const Interval<Number>& rhs);

template<typename Number>
std::pair<Interval<Number>, Interval<Number>> div_ext(const Interval<Number>& rhs) const;

template<typename Number>
Interval<Number>& inverse() const;

template<typename Number>
void inverse_assign();

template<typename Number>
Interval<Number>& reciprocal() const;

template<typename Number>
void reciprocal_assign();

template<typename Number>
Interval<Number>& power(unsigned exp) const;

template<typename Number>
void power_assign(unsigned exp);

template<typename Number>
Interval<Number>& sqrt() const;

template<typename Number>
void sqrt_assign();

template<typename Number>
Interval<Number>& root(unsigned deg) const;

template<typename Number>
void root_assign(unsigned deg);

template<typename Number>
Interval<Number>& log() const;

template<typename Number>
void log_assign();

/*******************************************************************************
 * Trigonometric functions
 ******************************************************************************/

template<typename Number>
Interval<Number>& sin() const;

template<typename Number>
void sin_assign();

template<typename Number>
Interval<Number>& cos() const;

template<typename Number>
void cos_assign();

template<typename Number>
Interval<Number>& tan() const;

template<typename Number>
void tan_assign();

template<typename Number>
Interval<Number>& asin() const;

template<typename Number>
void asin_assign();

template<typename Number>
Interval<Number>& acos() const;

template<typename Number>
void acos_assign();

template<typename Number>
Interval<Number>& atan() const;

template<typename Number>
void atan_assign();

template<typename Number>
Interval<Number>& sinh() const;

template<typename Number>
void sinh_assign();

template<typename Number>
Interval<Number>& cosh() const;

template<typename Number>
void cosh_assign();

template<typename Number>
Interval<Number>& tanh() const;

template<typename Number>
void tanh_assign();

template<typename Number>
Interval<Number>& asinh() const;

template<typename Number>
void asinh_assign();

template<typename Number>
Interval<Number>& acosh() const;

template<typename Number>
void acosh_assign();

template<typename Number>
Interval<Number>& atanh() const;

template<typename Number>
void atanh_assign();

/*******************************************************************************
 * Overloaded arithmetics operators
 ******************************************************************************/

template<typename Number>
inline const Interval<Number> operator +(const Interval<Number>& lhs, const Interval<Number>& rhs);

template<typename Number>
inline const Interval<Number> operator +(const Number& lhs, const Interval<Number>& rhs);

template<typename Number>
inline const Interval<Number> operator +(const Interval<Number>& lhs, const Number& rhs);

template<typename Number>
inline const Interval<Number> operator -(const Interval<Number>& lhs, const Interval<Number>& rhs);

template<typename Number>
inline const Interval<Number> operator -(const Number& lhs, const Interval<Number>& rhs);

template<typename Number>
inline const Interval<Number> operator -(const Interval<Number>& lhs, const Number& rhs);

template<typename Number>
inline const Interval<Number> operator *(const Interval<Number>& lhs, const Interval<Number>& rhs);

template<typename Number>
inline const Interval<Number> operator *(const Number& lhs, const Interval<Number>& rhs);

template<typename Number>
inline const Interval<Number> operator *(const Interval<Number>& lhs, const Number& rhs);

template<typename Number>
inline const Interval<Number> operator /(const Interval<Number>& lhs, const Interval<Number>& rhs);

template<typename Number>
inline const Interval<Number> operator /(const Number& lhs, const Interval<Number>& rhs);

template<typename Number>
inline const Interval<Number> operator /(const Interval<Number>& lhs, const Number& rhs);

/*******************************************************************************
 * Comparison operators
 ******************************************************************************/

template<typename Number>
inline bool operator ==(const Interval<Number>& lhs, const Interval<Number>& rhs);

template<typename Number>
inline bool operator !=(const Interval<Number>& lhs, const Interval<Number>& rhs);

template<typename Number>
inline bool operator <=(const Interval<Number>& lhs, const Interval<Number>& rhs);

template<typename Number>
inline bool operator >=(const Interval<Number>& lhs, const Interval<Number>& rhs);

template<typename Number>
inline bool operator <(const Interval<Number>& lhs, const Interval<Number>& rhs);

template<typename Number>
inline bool operator >(const Interval<Number>& lhs, const Interval<Number>& rhs);

template<typename Number>
inline std::ostream& operator << <>(std::ostream& str, const INterval<Number>& i);