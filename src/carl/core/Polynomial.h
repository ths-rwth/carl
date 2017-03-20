/** 
 * @file   Polynomial.h
 * @ingroup Polynomials
 * @author Sebastian Junges
 *
 */

#pragma once

namespace carl
{
/**
 * Abstract base class for polynomials.
 */
class Polynomial
{
public:
	/**
	 * Check if the polynomial is stored in a univariate representation.
     * @return If polynomial represented univariately.
     */
	virtual bool isUnivariateRepresented() const = 0;

	/**
	 * Check if the polynomial is stored in a multivariate representation.
     * @return If polynomial represented multivariately.
     */
	virtual bool isMultivariateRepresented() const = 0;

	/**
	 * Destructor.
     */
	virtual ~Polynomial() = default;
};
}
