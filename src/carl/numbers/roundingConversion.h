/* This file contains the conversion for the rounding modes of the different floating point implementations.
 *
 *	@file roundingConversion.h
 *	@author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de
 *
 *	@since		2013-12-16
 *	@version	2013-12-16
 */

#pragma once

#ifdef USE_MPFR_FLOAT
#include <mpfr.h>
#endif
#include <assert.h>

namespace carl {
	
	enum CARL_RND
    {
        CARL_RNDN=0,
        CARL_RNDZ=1,
        CARL_RNDU=2,
        CARL_RNDD=3,
        CARL_RNDA=4
    };
	
	template<typename RoundingType>
	struct convRnd
	{
		RoundingType operator() ( CARL_RND _rnd )
		{
			return (RoundingType)_rnd;
		}
	};
	
#ifdef USE_MPFR_FLOAT
	// mpfr specialization
	template<>
	struct convRnd<mpfr_t>
	{
		mpfr_rnd_t operator() ( CARL_RND _rnd )
		{
			switch(_rnd)
			{
				case CARL_RNDA:
					return MPFR_RNDA;
				case CARL_RNDD:
					return MPFR_RNDD;
				case CARL_RNDN:
					return MPFR_RNDN;
				case CARL_RNDU:
					return MPFR_RNDU;
				case CARL_RNDZ:
					return MPFR_RNDZ;
				default:
					// should not happen!
					assert(false);
					return NULL;
			}
		}
	};
#endif
}
