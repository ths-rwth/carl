/** 
 * @file   Polynomial.h
 * @ingroup Polynomials
 * @author Sebastian Junges
 *
 */

#pragma once

/**
 * Abstract base class for polynomials.
 */
class Polynomial
{
public:
	virtual bool isUnivariateRepresented() const = 0;
	virtual bool isMultivariateRepresented() const = 0;
	virtual ~Polynomial() {}
};