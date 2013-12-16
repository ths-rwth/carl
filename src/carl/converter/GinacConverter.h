/* 
 * File:   GinacConverter.h
 * Author: Florian Corzilius
 *
 * Created on December 13, 2013, 4:25 PM
 */

#ifndef GINACCONVERTER_H
#define	GINACCONVERTER_H

#include "config.h"

#ifdef COMPARE_WITH_GINAC
#include <ginac/ginac.h>
#include "carl/core/MultivariatePolynomial.h"

namespace carl
{   
    MultivariatePolynomial<cln::cl_RA> convert(const GiNaC::ex& _toConvert, const std::map<GiNaC::symbol, carl::Variable>& vars)
    {
        std::cout << __func__ << std::endl;
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
                    carl::MultivariatePolynomial<cln::cl_RA> carlSummand;
                    for(auto factor = summandEx.begin(); factor != summandEx.end(); ++factor)
                    {
                        const GiNaC::ex factorEx = *factor;
                        if(GiNaC::is_exactly_a<GiNaC::symbol>(factorEx))
                        {
                            auto iter = vars.find(GiNaC::ex_to<GiNaC::symbol>(factorEx));
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
                            auto iter = vars.find(GiNaC::ex_to<GiNaC::symbol>(subterm));
                            assert(iter != vars.end());
                            carlSummand *= Term<cln::cl_RA>((cln::cl_RA)1, iter->second, exp);
                        }
                        else assert(false);
                    }
                    result += carlSummand;
                }
                else if(GiNaC::is_exactly_a<GiNaC::symbol>(summandEx))
                {
                    auto iter = vars.find(GiNaC::ex_to<GiNaC::symbol>(summandEx));
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
                    auto iter = vars.find(GiNaC::ex_to<GiNaC::symbol>(subterm));
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
                    auto iter = vars.find(GiNaC::ex_to<GiNaC::symbol>(factorEx));
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
                    auto iter = vars.find(GiNaC::ex_to<GiNaC::symbol>(subterm));
                    assert(iter != vars.end());
                    result *= Term<cln::cl_RA>((cln::cl_RA)1, iter->second, exp);
                }
                else assert( false );
            }
        }
        else if(GiNaC::is_exactly_a<GiNaC::symbol>(ginacPoly))
        {
            auto iter = vars.find(GiNaC::ex_to<GiNaC::symbol>(ginacPoly));
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
            auto iter = vars.find(GiNaC::ex_to<GiNaC::symbol>(subterm));
            assert(iter != vars.end());
            result = MultivariatePolynomial<cln::cl_RA>(Term<cln::cl_RA>((cln::cl_RA)1, iter->second, exp));
        }
        else assert( false );
        return result;
    }
}

#endif

#endif	/* GINACCONVERTER_H */

