#include "NumberMpz.h"

#include "NumberClI.h"

namespace carl {

#ifdef USE_CLN_NUMBERS
	Number<mpz_class>::Number(const Number<cln::cl_I>& n) : Number(n.toString()) {}
#endif

    std::string Number<mpz_class>::toString(bool _infix) const {
		std::stringstream s;
		bool negative = this->isNegative();
		if(negative) s << "(-" << (_infix ? "" : " ");
		s << this->abs().mData;
		if(negative)
		    s << ")";
		return s.str();
    }


}
