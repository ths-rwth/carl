
#include "OldGinacConverter.h"

#include "../core/MultivariatePolynomial.h"

#ifdef COMPARE_WITH_GINAC

namespace carl
{
    GiNaC::ex convertToGinac(const MultivariatePolynomial<cln::cl_RA>& poly, const std::map<carl::Variable, GiNaC::ex>& vars)
    {
        GiNaC::ex result = 0;
        for(auto term = poly.begin(); term != poly.end(); ++term)
        {
            GiNaC::ex factor = GiNaC::ex( GiNaC::numeric( term->coeff() ) );
            if((*term).monomial())
            {
                for (auto it: *(term->monomial())) {
                    auto carlToGinacVar = vars.find(it.first);
                    assert(carlToGinacVar != vars.end());
                    factor *= GiNaC::pow(carlToGinacVar->second, it.second);
                }
            }
            result += factor;
        }
        return result;
    }

    MultivariatePolynomial<cln::cl_RA> convertToCarl(const GiNaC::ex& _toConvert, const std::map<GiNaC::ex, carl::Variable, GiNaC::ex_is_less>& vars)
    {
        carl::MultivariatePolynomial<cln::cl_RA> result;
        GiNaC::ex ginacPoly = _toConvert.expand();
        if(GiNaC::is_exactly_a<GiNaC::add>(ginacPoly))
        {
            result = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)0);
            for(auto summand = ginacPoly.begin(); summand != ginacPoly.end(); ++summand)
            {
                const GiNaC::ex summandEx = *summand;
                if(GiNaC::is_exactly_a<GiNaC::mul>(summandEx))
                {
                    carl::MultivariatePolynomial<cln::cl_RA> carlSummand = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)1);
                    for(auto factor = summandEx.begin(); factor != summandEx.end(); ++factor)
                    {
                        const GiNaC::ex factorEx = *factor;
                        if(GiNaC::is_exactly_a<GiNaC::symbol>(factorEx))
                        {
                            auto iter = vars.find(factorEx);
                            assert(iter != vars.end());
                            carlSummand *= iter->second;
                        }
                        else if(GiNaC::is_exactly_a<GiNaC::numeric>(factorEx))
                        {
                            carlSummand *= cln::rationalize(cln::realpart(GiNaC::ex_to<GiNaC::numeric>(factorEx).to_cl_N()));
                        }
                        else if(GiNaC::is_exactly_a<GiNaC::power>(factorEx))
                        {
                            assert(factorEx.nops() == 2);
                            GiNaC::ex exponent = *(++(factorEx.begin()));
                            assert(!exponent.info(GiNaC::info_flags::negative));
                            unsigned exp = static_cast<unsigned>(exponent.integer_content().to_int());
                            GiNaC::ex subterm = *factorEx.begin();
                            assert(GiNaC::is_exactly_a<GiNaC::symbol>(subterm));
                            auto iter = vars.find(subterm);
                            assert(iter != vars.end());
                            carlSummand *= Term<cln::cl_RA>((cln::cl_RA)1, iter->second, exp);
                        }
                        else assert(false);
                    }
                    result += carlSummand;
                }
                else if(GiNaC::is_exactly_a<GiNaC::symbol>(summandEx))
                {
                    auto iter = vars.find(summandEx);
                    assert(iter != vars.end());
                    result += iter->second;
                }
                else if(GiNaC::is_exactly_a<GiNaC::numeric>(summandEx))
                {
                    result += cln::rationalize(cln::realpart(GiNaC::ex_to<GiNaC::numeric>(summandEx).to_cl_N()));
                }
                else if(GiNaC::is_exactly_a<GiNaC::power>(summandEx))
                {
                    assert(summandEx.nops() == 2);
                    GiNaC::ex exponent = *(++(summandEx.begin()));
                    assert(!exponent.info( GiNaC::info_flags::negative));
                    unsigned exp = static_cast<unsigned>(exponent.integer_content().to_int());
                    GiNaC::ex subterm = *summandEx.begin();
                    assert(GiNaC::is_exactly_a<GiNaC::symbol>(subterm));
                    auto iter = vars.find(subterm);
                    assert(iter != vars.end());
                    result += Term<cln::cl_RA>((cln::cl_RA)1, iter->second, exp);
                }
                else assert(false);
            }
        }
        else if(GiNaC::is_exactly_a<GiNaC::mul>(ginacPoly))
        {
            result = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)1);
            for(auto factor = ginacPoly.begin(); factor != ginacPoly.end(); ++factor)
            {
                const GiNaC::ex factorEx = *factor;
                if(GiNaC::is_exactly_a<GiNaC::symbol>(factorEx))
                {
                    auto iter = vars.find(factorEx);
                    assert(iter != vars.end());
                    result *= iter->second;
                }
                else if(GiNaC::is_exactly_a<GiNaC::numeric>(factorEx))
                {
                    result *= cln::rationalize(cln::realpart(GiNaC::ex_to<GiNaC::numeric>(factorEx).to_cl_N()));
                }
                else if(GiNaC::is_exactly_a<GiNaC::power>(factorEx))
                {
                    assert(factorEx.nops() == 2);
                    GiNaC::ex exponent = *(++(factorEx.begin()));
                    assert(!exponent.info(GiNaC::info_flags::negative));
                    unsigned exp = static_cast<unsigned>(exponent.integer_content().to_int());
                    GiNaC::ex subterm = *factorEx.begin();
                    assert(GiNaC::is_exactly_a<GiNaC::symbol>(subterm));
                    auto iter = vars.find(subterm);
                    assert(iter != vars.end());
                    result *= Term<cln::cl_RA>((cln::cl_RA)1, iter->second, exp);
                }
                else assert( false );
            }
        }
        else if(GiNaC::is_exactly_a<GiNaC::symbol>(ginacPoly))
        {
            auto iter = vars.find(ginacPoly);
            assert(iter != vars.end());
            result = MultivariatePolynomial<cln::cl_RA>(iter->second);
        }
        else if(GiNaC::is_exactly_a<GiNaC::numeric>(ginacPoly))
        {
            result = MultivariatePolynomial<cln::cl_RA>(cln::rationalize(cln::realpart( GiNaC::ex_to<GiNaC::numeric>(ginacPoly).to_cl_N())));
        }
        else if(GiNaC::is_exactly_a<GiNaC::power>(ginacPoly))
        {
            assert(ginacPoly.nops() == 2);
            GiNaC::ex exponent = *(++ginacPoly.begin());
            assert(!exponent.info(GiNaC::info_flags::negative));
            unsigned exp = static_cast<unsigned>(exponent.integer_content().to_int());
            GiNaC::ex subterm = *ginacPoly.begin();
            assert(GiNaC::is_exactly_a<GiNaC::symbol>(subterm));
            auto iter = vars.find(subterm);
            assert(iter != vars.end());
            result = MultivariatePolynomial<cln::cl_RA>(Term<cln::cl_RA>((cln::cl_RA)1, iter->second, exp));
        }
        else assert( false );
        return result;
    }

    MultivariatePolynomial<cln::cl_RA> ginacGcd(const MultivariatePolynomial<cln::cl_RA>& polyA, const MultivariatePolynomial<cln::cl_RA>& polyB)
    {
        MultivariatePolynomial<cln::cl_RA> result;
        std::map<Variable, GiNaC::ex> carlToGinacVarMap;
        std::map<GiNaC::ex, Variable, GiNaC::ex_is_less> ginacToCarlVarMap;
        gatherVariables(polyA, carlToGinacVarMap, ginacToCarlVarMap);
        gatherVariables(polyB, carlToGinacVarMap, ginacToCarlVarMap);
        GiNaC::ex ginacResult = GiNaC::gcd(convertToGinac(polyA, carlToGinacVarMap), convertToGinac(polyB, carlToGinacVarMap));
        result = convertToCarl(ginacResult, ginacToCarlVarMap);
        if( !result.isZero() && result.lcoeff() < carl::constant_zero<cln::cl_RA>().get() )
            return -result;
        return result;
    }
    
    bool checkConversion(const MultivariatePolynomial<cln::cl_RA>& polyA)
    {
        std::map<Variable, GiNaC::ex> carlToGinacVarMap;
        std::map<GiNaC::ex, Variable, GiNaC::ex_is_less> ginacToCarlVarMap;
        gatherVariables(polyA, carlToGinacVarMap, ginacToCarlVarMap);
        MultivariatePolynomial<cln::cl_RA> result = convertToCarl(convertToGinac(polyA, carlToGinacVarMap), ginacToCarlVarMap);
        return polyA == result;  
    }

    bool ginacDivide(const MultivariatePolynomial<cln::cl_RA>& polyA, const MultivariatePolynomial<cln::cl_RA>& polyB, MultivariatePolynomial<cln::cl_RA>& result)
    {
        std::map<Variable, GiNaC::ex> carlToGinacVarMap;
        std::map<GiNaC::ex, Variable, GiNaC::ex_is_less> ginacToCarlVarMap;
        gatherVariables(polyA, carlToGinacVarMap, ginacToCarlVarMap);
        gatherVariables(polyB, carlToGinacVarMap, ginacToCarlVarMap);
        GiNaC::ex ginacResult;
        bool divided = GiNaC::divide(convertToGinac(polyA, carlToGinacVarMap), convertToGinac(polyB, carlToGinacVarMap), ginacResult);
        if(divided)
            result = convertToCarl(ginacResult, ginacToCarlVarMap);
        return divided;
    }

    std::unordered_map<const MultivariatePolynomial<cln::cl_RA>, unsigned, std::hash<MultivariatePolynomial<cln::cl_RA>>> ginacFactorization(const MultivariatePolynomial<cln::cl_RA>& poly)
    {
        std::unordered_map<const MultivariatePolynomial<cln::cl_RA>, unsigned, std::hash<MultivariatePolynomial<cln::cl_RA>>> result;
        std::map<Variable, GiNaC::ex> carlToGinacVarMap;
        std::map<GiNaC::ex, Variable, GiNaC::ex_is_less> ginacToCarlVarMap;
        gatherVariables(poly, carlToGinacVarMap, ginacToCarlVarMap);
        GiNaC::ex ginacResult = GiNaC::factor(convertToGinac(poly, carlToGinacVarMap));
        if(GiNaC::is_exactly_a<GiNaC::mul>(ginacResult))
        {
            for(auto factor = ginacResult.begin(); factor != ginacResult.end(); ++factor)
            {
                const GiNaC::ex& factorEx = *factor;
                if(GiNaC::is_exactly_a<GiNaC::power>(factorEx))
                {
                    assert(factorEx.nops() == 2);
                    GiNaC::ex exponent = *(++factorEx.begin());
                    assert(!exponent.info(GiNaC::info_flags::negative));
                    unsigned exp = static_cast<unsigned>(exponent.integer_content().to_int());
                    GiNaC::ex subterm = *factorEx.begin();
                    MultivariatePolynomial<cln::cl_RA> carlFactor = convertToCarl(subterm, ginacToCarlVarMap);
                    assert(result.find(carlFactor) == result.end());
                    result.insert(std::pair<MultivariatePolynomial<cln::cl_RA>, unsigned>(carlFactor, exp));
                }
                else
                {
                    MultivariatePolynomial<cln::cl_RA> carlFactor = convertToCarl(factorEx, ginacToCarlVarMap);
                    assert(result.find(carlFactor) == result.end());
                    result.insert(std::pair<MultivariatePolynomial<cln::cl_RA>, unsigned>(carlFactor, 1));
                }
            }
        }
        else if(GiNaC::is_exactly_a<GiNaC::power>(ginacResult))
        {
            assert(ginacResult.nops() == 2);
            GiNaC::ex exponent = *(++ginacResult.begin());
            assert(!exponent.info(GiNaC::info_flags::negative));
            unsigned exp = static_cast<unsigned>(exponent.integer_content().to_int());
            GiNaC::ex subterm = *ginacResult.begin();
            result.insert(std::pair<MultivariatePolynomial<cln::cl_RA>, unsigned>(convertToCarl(subterm, ginacToCarlVarMap), exp));
        }
        else
        {
            result.insert(std::pair<MultivariatePolynomial<cln::cl_RA>, unsigned>(convertToCarl(ginacResult, ginacToCarlVarMap), 1));
        }
        return result;
    }

	bool similar(GiNaC::ex a, GiNaC::ex b) {
		if (b == 0) return a == 0;
		GiNaC::ex x = a, y = b;
		while ((!GiNaC::is_exactly_a<GiNaC::numeric>(b)) && GiNaC::divide(x, b, x));
		while ((!GiNaC::is_exactly_a<GiNaC::numeric>(a)) && GiNaC::divide(y, a, y));
		while ((x != 1) && (y != 1)) {
			if (x == y) return true;
			if (x == -y) return true;
			if (GiNaC::is_exactly_a<GiNaC::numeric>(x) && GiNaC::is_exactly_a<GiNaC::numeric>(y)) return true;
			GiNaC::ex g = GiNaC::gcd(x, y);
			if (g != 1) {
				assert(GiNaC::divide(x, g, x));
				assert(GiNaC::divide(y, g, y));
				continue;
			} else {
				GiNaC::ex gx = GiNaC::gcd(x, b);
				GiNaC::ex gy = GiNaC::gcd(y, a);
				if ((gx == 1) && (gy == 1)) return false;
				assert(GiNaC::divide(x, gx, x));
				assert(GiNaC::divide(y, gy, y));
			}
		}
		return true;
	}
} // end namespace carl

#endif
