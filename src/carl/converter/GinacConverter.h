/* 
 * File:   GinacConverter.h
 * Author: Florian Corzilius
 *
 * Created on December 13, 2013, 4:25 PM
 */

#pragma once

#include "config.h"

#ifdef COMPARE_WITH_GINAC
#include <unordered_map>
#include <ginac/ginac.h>
namespace carl
{
	bool similar(GiNaC::ex a, GiNaC::ex b);
}
#include "../core/MultivariatePolynomial.h"

namespace carl
{
    GiNaC::ex convertToGinac(const MultivariatePolynomial<cln::cl_RA>& poly, const std::map<carl::Variable, GiNaC::ex>& vars);
    
    MultivariatePolynomial<cln::cl_RA> convertToCarl(const GiNaC::ex& _toConvert, const std::map<GiNaC::ex, carl::Variable, GiNaC::ex_is_less>& vars);
    
    void gatherVariables(const MultivariatePolynomial<cln::cl_RA>& poly, std::map<Variable, GiNaC::ex>& carlToGinacVarMap, std::map<GiNaC::ex, Variable, GiNaC::ex_is_less>& ginacToCarlVarMap);
    
    MultivariatePolynomial<cln::cl_RA> ginacGcd(const MultivariatePolynomial<cln::cl_RA>& polyA, const MultivariatePolynomial<cln::cl_RA>& polyB);
    
    bool ginacDivide(const MultivariatePolynomial<cln::cl_RA>& polyA, const MultivariatePolynomial<cln::cl_RA>& polyB, MultivariatePolynomial<cln::cl_RA>& result);
    
    std::unordered_map<const MultivariatePolynomial<cln::cl_RA>, unsigned, std::hash<MultivariatePolynomial<cln::cl_RA>>> ginacFactorization(const MultivariatePolynomial<cln::cl_RA>& poly);
}

#endif

