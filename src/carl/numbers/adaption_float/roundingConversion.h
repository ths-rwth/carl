/* This file contains the conversion for the rounding modes of the different floating point implementations.
 *
 *	@file roundingConversion.h
 *	@author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de
 *
 *	@since		2013-12-16
 *	@version	2013-12-16
 */

#pragma once

#include "../../util/platform.h"
CLANG_WARNING_DISABLE("-Wsign-conversion")
#ifdef USE_MPFR_FLOAT
#include <mpfr.h>
#endif
CLANG_WARNING_RESET

#include <cassert>

namespace carl {
	
	enum class CARL_RND:int
    {
        N=0,
        Z=1,
        U=2,
        D=3,
        A=4
    };
	
	template<typename NumberType>
	struct convRnd
	{
		CARL_RND operator() ( CARL_RND _rnd )
		{
			return _rnd;
		}
	};
#ifdef USE_MPFR_FLOAT
	// mpfr specialization
	template<>
	struct convRnd<mpfr_ptr>
	{
		inline mpfr_rnd_t operator() ( CARL_RND _rnd )
		{
			switch(_rnd)
			{
				case CARL_RND::A:
					return MPFR_RNDA;
				case CARL_RND::D:
					return MPFR_RNDD;
				case CARL_RND::N:
					return MPFR_RNDN;
				case CARL_RND::U:
					return MPFR_RNDU;
				case CARL_RND::Z:
					return MPFR_RNDZ;
				default:
					// should not happen!
					assert(false);
					//return nullptr;
			}
		}
	};
#endif
}
