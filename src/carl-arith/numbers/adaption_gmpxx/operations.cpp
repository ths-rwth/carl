#include "../numbers.h"
#include <limits>

#include "parser.h"

namespace carl
{

    bool sqrt_exact(const mpq_class& a, mpq_class& b)
    {
        if( mpq_sgn(a.__get_mp()) < 0 ) return false;
        mpz_class den = a.get_den();
        mpz_class num = a.get_num();
        mpz_class root_den;
        mpz_class root_den_rem;
        mpz_sqrtrem(root_den.__get_mp(), root_den_rem.__get_mp(), den.__get_mp());
        if( !carl::is_zero( root_den_rem ) )
            return false;

        mpz_class root_num;
        mpz_class root_num_rem;
        mpz_sqrtrem(root_num.__get_mp(), root_num_rem.__get_mp(), num.__get_mp());
        if( !carl::is_zero( root_num_rem ) )
            return false;

        mpq_class resNum;
        mpq_set_z(resNum.get_mpq_t(), root_num.get_mpz_t());
        mpq_class resDen;
        mpq_set_z(resDen.get_mpq_t(), root_den.get_mpz_t());
        mpq_div(b.get_mpq_t(), resNum.get_mpq_t(), resDen.get_mpq_t());
        return true;
    }

    mpq_class sqrt(const mpq_class& a) {
        auto r = sqrt_safe(a);
        return (r.first + r.second) / 2;
    }

    std::pair<mpq_class,mpq_class> sqrt_safe(const mpq_class& a)
    {
        if (mpq_sgn(a.__get_mp()) == 0) return std::make_pair(mpq_class(0),mpq_class(0));
        assert( mpq_sgn(a.__get_mp()) > 0 );
        mpz_class den = a.get_den();
        mpz_class num = a.get_num();
        mpz_class root_den;
        mpz_class root_den_rem;
        mpz_sqrtrem(root_den.__get_mp(), root_den_rem.__get_mp(), den.__get_mp());

        mpz_class root_num;
        mpz_class root_num_rem;
        mpz_sqrtrem(root_num.__get_mp(), root_num_rem.__get_mp(), num.__get_mp());

        mpq_class lower;
        mpq_class upper;

        lower = root_num;
        if(root_den_rem == 0)
            lower /= root_den;
        else
            lower /= root_den+1;

        if(root_num_rem == 0)
            upper = root_num;
        else
            upper = root_num+1;

        upper /= root_den;

        return std::make_pair(lower,upper);
    }
	
	std::pair<mpq_class,mpq_class> root_safe(const mpq_class& a, uint n)
	{
		assert(mpq_sgn(a.__get_mp()) > 0);
		mpz_class den = a.get_den();
		mpz_class num = a.get_num();
		mpz_class root_den;
		int den_exact = mpz_root(root_den.__get_mp(), den.__get_mp(), n);

		mpz_class root_num;
		int num_exact = mpz_root(root_num.__get_mp(), num.__get_mp(), n);
		
		mpq_class lower;
		mpq_class upper;

		lower = root_num;
		if(den_exact)
			lower /= root_den;
		else
			lower /= root_den+1;

		if(num_exact)
			upper = root_num;
		else
			upper = root_num+1;

		upper /= root_den;
		return std::make_pair(lower,upper);
	}

    std::pair<mpq_class, mpq_class> sqrt_fast(const mpq_class& a)
    {
        assert(a >= 0);
#if 1
        mpz_class num;
        mpz_class num_rem;
        mpz_sqrtrem(num.__get_mp(), num_rem.__get_mp(), a.get_num().__get_mp());
        mpz_class den;
        mpz_class den_rem;
        mpz_sqrtrem(den.__get_mp(), den_rem.__get_mp(), a.get_den().__get_mp());

        if (carl::is_zero(num_rem)) {
            if (carl::is_zero(den_rem)) {
                mpq_class exact_root = mpq_class(num) / den;
                return std::make_pair(exact_root, exact_root);
            } else {
                mpq_class lower = mpq_class(num) / (den + 1);
                mpq_class upper = mpq_class(num) / den;
                return std::make_pair(lower, upper);
            }
        } else {
            if (carl::is_zero(den_rem)) {
                mpq_class lower = mpq_class(num) / den;
                mpq_class upper = (mpq_class(num) + 1) / den;
                return std::make_pair(lower, upper);
            } else {
                mpq_class lower = mpq_class(num) / (den + 1);
                mpq_class upper = (mpq_class(num) + 1) / den;
                return std::make_pair(lower, upper);
            }
        }
#else
        mpq_class exact_root;
        if (carl::sqrtp(a, exact_root)) {
            // root can be computed exactly.
            return std::make_pair(exact_root, exact_root);
        } else {
            // compute an approximation with sqrt(). we can assume that the surrounding integers contain the actual root.
            mpf_class af = sqrt(mpf_class(a));
            mpq_class lower(af - carl::constant_one<mpf_class>::get());
            mpq_class upper(af + carl::constant_one<mpf_class>::get());
            assert(lower * lower < a);
            assert(upper * upper > a);
            return std::make_pair(lower, upper);
        }
#endif
    }

    template<>
    mpz_class parse<mpz_class>(const std::string& n) {
        mpz_class res;
        bool success = parser::parseDecimal(n, res);
        assert(success);
        return res;
    }
	
	template<>
	bool try_parse<mpz_class>(const std::string& n, mpz_class& res) {
		return parser::parseDecimal(n, res);
	}

    template<>
    mpq_class parse<mpq_class>(const std::string& n) {
        mpq_class res;
        bool success = parser::parseRational(n, res);
        assert(success);
        return res;
    }
	
	template<>
	bool try_parse<mpq_class>(const std::string& n, mpq_class& res) {
		return parser::parseRational(n, res);
	}

    std::string toString(const mpq_class& _number, bool _infix)
    {
        std::stringstream s;
        bool negative = (_number < mpq_class(0));
        if(negative) s << "(-" << (_infix ? "" : " ");
        if(_infix) s << carl::abs(_number);
        else
        {
            mpz_class d = carl::get_denom(_number);
            if(d != mpz_class(1)) s << "(/ " << carl::abs(carl::get_num(_number)) << " " << carl::abs(d) << ")";
            else s << carl::abs(_number);
        }
        if(negative)
            s << ")";
        return s.str();
    }

    std::string toString(const mpz_class& _number, bool _infix)
    {
        std::stringstream s;
        bool negative = (_number < mpz_class(0));
        if(negative) s << "(-" << (_infix ? "" : " ");
        s << carl::abs(_number);
        if(negative)
            s << ")";
        return s.str();
    }
}
