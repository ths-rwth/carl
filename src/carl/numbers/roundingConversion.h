/* This file contains the conversion for the rounding modes of the different floating point implementations.
 *
 *	@file roundingConversion.h
 *	@author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de
 *
 *	@since		2013-12-16
 *	@version	2013-12-16
 */

#pragma once


#include <mpfr.h>

#include <assert.h>

namespace carl {
	
	enum class CARL_RND:int
    {
        CARL_RNDN=0,
        CARL_RNDZ=1,
        CARL_RNDU=2,
        CARL_RNDD=3,
        CARL_RNDA=4
    };
	
	template<typename NumberType>
	struct convRnd
	{
		CARL_RND operator() ( CARL_RND _rnd )
		{
			return _rnd;
		}
	};
	
	// mpfr specialization
	template<>
	struct convRnd<mpfr_ptr>
	{
		inline mpfr_rnd_t operator() ( CARL_RND _rnd )
		{
			switch(_rnd)
			{
				case CARL_RND::CARL_RNDA:
					return MPFR_RNDA;
				case CARL_RND::CARL_RNDD:
					return MPFR_RNDD;
				case CARL_RND::CARL_RNDN:
					return MPFR_RNDN;
				case CARL_RND::CARL_RNDU:
					return MPFR_RNDU;
				case CARL_RND::CARL_RNDZ:
					return MPFR_RNDZ;
				default:
					// should not happen!
					assert(false);
					//return nullptr;
			}
		}
	};
}
