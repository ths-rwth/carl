 
#include "NumberMpz.h"


namespace carl {



    std::string Number<mpz_class>::toString(bool _infix) const
    {
	std::stringstream s;
	bool negative = this->isNegative();
	if(negative) s << "(-" << (_infix ? "" : " ");
	s << this->abs().mData;
	if(negative)
	    s << ")";
	return s.str();
    }


}
