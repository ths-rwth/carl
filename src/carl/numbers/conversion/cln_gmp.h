#pragma once

#include "../config.h"

#ifdef USE_CLN_NUMBERS
namespace carl {
	
	template<>
    inline cln::cl_RA convert<cln::cl_RA, cln::cl_RA>(const cln::cl_RA& n) {
		return n;
	}
	
    template<>
    inline mpq_class convert<cln::cl_RA, mpq_class>(const cln::cl_RA& n) {
        typedef signed long int IntType;
        cln::cl_I den = carl::getDenom(n);
        if( den <= std::numeric_limits<IntType>::max() && den >= std::numeric_limits<IntType>::min() )
        {
            cln::cl_I num = carl::getNum(n);
            if( num <= std::numeric_limits<IntType>::max() && num >= std::numeric_limits<IntType>::min() )
            {
                return mpq_class(carl::toInt<IntType>(num))/mpq_class(carl::toInt<IntType>(den));
            }
        }
        std::stringstream s;
        s << n;
        mpq_class result = rationalize<mpq_class>(s.str());
        return result;
    }
	
	template<>
    inline mpq_class convert<mpq_class, mpq_class>(const mpq_class& n) {
		return n;
	}

    template<>
    inline cln::cl_RA convert<mpq_class, cln::cl_RA>(const mpq_class& n) {
        typedef signed long int IntType;
        mpz_class den = carl::getDenom(n);
        if( den <= std::numeric_limits<IntType>::max() && den >= std::numeric_limits<IntType>::min() )
        {
            mpz_class num = carl::getNum(n);
            if( num <= std::numeric_limits<IntType>::max() && num >= std::numeric_limits<IntType>::min() )
            {
                return cln::cl_RA(carl::toInt<IntType>(num))/cln::cl_RA(carl::toInt<IntType>(den));
            }
        }
        std::stringstream s;
        s << n;
        cln::cl_RA result = rationalize<cln::cl_RA>(s.str());
        return result;
    }
	
}
#endif
