/* 
 * File:   GinacConverter.h
 * Author: Florian Corzilius
 *
 * Created on December 13, 2013, 4:25 PM
 */

#pragma once

#include <carl-common/config.h>

#ifdef USE_GINAC

#include "../core/VariablePool.h"
#include <carl/core/Variables.h>
#include <carl/numbers/numbers.h>
#include <carl/core/Common.h>
#include <carl-common/meta/SFINAE.h>
#include <carl-common/memory/Singleton.h>
#include "ginac.h"

#include <cln/cln.h>
#include <mutex>
#include <unordered_map>

namespace carl
{   
    template<typename Poly> 
    class OldGinacConverter : public Singleton<OldGinacConverter<Poly>>
    {
        friend Singleton<OldGinacConverter>;
    private:
        std::recursive_mutex mMutex;
        std::shared_ptr<typename Poly::CACHE> mpPolynomialCache;
    public:
        ~OldGinacConverter() = default;
        
        void setPolynomialCache( const std::shared_ptr<typename Poly::CACHE>& _cache )
        {
            mpPolynomialCache = _cache;
        }
        
        template<typename P = Poly, EnableIf<needs_cache<P>> = dummy>
        P createPolynomial( typename P::PolyType&& _poly )
        {
            return P( std::move(_poly), mpPolynomialCache );
        }

        template<typename P = Poly, EnableIf<needs_cache<P>> = dummy>
        P createPolynomial( Variable::Arg _var )
        {
            return P( std::move(typename P::PolyType(_var)), mpPolynomialCache );
        }

        template<typename P = Poly, EnableIf<needs_cache<P>> = dummy>
        P createPolynomial( const typename P::PolyType& _poly )
        {
            return createPolynomial(typename P::PolyType(_poly));
        }

        template<typename P = Poly, DisableIf<needs_cache<P>> = dummy>
        P createPolynomial( Variable::Arg _var )
        {
            return P( _var );
        }
        
        bool similar( const GiNaC::ex& a, const GiNaC::ex& b);

        GiNaC::ex convertToGinac(const typename Poly::PolyType& poly, const std::map<carl::Variable, GiNaC::ex>& vars)
        {
            std::lock_guard<std::recursive_mutex> lock( mMutex );
            GiNaC::ex result = 0;
            for(auto term = poly.begin(); term != poly.end(); ++term)
            {
                GiNaC::ex factor = GiNaC::ex( GiNaC::numeric( carl::convert<typename Poly::PolyType::CoeffType, cln::cl_RA>(term->coeff()) ) );
                if((*term).monomial())
                {
                    for (auto it: *(term->monomial())) {
                        auto carlToGinacVar = vars.find(it.first);
                        assert(carlToGinacVar != vars.end());
                        factor *= GiNaC::pow(carlToGinacVar->second, static_cast<unsigned long>(it.second));
                    }
                }
                result += factor;
            }
            return result;
        }

        template<typename P = Poly, EnableIf<is_factorized<P>> = dummy>
        GiNaC::ex convertToGinac(const P& poly, const std::map<carl::Variable, GiNaC::ex>& vars)
        {
            std::lock_guard<std::recursive_mutex> lock( mMutex );
            if( existsFactorization( poly ) )
            {
                if( poly.factorizedTrivially() )
                    return convertToGinac( poly.polynomial(), vars );
                GiNaC::ex result = GiNaC::ex( GiNaC::numeric( carl::rationalize<cln::cl_RA>(PreventConversion<typename Poly::PolyType::CoeffType>( poly.coefficient() ))));
                for( const auto& factor : poly.factorization() )
                {
                    result *= GiNaC::pow(convertToGinac( factor.first, vars ), factor.second );
                }
                return result;
            }
            return GiNaC::ex( GiNaC::numeric( carl::rationalize<cln::cl_RA>(PreventConversion<typename Poly::PolyType::CoeffType>( poly.coefficient() ))));
        }

        Poly convertToCarl(const GiNaC::ex& _toConvert, const std::map<GiNaC::ex, carl::Variable, GiNaC::ex_is_less>& vars);

        void gatherVariables(const Poly& poly, std::map<Variable, GiNaC::ex>& carlToGinacVarMap, std::map<GiNaC::ex, Variable, GiNaC::ex_is_less>& ginacToCarlVarMap)
        {
            std::lock_guard<std::recursive_mutex> lock( mMutex );
			for (auto var: carl::variables(poly)) {
                GiNaC::symbol vg(var.name());
                if( carlToGinacVarMap.emplace(var, vg).second )
                {
                    ginacToCarlVarMap.emplace(vg, var);
                }
            }
        }

        Poly ginacGcd(const Poly& polyA, const Poly& polyB);

        bool ginacDivide(const Poly& polyA, const Poly& polyB, Poly& result);

        Factors<Poly> ginacFactorization(const Poly& poly);

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
	Factors<Poly> ginacFactorization(const Poly& poly)
    {
        return OldGinacConverter<Poly>::getInstance().ginacFactorization(poly);
    }
	
	template<typename Poly>
	bool checkConversion(const Poly& polyA)
    {
        return OldGinacConverter<Poly>::getInstance().checkConversion(polyA);
    }
	
	template<typename Poly>
	void setGinacConverterPolynomialCache( const std::shared_ptr<typename Poly::CACHE>& _cache )
    {
        OldGinacConverter<Poly>::getInstance().setPolynomialCache( _cache );
    }
}

#include "OldGinacConverter.tpp"

#endif
