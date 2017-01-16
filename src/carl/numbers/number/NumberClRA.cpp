#include "NumberClRA.h"

#include "NumberMpq.h"
#include "NumberMpz.h"

#include <boost/numeric/interval/interval.hpp>
#include <boost/numeric/interval/policies.hpp>
#include <boost/numeric/interval/arith2.hpp>
#include <boost/numeric/interval/checking.hpp>

#include <limits>

namespace carl {

#ifdef USE_CLN_NUMBERS


		//constructs a/b:
		//(this looks hacky.. seems to be the only really functioning way though: take the integers as strings, put the sign at the front and construct
		//cl_RA from the string "[-]a/b")
		Number<cln::cl_RA>::Number(const Number<cln::cl_I>& a,const Number<cln::cl_I>& b) :
			 Number(((a.isNegative() != b.isNegative()) ? "-" : "") + a.abs().toString()+"/"+b.abs().toString()) {}

	
		Number<cln::cl_RA>::Number(const Number<cln::cl_I>& n) { mData = cln::cl_RA(n.getValue()); }
		//Number(const cln::cl_I& n) { mData = cln::cl_RA(n); }


		Number<cln::cl_RA>::Number(const Number<mpq_class>& n) : Number(cln::cl_RA(n.toString().c_str())) {} 
		Number<cln::cl_RA>::Number(const Number<mpz_class>& n) : Number(cln::cl_RA(n.toString().c_str())) {} 







		//this can maybe be done such that it's the same as for mpq_class
    	   Number<cln::cl_RA>::Number(const std::string& s) {	
		//here, we need to distinguish two cases: we want to deal with normal floating point inputs (as "3.5") but also fractions ("7/2")
		if (s.find_first_of("/") == std::string::npos) { //this is the floating point case				
		
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
		} else { //the case where we have a fraction as input
			cln::cl_read_flags flags = {cln::syntax_rational, cln::lsyntax_all, 10, {cln::default_float_format, cln::default_float_format, false}}; 
			std::istringstream istr(s);
			mData = cln::cl_RA(cln::read_rational(istr,flags));
		}
	    } 
	 /*   Number<cln::cl_RA>::Number(double n) {
		switch (std::fpclassify(n)) {
		    case FP_NORMAL: // normalized are fully supported
		        mData = cln::rationalize(convert<mpq_class, cln::cl_RA>(n));
		    case FP_SUBNORMAL: { // subnormals result in underflows, hence the value of the double is 0.f, where f is the significand precision
					static_assert(sizeof(n) == 8, "double is assumed to be eight bytes wide.");
		        sint significandBits = reinterpret_cast<sint>(&n);
		        significandBits = (significandBits << 12) >> 12;
		        if( n < 0 )
		            significandBits = -significandBits;
		        mData = cln::cl_RA( significandBits ) * ONE_DIVIDED_BY_10_TO_THE_POWER_OF_52;
		}
		case FP_ZERO:
		    mData = cln::cl_RA(0);
		case FP_NAN: // NaN and infinite are not supported
		case FP_INFINITE:
		    assert(false);
		    break;
		}
		mData = cln::cl_RA(0);
	}

	Number<cln::cl_RA>::Number(float n) {
		switch (std::fpclassify(n))
		{
		    case FP_NORMAL: // normalized are fully supported
		        mData = cln::rationalize(convert<mpq_class, cln::cl_RA>(n));
		    case FP_SUBNORMAL: { // subnormals result in underflows, hence the value of the double is 0.f, where f is the significand precision
					static_assert(sizeof(n) == 4, "float is assumed to be four bytes wide.");
		        sint significandBits = reinterpret_cast<sint>(&n);
		        significandBits = (significandBits << 9) >> 9;
		        if( n < 0 )
		            significandBits = -significandBits;
		        mData = cln::cl_RA( significandBits ) * ONE_DIVIDED_BY_10_TO_THE_POWER_OF_23;
		}
		case FP_ZERO:
		    mData = cln::cl_RA(0);
		case FP_NAN: // NaN and infinite are not supported
		case FP_INFINITE:
		    assert(false);
		    break;
		}
		mData = cln::cl_RA(0);
	} */
	

	//TODO: why not use the standard output of cln here?! Surely that works?
	    std::string Number<cln::cl_RA>::toString(bool _infix) const
	    {

		std::stringstream s;
		bool negative = (mData < cln::cl_RA(0));
		if(negative) s << "(-" << (_infix ? "" : " ");
		if(_infix) s << this->abs().mData;
		else
		{
		    cln::cl_I d = cln::denominator(mData);
		    if(mData != carl::constant_one<cln::cl_I>().get()) s << "(/ " << cln::abs(cln::numerator(mData)) << " " << cln::abs(d) << ")";
		    else s << this->abs().getValue();
		}
		if(negative)
		    s << ")";
		return s.str();
	    } 





	 bool Number<cln::cl_RA>::sqrt_exact(Number<cln::cl_RA>& b) const
	 {
		if( mData < 0 ) return false;
		cln::cl_RA result;
		bool boolResult = cln::sqrtp( mData, &result );
		b = Number(result);
		return boolResult;
	 }

	
	//this is the same as for mpq_class	
	 Number<cln::cl_RA> Number<cln::cl_RA>::sqrt() const
	 {
		std::pair<Number<cln::cl_RA>, Number<cln::cl_RA>> r = this->sqrt_safe();
		return Number((r.first.getValue() + r.second.getValue()) / 2); //TODO: remove the getValue once operators are implemented!!
	 }
	

	cln::cl_RA Number<cln::cl_RA>::scaleByPowerOfTwo(const cln::cl_RA& a, int exp) const {
		if (exp > 0) {
			return cln::cl_RA(cln::numerator(a) << exp) / cln::denominator(a);
		} else if (exp < 0) {
			return cln::cl_RA(cln::numerator(a)) / (cln::denominator(a) << -exp);
		}
		return a;
	} 

	    std::pair<Number<cln::cl_RA>, Number<cln::cl_RA>> Number<cln::cl_RA>::sqrt_safe() const
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

	    std::pair<Number<cln::cl_RA>, Number<cln::cl_RA>> Number<cln::cl_RA>::sqrt_fast() const
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
