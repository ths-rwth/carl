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
#include <mutex>
#include "../core/VariablePool.h"
#include "../util/Singleton.h"

namespace carl
{   
    template<typename Poly> 
    class OldGinacConverter : public Singleton<OldGinacConverter<Poly>>
    {
        friend Singleton<OldGinacConverter>;
    private:
        std::recursive_mutex mMutex;
    public:
        
        OldGinacConverter() : Singleton<OldGinacConverter<Poly>>() {}
            
        ~OldGinacConverter() {}
        
        bool similar( const GiNaC::ex& a, const GiNaC::ex& b);

        GiNaC::ex convertToGinac(const Poly& poly, const std::map<carl::Variable, GiNaC::ex>& vars);

        Poly convertToCarl(const GiNaC::ex& _toConvert, const std::map<GiNaC::ex, carl::Variable, GiNaC::ex_is_less>& vars);

        void gatherVariables(const Poly& poly, std::map<Variable, GiNaC::ex>& carlToGinacVarMap, std::map<GiNaC::ex, Variable, GiNaC::ex_is_less>& ginacToCarlVarMap)
        {
            std::lock_guard<std::recursive_mutex> lock( mMutex );
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

        Poly ginacGcd(const Poly& polyA, const Poly& polyB);

        bool ginacDivide(const Poly& polyA, const Poly& polyB, Poly& result);

        std::unordered_map<const Poly, unsigned, std::hash<Poly>> ginacFactorization(const Poly& poly);

        bool checkConversion(const Poly& polyA);
    };

    template<typename Poly> 
    bool similar(const GiNaC::ex& a, const GiNaC::ex& b)
    {
        return OldGinacConverter<Poly>::getInstance().similar(a, b);
    }
    
	template<typename Poly> 
    void gatherVariables(const Poly& poly, std::map<Variable, GiNaC::ex>& carlToGinacVarMap, std::map<GiNaC::ex, Variable, GiNaC::ex_is_less>& ginacToCarlVarMap)
    {
        return OldGinacConverter<Poly>::getInstance().gatherVariables(poly, carlToGinacVarMap, ginacToCarlVarMap);
    }

    template<typename Poly>
	GiNaC::ex convertToGinac(const Poly& poly, const std::map<carl::Variable, GiNaC::ex>& vars)
    {
        return OldGinacConverter<Poly>::getInstance().convertToGinac(poly, vars);
    }
    
    template<typename Poly>
	Poly convertToCarl(const GiNaC::ex& _toConvert, const std::map<GiNaC::ex, carl::Variable, GiNaC::ex_is_less>& vars)
    {
        return OldGinacConverter<Poly>::getInstance().convertToCarl(_toConvert, vars);
    }
    
    template<typename Poly>
	Poly ginacGcd(const Poly& polyA, const Poly& polyB)
    {
        return OldGinacConverter<Poly>::getInstance().ginacGcd(polyA, polyB);
    }
    
    template<typename Poly>
	bool ginacDivide(const Poly& polyA, const Poly& polyB, Poly& result)
    {
        return OldGinacConverter<Poly>::getInstance().ginacDivide(polyA, polyB, result);
    }
    
    template<typename Poly>
	std::unordered_map<const Poly, unsigned, std::hash<Poly>> ginacFactorization(const Poly& poly)
    {
        return OldGinacConverter<Poly>::getInstance().ginacFactorization(poly);
    }
	
	template<typename Poly>
	bool checkConversion(const Poly& polyA)
    {
        return OldGinacConverter<Poly>::getInstance().checkConversion(polyA);
    }
}

#include "OldGinacConverter.tpp"

#endif

