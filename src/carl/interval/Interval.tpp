/**
 * The implementation for the templated interval class.
 *
 * @file Interval.tpp
 * @autor Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 * 
 * @since	2013-12-20
 * @version 2013-12-20
 */
#pragma once
#include "Interval.h"

namespace carl
{

/*******************************************************************************
 * Constructors & Destructor
 ******************************************************************************/
/*
template<typename Rational, typename Number>
Interval<Number>::Interval(Rational n);

template<typename Rational, typename Number>
Interval<Number>::Interval(Rational lower, Rational upper);

template<typename Rational, typename Number>
Interval<Number>::Interval(Rational lower, BoundType lowerBoundType, Rational upper, BoundType upperBoundType);
*/
/*******************************************************************************
 * Transformations and advanced getters/setters
 ******************************************************************************/
	
template<typename Number>
Number Interval<Number>::diameter() const
	{
		return 0;
	}

template<typename Number>
void Interval<Number>::diameter_assign()
	{
		
	}

template<typename Number>
	Number Interval<Number>::diameterRatio(const Interval<Number>& rhs) const
	{}

template<typename Number>
	void Interval<Number>::diameterRatio_assign(const Interval<Number>& rhs)
	{}

template<typename Number>
	Number Interval<Number>::magnitude() const
	{}

template<typename Number>
	void Interval<Number>::magnitude_assign()
	{}

template<typename Number>
	Number Interval<Number>::center() const
	{}

template<typename Number>
	void Interval<Number>::center_assign()
	{}

template<typename Number>
	void Interval<Number>::bloat_by(const Number& width)
	{}

template<typename Number>
void Interval<Number>::bloat_times(const Number& factor)
	{}

template<typename Number>
void Interval<Number>::shrink_by(const Number& width)
	{}

template<typename Number>
void Interval<Number>::shrink_times(const Number& factor)
	{}

template<typename Number>
std::pair<Interval<Number>, Interval<Number>> Interval<Number>::split() const
	{}

template<typename Number>
std::list<Interval<Number>> Interval<Number>::split(unsigned n) const
	{}

/*******************************************************************************
 * Arithmetic functions
 ******************************************************************************/

template<typename Number>
Interval<Number>& Interval<Number>::add(const Interval<Number>& rhs) const
	{}

template<typename Number>
void Interval<Number>::add_assign(const Interval<Number>& rhs)
	{}

template<typename Number>
Interval<Number>& Interval<Number>::sub(const Interval<Number>& rhs) const
	{}

template<typename Number>
void Interval<Number>::sub_assign(const Interval<Number>& rhs)
	{}

template<typename Number>
Interval<Number>& Interval<Number>::mul(const Interval<Number>& rhs) const
	{}

template<typename Number>
void Interval<Number>::mul_assign(const Interval<Number>& rhs)
	{}

template<typename Number>
Interval<Number>& Interval<Number>::div(const Interval<Number>& rhs) const
	{}

template<typename Number>
void Interval<Number>::div_assign(const Interval<Number>& rhs)
	{}

template<typename Number>
std::pair<Interval<Number>, Interval<Number>> Interval<Number>::div_ext(const Interval<Number>& rhs) const
	{}

template<typename Number>
Interval<Number>& Interval<Number>::inverse() const
	{}

template<typename Number>
void Interval<Number>::inverse_assign()
	{}

template<typename Number>
Interval<Number>& Interval<Number>::reciprocal() const
	{}

template<typename Number>
	void Interval<Number>::reciprocal_assign()
	{}

template<typename Number>
Interval<Number>& Interval<Number>::power(unsigned exp) const
	{}

template<typename Number>
void Interval<Number>::power_assign(unsigned exp)
	{}

template<typename Number>
Interval<Number>& Interval<Number>::sqrt() const
	{}

template<typename Number>
void Interval<Number>::sqrt_assign()
	{}

template<typename Number>
Interval<Number>& Interval<Number>::root(unsigned deg) const
	{}

template<typename Number>
void Interval<Number>::root_assign(unsigned deg)
	{}

template<typename Number>
Interval<Number>& Interval<Number>::log() const
	{}

template<typename Number>
void Interval<Number>::log_assign()
	{}

/*******************************************************************************
 * Trigonometric functions
 ******************************************************************************/

template<typename Number>
Interval<Number>& Interval<Number>::sin() const
	{}

template<typename Number>
void Interval<Number>::sin_assign()
	{}

template<typename Number>
Interval<Number>& Interval<Number>::cos() const
	{}

template<typename Number>
void Interval<Number>::cos_assign()
	{}

template<typename Number>
Interval<Number>& Interval<Number>::tan() const
	{}

template<typename Number>
void Interval<Number>::tan_assign()
	{}

template<typename Number>
Interval<Number>& Interval<Number>::asin() const
	{}

template<typename Number>
void Interval<Number>::asin_assign()
	{}

template<typename Number>
Interval<Number>& Interval<Number>::acos() const
	{}

template<typename Number>
void Interval<Number>::acos_assign()
	{}

template<typename Number>
Interval<Number>& Interval<Number>::atan() const
	{}

template<typename Number>
void Interval<Number>::atan_assign()
	{}

template<typename Number>
Interval<Number>& Interval<Number>::sinh() const
	{}

template<typename Number>
void Interval<Number>::sinh_assign()
	{}

template<typename Number>
Interval<Number>& Interval<Number>::cosh() const
	{}

template<typename Number>
void Interval<Number>::cosh_assign()
	{}

template<typename Number>
Interval<Number>& Interval<Number>::tanh() const
	{}

template<typename Number>
void Interval<Number>::tanh_assign()
	{}

template<typename Number>
Interval<Number>& Interval<Number>::asinh() const
	{}

template<typename Number>
void Interval<Number>::asinh_assign()
	{}

template<typename Number>
Interval<Number>& Interval<Number>::acosh() const
	{}

template<typename Number>
void Interval<Number>::acosh_assign()
	{}

template<typename Number>
Interval<Number>& Interval<Number>::atanh() const
	{}

template<typename Number>
void Interval<Number>::atanh_assign()
	{}

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
inline std::ostream& operator <<(std::ostream& str, const Interval<Number>& i);
}