#include "NumberClI.h"

#include "NumberClRA.h"
#include "NumberMpz.h"



namespace carl {

#ifdef USE_CLN_NUMBERS

	Number<cln::cl_I>::Number(const Number<mpz_class>& n) : Number(cln::cl_I(n.toString().c_str())) {}

//TODO: why not use the standard output of cln here?! Surely that works?
    	std::string Number<cln::cl_I>::toString(bool _infix) const
	{
		std::stringstream s;
		bool negative = (mData < cln::cl_I(0));
		if(negative) s << "(-" << (_infix ? "" : " ");
		s << this->abs().mData;
		if(negative)
		    s << ")";
		return s.str();
	}

	Number<cln::cl_I> Number<cln::cl_I>::pow(std::size_t exp) const {
		Number<cln::cl_RA> ra = Number<cln::cl_RA>(cln::expt(mData, int(exp)));
		return ra.toInt<Number<cln::cl_I>>();

	}

#endif

}
