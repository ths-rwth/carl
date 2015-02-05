#include "../numbers.h"
#include <limits>

namespace carl
{

    bool sqrtp(const mpq_class& a, mpq_class& b)
    {
        if( mpq_sgn(a.__get_mp()) < 0 ) return false;
        mpz_class den = a.get_den();
        mpz_class num = a.get_num();
        mpz_class root_den;
        mpz_class root_den_rem;
        mpz_sqrtrem(root_den.__get_mp(), root_den_rem.__get_mp(), den.__get_mp());
        if( !carl::isZero( root_den_rem ) )
            return false;

        mpz_class root_num;
        mpz_class root_num_rem;
        mpz_sqrtrem(root_num.__get_mp(), root_num_rem.__get_mp(), num.__get_mp());
        if( !carl::isZero( root_num_rem ) )
            return false;
        
        mpq_class resNum;
        mpq_set_z(resNum.get_mpq_t(), root_num.get_mpz_t());
        mpq_class resDen;
        mpq_set_z(resDen.get_mpq_t(), root_den.get_mpz_t());
        mpq_div(b.get_mpq_t(), resNum.get_mpq_t(), resDen.get_mpq_t());
        return true;
    }
    
    std::pair<mpq_class,mpq_class> sqrt(const mpq_class& a)
    {
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
        long unsigned numerator, denominator;

        numerator = mpz_get_ui(root_num.__get_mp());
        denominator = mpz_get_ui(root_den.__get_mp());

        mpq_set_ui(upper.__get_mp(), mpz_sgn(root_den_rem.__get_mp()) != 0 ? denominator+1 : denominator, numerator);
        mpq_set_ui(lower.__get_mp(), denominator, mpz_sgn(root_num_rem.__get_mp()) != 0 ? numerator+1 : numerator );
        return std::make_pair(lower,upper);
    }
    
    std::pair<mpq_class, mpq_class> sqrt_fast(const mpq_class& a)
    {
        assert(a >= 0);
        mpq_class exact_root;
        if (carl::sqrtp(a, exact_root)) {
            // root can be computed exactly.
            return std::make_pair(exact_root, exact_root);
        } else {
            // compute an approximation with sqrt(). we can assume that the surrounding integers contain the actual root.
            mpz_class lower( floor( sqrt( mpf_class( a ) ) ) );
            mpz_class upper = lower + carl::constant_one<mpz_class>::get();
            return std::make_pair(lower, upper);
        }
    }
    
    template<>
    mpq_class rationalize<mpq_class>(const std::string& inputstring)
    {
        std::vector<std::string> strs;
        boost::split(strs, inputstring, boost::is_any_of("."));

        if(strs.size() > 2)
        {
            throw std::invalid_argument("More than one delimiter in the string.");
        }
        mpq_class result;
        if(!strs.front().empty())
        {
            result += mpq_class(strs.front());
        }
        if(strs.size() > 1)
        {
            //if(strs.back().size() > )
            result += (mpq_class(strs.back())/carl::pow(mpz_class(10),static_cast<unsigned>(strs.back().size())));
        }
        return result;
    }
    
    template<>
    mpq_class rationalize<mpq_class>(const PreventConversion<cln::cl_RA>& n) {
        typedef signed long int IntType;
        cln::cl_I den = carl::getDenom((cln::cl_RA)n);
        if( den <= std::numeric_limits<IntType>::max() && den >= std::numeric_limits<IntType>::min() )
        {
            cln::cl_I num = carl::getNum((cln::cl_RA)n);
            if( num <= std::numeric_limits<IntType>::max() && num >= std::numeric_limits<IntType>::min() )
            {
                return mpq_class(carl::toInt<IntType>(num))/mpq_class(carl::toInt<IntType>(den));
            }
        }
        std::stringstream s;
        s << ((cln::cl_RA)n);
        mpq_class result = rationalize<mpq_class>(s.str());
        return result;
    }

    std::string toString(const mpq_class& _number, bool _infix)
    {
        std::stringstream s;
        bool negative = (_number < mpq_class(0));
        if(negative) s << "(-" << (_infix ? "" : " ");
        if(_infix) s << carl::abs(_number);
        else
        {
            mpz_class d = carl::getDenom(_number);
            if(d != mpz_class(1)) s << "(/ " << carl::abs(carl::getNum(_number)) << " " << carl::abs(d) << ")";
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
