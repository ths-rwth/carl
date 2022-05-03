#pragma once

#include <carl-common/config.h>

#ifdef USE_CLN_NUMBERS
namespace carl {

    template<>
    inline cln::cl_I convert<cln::cl_I, cln::cl_I>(const cln::cl_I& n) {
        return n;
    };

	template<>
    inline cln::cl_RA convert<cln::cl_I, cln::cl_RA>(const cln::cl_I& n) {
        return n;
    };

	template<>
    inline cln::cl_I convert<cln::cl_RA, cln::cl_I>(const cln::cl_RA& n) {
		assert(cln::denominator(n) == 1);
        return cln::numerator(n);
    };

	template<>
    inline cln::cl_RA convert<cln::cl_RA, cln::cl_RA>(const cln::cl_RA& n) {
		return n;
	}

    template<>
    inline mpz_class convert<cln::cl_I, mpz_class>(const cln::cl_I& n) {
        if( n <= std::numeric_limits<sint>::max() && n >= std::numeric_limits<sint>::min() )
        {
            return mpz_class(static_cast<signed long>(carl::toInt<sint>(n)));
        }

        std::stringstream s;
        s << n;
        mpz_class result = parse<mpz_class>(s.str());
        return result;
    }

    template<>
    inline mpq_class convert<cln::cl_RA, mpq_class>(const cln::cl_RA& n) {
        cln::cl_I den = carl::getDenom(n);
        if( den <= std::numeric_limits<sint>::max() && den >= std::numeric_limits<sint>::min() )
        {
            cln::cl_I num = carl::getNum(n);
            if( num <= std::numeric_limits<sint>::max() && num >= std::numeric_limits<sint>::min() )
            {
                return mpq_class(static_cast<signed long>(carl::toInt<sint>(num)))/mpq_class(static_cast<signed long>(carl::toInt<sint>(den)));
            }
        }
        std::stringstream s;
        s << n;
        mpq_class result = parse<mpq_class>(s.str());
        return result;
    }

    template<>
    inline mpz_class convert<mpz_class, mpz_class>(const mpz_class& n) {
        return n;
    }

	template<>
    inline mpq_class convert<mpq_class, mpq_class>(const mpq_class& n) {
		return n;
	}

    template<>
    inline cln::cl_I convert<mpz_class, cln::cl_I>(const mpz_class& n) {
        if( n <= std::numeric_limits<int>::max() && n >= std::numeric_limits<int>::min() )
        {
            return cln::cl_I(carl::toInt<sint>(n));
        }
        std::stringstream s;
        s << n;
        cln::cl_I result = parse<cln::cl_I>(s.str());
        return result;
    }

    template<>
    inline cln::cl_RA convert<mpq_class, cln::cl_RA>(const mpq_class& n) {
        mpz_class den = carl::getDenom(n);
        if( den <= std::numeric_limits<int>::max() && den >= std::numeric_limits<int>::min() )
        {
            mpz_class num = carl::getNum(n);
            if( num <= std::numeric_limits<int>::max() && num >= std::numeric_limits<int>::min() )
            {
                return cln::cl_RA(carl::toInt<sint>(num))/cln::cl_RA(carl::toInt<sint>(den));
            }
        }
        std::stringstream s;
        s << n;
        cln::cl_RA result = parse<cln::cl_RA>(s.str());
        return result;
    }
	
}
#endif
