/** 
 * @file:   Polynomial.h
 * @author: Sebastian Junges
 *
 * @since October 1, 2013
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
};