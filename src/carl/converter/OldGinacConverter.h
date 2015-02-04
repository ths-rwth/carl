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
#include "../core/VariablePool.h"

namespace carl
{

	bool similar(GiNaC::ex a, GiNaC::ex b);
    
	template<typename Poly> void gatherVariables(const Poly& poly, std::map<Variable, GiNaC::ex>& carlToGinacVarMap, std::map<GiNaC::ex, Variable, GiNaC::ex_is_less>& ginacToCarlVarMap);

    template<typename Poly>
	GiNaC::ex convertToGinac(const Poly& poly, const std::map<carl::Variable, GiNaC::ex>& vars);
    
    template<typename Poly>
	Poly convertToCarl(const GiNaC::ex& _toConvert, const std::map<GiNaC::ex, carl::Variable, GiNaC::ex_is_less>& vars);
    
	template<typename Poly>
	void gatherVariables(const Poly& poly, std::map<Variable, GiNaC::ex>& carlToGinacVarMap, std::map<GiNaC::ex, Variable, GiNaC::ex_is_less>& ginacToCarlVarMap)
	{
        std::set<Variable> carlVars;
        poly.gatherVariables(carlVars);
        for(auto var = carlVars.begin(); var != carlVars.end(); ++var)
        {
            GiNaC::symbol vg(VariablePool::getInstance().getName(*var));
            if( carlToGinacVarMap.insert(std::pair<Variable, GiNaC::ex>(*var, vg)).second )
            {
                ginacToCarlVarMap.insert(std::pair<GiNaC::ex, Variable>(vg, *var));
            }
        }
	}
    
    template<typename Poly>
	Poly ginacGcd(const Poly& polyA, const Poly& polyB);
    
    template<typename Poly>
	bool ginacDivide(const Poly& polyA, const Poly& polyB, Poly& result);
    
    template<typename Poly>
	std::unordered_map<const Poly, unsigned, std::hash<Poly>> ginacFactorization(const Poly& poly);
	
	template<typename Poly>
	bool checkConversion(const Poly& polyA);
}

#include "OldGinacConverter.tpp"

#endif

