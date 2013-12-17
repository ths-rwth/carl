/**
 * File which adds constructors and destructors for mpfr_ptr.
 * @file mpfr_wrapper.h
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-17
 * @version 2013-12-17
 */


#pragma once
#include <mpfr.h>

namespace carl {
	class carl_mpfr
	{
	public:
		/**
		 * Member
		 */
		mpfr_ptr val;
		
		carl_mpfr()
		{
			mpfr_init(val);
		}
		
		carl_mpfr(double _d)
		{
			mpfr_init(val);
			mpfr_set_d(val, _d);
		}
		
		~carl_mpfr()
		{
			mpfr_clear(val);
		}
	};
}
