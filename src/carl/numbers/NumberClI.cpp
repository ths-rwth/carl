#include "NumberClI.h"


namespace carl {

#ifdef USE_CLN_NUMBERS

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

#endif

}
