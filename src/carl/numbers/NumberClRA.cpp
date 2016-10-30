#include "numbers.h"
#include "NumberClRA.h"

#include <limits>
#include <boost/numeric/interval/interval.hpp>

namespace carl {

#ifdef USE_CLN_NUMBERS


		//this can maybe be done such that it's the same as for mpq_class
    	    Number<cln::cl_RA>::Number(const std::string& s) {	

		std::vector<std::string> strs;
		boost::split(strs, s, boost::is_any_of("."));

		if(strs.size() > 2)
		{
		    throw std::invalid_argument("More than one delimiter in the string.");
		}
		cln::cl_RA result(0);
		if(!strs.front().empty())
		{
		    result += cln::cl_RA(strs.front().c_str());
		}
		if(strs.size() > 1)
		{
		    result += (cln::cl_RA(strs.back().c_str())/carl::pow(cln::cl_RA(10),static_cast<unsigned>(strs.back().size())));
		}
		mData = cln::cl_RA(result);
	    }
	


	    std::string Number<cln::cl_RA>::toString(bool _infix) const
	    {
		std::stringstream s;
		bool negative = (mData < cln::cl_RA(0));
		if(negative) s << "(-" << (_infix ? "" : " ");
		if(_infix) s << this->abs().mData;
		else
		{
		    Number<cln::cl_I> d = this->getDenom();
		    if(!d.isOne()) s << "(/ " << this->getNum().abs().getNumber() << " " << d.abs().getNumber() << ")";
		    else s << this->abs().getNumber();
		}
		if(negative)
		    s << ")";
		return s.str();
	    }





	 bool Number<cln::cl_RA>::sqrt_exact(Number<cln::cl_RA>& b)
	 {
		if( mData < 0 ) return false;
		cln::cl_RA result;
		bool boolResult = cln::sqrtp( mData, &result );
		//TODO: test if this works, otherwise implement and use setValue for Number
		//TODO: return b
		//b = Number(result);
		return boolResult;
	 }

	
	//this is the same as for mpq_class	
	 Number<cln::cl_RA> Number<cln::cl_RA>::sqrt()
	 {
		auto r = this->sqrt_safe();
		return Number((r.first + r.second) / 2);
	 }
	

	cln::cl_RA scaleByPowerOfTwo(const cln::cl_RA& a, int exp) {
		if (exp > 0) {
			return cln::cl_RA(cln::numerator(a) << exp) / cln::denominator(a);
		} else if (exp < 0) {
			return cln::cl_RA(cln::numerator(a)) / (cln::denominator(a) << -exp);
		}
		return a;
	}

	    std::pair<Number<cln::cl_RA>, Number<cln::cl_RA>> Number<cln::cl_RA>::sqrt_safe()
	    {
		assert( mData >= 0 );
		cln::cl_RA exact_root;
		if (cln::sqrtp(mData, &exact_root)) {
		    // root can be computed exactly.
		    return std::make_pair(Number(exact_root), Number(exact_root));
		} else {
				auto factor = int(cln::integer_length(cln::denominator(mData))) - int(cln::integer_length(cln::numerator(mData)));
				if (cln::oddp(factor)) factor += 1;
				cln::cl_RA n = scaleByPowerOfTwo(mData, factor);
				double dn = cln::double_approx(n);
				cln::cl_RA nra = cln::rationalize(dn);
				boost::numeric::interval<double> i;
				if (nra > n) {
					i.assign(cln::double_approx(2*n-nra), dn);
					assert(2*n-nra <= n);
				} else {
					i.assign(dn, cln::double_approx(2*n-nra));
					assert(n <= 2*n-nra);
				}
				i = boost::numeric::sqrt(i);
				i.assign(
					std::nexttoward(i.lower(), -std::numeric_limits<double>::infinity()),
					std::nexttoward(i.upper(), std::numeric_limits<double>::infinity())
				);
				factor = factor / 2;
				cln::cl_RA lower = scaleByPowerOfTwo(cln::rationalize(i.lower()), -factor);
				cln::cl_RA upper = scaleByPowerOfTwo(cln::rationalize(i.upper()), -factor);
				assert(lower*lower <= mData);
				assert(mData <= upper*upper);
				return std::make_pair(Number(lower), Number(upper));
		}
	    }

	    std::pair<Number<cln::cl_RA>, Number<cln::cl_RA>> Number<cln::cl_RA>::sqrt_fast()
	    {
			assert(mData >= 0);
			cln::cl_RA exact_root;
			if (cln::sqrtp(mData, &exact_root)) {
				// root can be computed exactly.
				return std::make_pair(Number(exact_root), Number(exact_root));
			} else {
				// compute an approximation with sqrt(). we can assume that the surrounding integers contain the actual root.
				//auto factor = cln::integer_length(cln::denominator(a)) - cln::integer_length(cln::numerator(a));
				//if (cln::oddp(factor)) factor += 1;
				cln::cl_I lower = cln::floor1(cln::sqrt(toLF(mData)));
			        cln::cl_I upper = lower + 1;
			        assert(cln::expt_pos(lower,2) < mData);
			        assert(cln::expt_pos(upper,2) > mData);
			        return std::make_pair(Number(lower), Number(upper));
			}
	    }



#endif

}

