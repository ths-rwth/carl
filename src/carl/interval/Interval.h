/* The class which contains the interval arithmetic including trigonometric
 * functions. The template parameter contains the number type used for the
 * boundaries. It is necessary to implement the rounding and checking policies
 * for any non-primitive type such that the desired inclusion property can be
 * maintained.
 *
 * Requirements for the NumberType:
 * - Operators +,-,*,/ with the expected functionality
 * - Operators +=,-=,*=,/= with the expected functionality
 * - Operators <,>,<=,>=,==,!= with the expected functionality
 * - Operations abs, min, max, log, exp, power, sqrt
 * - Trigonometric functions sin, cos, tan, asin, acos, atan, sinh, cosh, tanh,
 *				asinh, acosh, atanh (these functions are needed for the
 *				specialization of the rounding modes.
 * - Operator <<
 *
 * @file   Interval.h
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-13
 * @version 2014-11-11
 */

#pragma once

#include "../core/Sign.h"
#include "../core/Variable.h"
#include "../core/Sign.h"
#include "../numbers/numbers.h"
#include "../util/platform.h"
#include "../util/SFINAE.h"
#include "BoundType.h"
#include "checking.h"
#include "rounding.h"

CLANG_WARNING_DISABLE("-Wunused-parameter")
CLANG_WARNING_DISABLE("-Wunused-local-typedef")
#include <boost/functional/hash.hpp>
#include <boost/numeric/interval.hpp>
#include <boost/numeric/interval/interval.hpp>
CLANG_WARNING_RESET

#include <cassert>
#include <cmath>
#include <limits>
#include <list>
#include <map>
#include <sstream>


namespace carl
{
    template<typename Number>
    class Interval;

   /**
    * States that `boost::variant` is indeed a `boost::variant`.
    */
   template <class Number> struct is_interval<carl::Interval<Number>> : std::true_type {};
   /**
    * States that `const boost::variant` is indeed a `boost::variant`.
    */
   template <class Number> struct is_interval<const carl::Interval<Number>> : std::true_type {};

   /**
	* Check if this interval is a point-interval containing 0.
	*/
   template<typename Number>
   static bool isZero(const Interval<Number>& _in)
   {
	   return _in.isZero();
   }

   /**
	* Check if this interval is a point-interval containing 1.
	*/
   template<typename Number>
   static bool isOne(const Interval<Number>& _in)
   {
	   return _in.isOne();
   }


    /**
     * Struct which holds the rounding and checking policies required for boost
     * interval.
     */
    template<typename Number, typename Interval>
    struct policies
    {
        using roundingP = carl::rounding<Number>;
        using checkingP = carl::checking<Number>;
		static void sanitize(Interval&) {}
    };

    /**
     * Template specialization for rounding and checking policies for native double.
     */
    // TODO: Create struct specialization for all types which are already covered by the standard boost interval policies.
    template<typename Interval>
    struct policies<double, Interval>
    {
        using roundingP = boost::numeric::interval_lib::save_state<boost::numeric::interval_lib::rounded_transc_std<double> >; // TODO: change it to boost::numeric::interval_lib::rounded_transc_opp, if new boost release patches the bug with clang
        using checkingP = boost::numeric::interval_lib::checking_no_nan<double, boost::numeric::interval_lib::checking_no_nan<double> >;
		static void sanitize(Interval& n) {
			if (std::isinf(n.lower())) {
				n.setLowerBoundType(BoundType::INFTY);
			}
			if (std::isinf(n.upper())) {
				n.setUpperBoundType(BoundType::INFTY);
			}
			assert(!std::isnan(n.lower()));
			assert(!std::isnan(n.upper()));
		}
    };

    /**
     *The class which contains the interval arithmetic including trigonometric
     * functions. The template parameter contains the number type used for the
     * boundaries. It is necessary to implement the rounding and checking policies
     * for any non-primitive type such that the desired inclusion property can be
     * maintained.
     *
     * Requirements for the NumberType:
     * - Operators +,-,*,/ with the expected functionality
     * - Operators +=,-=,*=,/= with the expected functionality
     * - Operators <,>,<=,>=,==,!= with the expected functionality
     * - Operations abs, min, max, log, exp, power, sqrt
     * - Trigonometric functions sin, cos, tan, asin, acos, atan, sinh, cosh, tanh,
     *				asinh, acosh, atanh (these functions are needed for the
     *				specialization of the rounding modes.
     * - Operator <<
     */
    template<typename Number>
    class Interval : public policies<Number, Interval<Number>>
    {
    public:
		using Policy = policies<Number, Interval<Number>>;
        /*
         * Typedefs
         */
        //typedef typename policies<Number>::checking checking;
        //typedef typename policies<Number>::rounding rounding;
        using BoostIntervalPolicies = boost::numeric::interval_lib::policies< typename Policy::roundingP, typename Policy::checkingP >;
        using BoostInterval = boost::numeric::interval< Number, BoostIntervalPolicies >;
        using evalintervalmap = std::map<Variable, Interval<Number> >;

        /// Macro to perform a quick check on the passed interval bounds.
#define BOUNDS_OK( lower, lowerBoundType, upper, upperBoundType )\
(lowerBoundType == BoundType::INFTY || upperBoundType == BoundType::INFTY || lower <= upper)

        /// Macro to perform a quick check for emptiness of the interval.
#define IS_EMPTY(lower, lowerBoundType, upper, upperBoundType )\
(((lowerBoundType == BoundType::STRICT && upperBoundType != BoundType::INFTY) || (lowerBoundType != BoundType::INFTY && upperBoundType == BoundType::STRICT)) && lower == upper)

        /// Macro to perform a quick check if the interval is unbounded.
#define IS_UNBOUNDED(lower, lowerBoundType, upper, upperBoundType )\
(lowerBoundType == BoundType::INFTY && upperBoundType == BoundType::INFTY)

    protected:
        /***********************************************************************
         * Members
         **********************************************************************/

        BoostInterval mContent;
        BoundType mLowerBoundType;
        BoundType mUpperBoundType;

    public:

        /***********************************************************************
         * Constructors & Destructor
         **********************************************************************/

        /**
         * Default constructor which constructs the empty interval at point 0.
         */
        Interval() :
	        mContent(carl::constant_zero<Number>().get()),
	        mLowerBoundType(BoundType::STRICT),
	        mUpperBoundType(BoundType::STRICT)
		{ }

        /**
         * Constructor which constructs the pointinterval at n.
         * @param n Location of the pointinterval.
         */
        Interval(const Number& n) :
	        mContent(n),
	        mLowerBoundType(BoundType::WEAK),
	        mUpperBoundType(BoundType::WEAK)
		{
			Policy::sanitize(*this);
		}

        /**
         * Constructor which constructs the weak-bounded interval between lower
         * and upper. If the bounds are invalid an empty interval at point 0 is
         * constructed.
         * @param lower The desired lower bound.
         * @param upper The desired upper bound.
         */
        Interval(const Number& lower, const Number& upper):
			mContent(), mLowerBoundType(), mUpperBoundType()
        {
            if (BOUNDS_OK(lower, BoundType::WEAK, upper, BoundType::WEAK))
            {
                mContent = BoostInterval(lower, upper);
                mLowerBoundType = BoundType::WEAK;
                mUpperBoundType = BoundType::WEAK;
            }
            else
            {
                mContent = BoostInterval(carl::constant_zero<Number>().get());
                mLowerBoundType = BoundType::STRICT;
                mUpperBoundType = BoundType::STRICT;
            }
			Policy::sanitize(*this);
        }

        /**
         * Constructor which constructs the interval according to the passed boost
         * interval with the passed bound types. Note that if the interval is a
         * pointinterval with both strict bounds or the content is invalid the
         * empty interval is constructed and if both bounds are infty the unbounded
         * interval is constructed.
         * @param content The passed boost interval.
         * @param lowerBoundType The desired lower bound type, defaults to WEAK.
         * @param upperBoundType The desired upper bound type, defaults to WEAK.
         */
        explicit Interval(const BoostInterval& content, BoundType lowerBoundType = BoundType::WEAK, BoundType upperBoundType = BoundType::WEAK):
			mContent(), mLowerBoundType(), mUpperBoundType()
        {
            if (BOUNDS_OK(content.lower(), lowerBoundType, content.upper(), upperBoundType))
            {
                if (IS_EMPTY(content.lower(), lowerBoundType, content.upper(), upperBoundType) )
                {
                    mContent = BoostInterval(carl::constant_zero<Number>().get());
                    mLowerBoundType = BoundType::STRICT;
                    mUpperBoundType = BoundType::STRICT;
                }
                if (IS_UNBOUNDED(content.lower(), lowerBoundType, content.upper(), upperBoundType))
                {
                    mContent = BoostInterval(carl::constant_zero<Number>().get());
                    mLowerBoundType = BoundType::INFTY;
                    mUpperBoundType = BoundType::INFTY;
                }
                else if (lowerBoundType == BoundType::INFTY || upperBoundType == BoundType::INFTY)
                {
                    mContent = BoostInterval(lowerBoundType == BoundType::INFTY ? content.upper() : content.lower());
                    mLowerBoundType = lowerBoundType;
                    mUpperBoundType = upperBoundType;
                }
                else
                {
                    mContent = content;
                    mLowerBoundType = lowerBoundType;
                    mUpperBoundType = upperBoundType;
                }
            }
            else
            {
                mContent = BoostInterval(carl::constant_zero<Number>().get());
                mLowerBoundType = BoundType::STRICT;
                mUpperBoundType = BoundType::STRICT;
            }
			Policy::sanitize(*this);
			assert(isConsistent());
        }

        /**
         * Constructor which constructs the interval according to the passed bounds
         * with the passed bound types. Note that if the interval is a
         * pointinterval with both strict bounds or the content is invalid the
         * empty interval is constru
         * @param lower The desired lower bound.
         * @param lowerBoundType The desired lower bound type.
         * @param upper The desired upper bound.
         * @param upperBoundType The desired upper bound type.
         */
        Interval(const Number& lower, BoundType lowerBoundType, const Number& upper, BoundType upperBoundType):
			mContent(), mLowerBoundType(), mUpperBoundType()
        {
            if (BOUNDS_OK(lower, lowerBoundType, upper, upperBoundType))
            {
                if (IS_EMPTY(lower, lowerBoundType, upper, upperBoundType))
                {
                    mContent = BoostInterval(carl::constant_zero<Number>().get());
                    mLowerBoundType = BoundType::STRICT;
                    mUpperBoundType = BoundType::STRICT;
                }
                else if (IS_UNBOUNDED(lower, lowerBoundType, upper, upperBoundType))
                {
                    mContent = BoostInterval(carl::constant_zero<Number>().get());
                    mLowerBoundType = BoundType::INFTY;
                    mUpperBoundType = BoundType::INFTY;
                }
                else if (lowerBoundType == BoundType::INFTY || upperBoundType == BoundType::INFTY)
                {
                    mContent = BoostInterval(lowerBoundType == BoundType::INFTY ? upper : lower);
                    mLowerBoundType = lowerBoundType;
                    mUpperBoundType = upperBoundType;
                }
                else
                {
                    mContent = BoostInterval(lower, upper);
                    mLowerBoundType = lowerBoundType;
                    mUpperBoundType = upperBoundType;
                }
            }
            else
            {
                mContent = BoostInterval(carl::constant_zero<Number>().get());
                mLowerBoundType = BoundType::STRICT;
                mUpperBoundType = BoundType::STRICT;
            }
			Policy::sanitize(*this);
        }

        /**
         * Copy constructor.
         * @param o The original interval.
         */
        Interval(const Interval<Number>& o) :
        mContent(BoostInterval(o.mContent)),
        mLowerBoundType(o.mLowerBoundType),
        mUpperBoundType(o.mUpperBoundType) { }

        /**
         * Constructor which constructs a pointinterval from a passed double.
         * @param n The passed double.
         */
        template<typename N = Number, DisableIf<std::is_same<N, double >> = dummy, DisableIf<is_rational<N >> = dummy >
        explicit Interval(const double& n) :
        mContent(carl::Interval<Number>::BoostInterval(n, n)),
        mLowerBoundType(BoundType::WEAK),
        mUpperBoundType(BoundType::WEAK) { 
			Policy::sanitize(*this);
		}

        /**
         * Constructor which constructs an interval from the passed double bounds.
         * @param lower The desired lower bound.
         * @param upper The desired upper bound.
         */
        template<typename N = Number, DisableIf<std::is_same<N, double >> = dummy, DisableIf<is_rational<N >> = dummy >
        Interval(double lower, double upper)
        {
            if (BOUNDS_OK(lower, BoundType::WEAK, upper, BoundType::WEAK))
            {
                mContent = BoostInterval(lower, upper);
                mLowerBoundType = BoundType::WEAK;
                mUpperBoundType = BoundType::WEAK;
            }
            else
            {
                mContent = BoostInterval(carl::constant_zero<Number>().get());
                mLowerBoundType = BoundType::STRICT;
                mUpperBoundType = BoundType::STRICT;
            }
			Policy::sanitize(*this);
        }

        /**
         * Constructor which constructs the interval according to the passed double
         * bounds with the passed bound types. Note that if the interval is a
         * pointinterval with both strict bounds or the content is invalid the
         * empty interval is constru
         * @param lower The desired double lower bound.
         * @param lowerBoundType The desired lower bound type.
         * @param upper The desired double upper bound.
         * @param upperBoundType The desired upper bound type.
         */
        template<typename N = Number, DisableIf<std::is_same<N, double >> = dummy, DisableIf<is_rational<N >> = dummy>
        Interval(double lower, BoundType lowerBoundType, double upper, BoundType upperBoundType)
        {
            if (BOUNDS_OK(lower, lowerBoundType, upper, upperBoundType))
            {
                if (IS_EMPTY(lower, lowerBoundType, upper, upperBoundType))
                {
                    mContent = BoostInterval(carl::constant_zero<Number>().get());
                    mLowerBoundType = BoundType::STRICT;
                    mUpperBoundType = BoundType::STRICT;
                }
                else if (IS_UNBOUNDED(lower, lowerBoundType, upper, upperBoundType))
                {
                    mContent = BoostInterval(carl::constant_zero<Number>().get());
                    mLowerBoundType = BoundType::INFTY;
                    mUpperBoundType = BoundType::INFTY;
                }
                else if (lowerBoundType == BoundType::INFTY || upperBoundType == BoundType::INFTY)
                {
                    mContent = BoostInterval(lowerBoundType == BoundType::INFTY ? upper : lower);
                    mLowerBoundType = lowerBoundType;
                    mUpperBoundType = upperBoundType;
                }
                else
                {
                    mContent = BoostInterval(lower, upper);
                    mLowerBoundType = lowerBoundType;
                    mUpperBoundType = upperBoundType;
                }
            }
            else
            {
                mContent = BoostInterval(carl::constant_zero<Number>().get());
                mLowerBoundType = BoundType::STRICT;
                mUpperBoundType = BoundType::STRICT;
            }
			Policy::sanitize(*this);
        }

        /**
         * Constructor which constructs a pointinterval from a passed int.
         * @param n The passed double.
         */
        template<typename N = Number, DisableIf<std::is_same<N, int >> = dummy >
        explicit Interval(const int& n) :
        mContent(carl::Interval<Number>::BoostInterval(n, n)),
        mLowerBoundType(BoundType::WEAK),
        mUpperBoundType(BoundType::WEAK) { }

        /**
         * Constructor which constructs an interval from the passed int bounds.
         * @param lower The desired lower bound.
         * @param upper The desired upper bound.
         */
        template<typename N = Number, DisableIf<std::is_same<N, int >> = dummy>
        Interval(int lower, int upper)
        {
            if (BOUNDS_OK(lower, BoundType::WEAK, upper, BoundType::WEAK))
            {
                mContent = BoostInterval(lower, upper);
                mLowerBoundType = BoundType::WEAK;
                mUpperBoundType = BoundType::WEAK;
            }
            else
            {
                mContent = BoostInterval(carl::constant_zero<Number>().get());
                mLowerBoundType = BoundType::STRICT;
                mUpperBoundType = BoundType::STRICT;
            }
        }

        /**
         * Constructor which constructs the interval according to the passed int
         * bounds with the passed bound types. Note that if the interval is a
         * pointinterval with both strict bounds or the content is invalid the
         * empty interval is constru
         * @param lower The desired lower bound.
         * @param lowerBoundType The desired lower bound type.
         * @param upper The desired upper bound.
         * @param upperBoundType The desired upper bound type.
         */
        template<typename N = Number, DisableIf<std::is_same<N, int >> = dummy>
        Interval(int lower, BoundType lowerBoundType, int upper, BoundType upperBoundType)
        {
            if (BOUNDS_OK(lower, lowerBoundType, upper, upperBoundType))
            {
                if (IS_EMPTY(lower, lowerBoundType, upper, upperBoundType))
                {
                    mContent = BoostInterval(carl::constant_zero<Number>().get());
                    mLowerBoundType = BoundType::STRICT;
                    mUpperBoundType = BoundType::STRICT;
                }
                else if (IS_UNBOUNDED(lower, lowerBoundType, upper, upperBoundType))
                {
                    mContent = BoostInterval(carl::constant_zero<Number>().get());
                    mLowerBoundType = BoundType::INFTY;
                    mUpperBoundType = BoundType::INFTY;
                }
                else if (lowerBoundType == BoundType::INFTY || upperBoundType == BoundType::INFTY)
                {
                    mContent = BoostInterval(lowerBoundType == BoundType::INFTY ? upper : lower);
                    mLowerBoundType = lowerBoundType;
                    mUpperBoundType = upperBoundType;
                }
                else
                {
                    mContent = BoostInterval(lower, upper);
                    mLowerBoundType = lowerBoundType;
                    mUpperBoundType = upperBoundType;
                }
            }
            else
            {
                mContent = BoostInterval(carl::constant_zero<Number>().get());
                mLowerBoundType = BoundType::STRICT;
                mUpperBoundType = BoundType::STRICT;
            }
        }

        /**
         * Constructor which constructs a pointinterval from a passed unsigned int.
         * @param n The passed double.
         */
        template<typename N = Number, DisableIf<std::is_same<N, unsigned int >> = dummy >
        explicit Interval(const unsigned int& n) :
        mContent(carl::Interval<Number>::BoostInterval(n, n)),
        mLowerBoundType(BoundType::WEAK),
        mUpperBoundType(BoundType::WEAK) { }

        /**
         * Constructor which constructs an interval from the passed unsigned int bounds.
         * @param lower The desired lower bound.
         * @param upper The desired upper bound.
         */
        template<typename N = Number, DisableIf<std::is_same<N, unsigned int >> = dummy>
        explicit Interval(unsigned int lower, unsigned int upper)
        {
            if (BOUNDS_OK(lower, BoundType::WEAK, upper, BoundType::WEAK))
            {
                mContent = BoostInterval(lower, upper);
                mLowerBoundType = BoundType::WEAK;
                mUpperBoundType = BoundType::WEAK;
            }
            else
            {
                mContent = BoostInterval(carl::constant_zero<Number>().get());
                mLowerBoundType = BoundType::STRICT;
                mUpperBoundType = BoundType::STRICT;
            }
        }

        /**
         * Constructor which constructs the interval according to the passed unsigned int
         * bounds with the passed bound types. Note that if the interval is a
         * pointinterval with both strict bounds or the content is invalid the
         * empty interval is constru
         * @param lower The desired lower bound.
         * @param lowerBoundType The desired lower bound type.
         * @param upper The desired upper bound.
         * @param upperBoundType The desired upper bound type.
         */
        template<typename N = Number, DisableIf<std::is_same<N, unsigned int >> = dummy>
        explicit Interval(unsigned int lower, BoundType lowerBoundType, unsigned int upper, BoundType upperBoundType)
        {
            if (BOUNDS_OK(lower, lowerBoundType, upper, upperBoundType))
            {
                if (IS_EMPTY(lower, lowerBoundType, upper, upperBoundType))
                {
                    mContent = BoostInterval(carl::constant_zero<Number>().get());
                    mLowerBoundType = BoundType::STRICT;
                    mUpperBoundType = BoundType::STRICT;
                }
                else if (IS_UNBOUNDED(lower, lowerBoundType, upper, upperBoundType))
                {
                    mContent = BoostInterval(carl::constant_zero<Number>().get());
                    mLowerBoundType = BoundType::INFTY;
                    mUpperBoundType = BoundType::INFTY;
                }
                else if (lowerBoundType == BoundType::INFTY || upperBoundType == BoundType::INFTY)
                {
                    mContent = BoostInterval(lowerBoundType == BoundType::INFTY ? upper : lower);
                    mLowerBoundType = lowerBoundType;
                    mUpperBoundType = upperBoundType;
                }
                else
                {
                    mContent = BoostInterval(lower, upper);
                    mLowerBoundType = lowerBoundType;
                    mUpperBoundType = upperBoundType;
                }
            }
            else
            {
                mContent = BoostInterval(carl::constant_zero<Number>().get());
                mLowerBoundType = BoundType::STRICT;
                mUpperBoundType = BoundType::STRICT;
            }
        }

        /**
         * Constructor which constructs a pointinterval from a passed general
         * rational number.
         * @param n The passed double.
         */
        template<typename Num = Number, typename Rational, EnableIf<std::is_floating_point<Num >> = dummy, DisableIf<std::is_same<Num, Rational >> = dummy>
        explicit Interval(Rational n):
			mContent(), mLowerBoundType(), mUpperBoundType()
        {
            *this = Interval<double>(n, n);
			Policy::sanitize(*this);
        }

        /**
         * Constructor which constructs an interval from the passed general rational
         * bounds.
         * @param lower The desired lower bound.
         * @param upper The desired upper bound.
         */
        template<typename Num = Number, typename Rational, EnableIf<std::is_floating_point<Num >> = dummy, DisableIf<std::is_same<Num, Rational >> = dummy>
        Interval(Rational lower, Rational upper):
			mContent(), mLowerBoundType(), mUpperBoundType()
        {
            *this = Interval<double>(lower, BoundType::WEAK, upper, BoundType::WEAK);
			Policy::sanitize(*this);
        }

        /**
         * Constructor which constructs the interval according to the passed general
         * rational bounds with the passed bound types. Note that if the interval is a
         * pointinterval with both strict bounds or the content is invalid the
         * empty interval is constru
         * @param lower The desired lower bound.
         * @param lowerBoundType The desired lower bound type.
         * @param upper The desired upper bound.
         * @param upperBoundType The desired upper bound type.
         */
        template<typename Num = Number, typename Rational, EnableIf<std::is_floating_point<Num >> = dummy, DisableIf<std::is_same<Num, Rational >> = dummy>
        Interval(Rational lower, BoundType lowerBoundType, Rational upper, BoundType upperBoundType)
        {
            mLowerBoundType = lowerBoundType;
            mUpperBoundType = upperBoundType;
            double dLeft = carl::roundDown(lower, false);
            double dRight = carl::roundUp(upper, false);
            if (dLeft == -std::numeric_limits<double>::infinity()) mLowerBoundType = BoundType::INFTY;
            if (dRight == std::numeric_limits<double>::infinity()) mUpperBoundType = BoundType::INFTY;
            if (mLowerBoundType == BoundType::INFTY && mUpperBoundType == BoundType::INFTY)
            {
                mContent = BoostInterval(carl::constant_zero<Num>().get());
            }
            else if (mLowerBoundType == BoundType::INFTY)
            {
                mContent = BoostInterval(Num(dRight));
            }
            else if (mUpperBoundType == BoundType::INFTY)
            {
                mContent = BoostInterval(Num(dLeft));
            }
            else if ((lower == upper && lowerBoundType != upperBoundType) || lower > upper)
            {
                mLowerBoundType = BoundType::STRICT;
                mUpperBoundType = BoundType::STRICT;
                mContent = BoostInterval(carl::constant_zero<Num>().get());
            }
            else
            {
                mContent = BoostInterval(Num(dLeft), Num(dRight));
            }
			Policy::sanitize(*this);
        }

        /**
         * Constructor which constructs a pointinterval from a passed general
         * float number (e.g. FLOAT_T).
         * @param n The passed double.
         */
        template<typename Num = Number, typename Float, EnableIf<is_rational<Num >> = dummy, EnableIf<std::is_floating_point<Float >> = dummy, DisableIf<std::is_same<Num, Float >> = dummy>
        explicit Interval(Float n):
			mContent(), mLowerBoundType(), mUpperBoundType()
        {
            *this = Interval<double>(n, n);
			Policy::sanitize(*this);
        }

        /**
         * Constructor which constructs an interval from the passed general float
         * bounds (e.g. FLOAT_T).
         * @param lower The desired lower bound.
         * @param upper The desired upper bound.
         */
        template<typename Num = Number, typename Float, EnableIf<is_rational<Num >> = dummy, EnableIf<std::is_floating_point<Float >> = dummy, DisableIf<std::is_same<Num, Float >> = dummy>
        Interval(Float lower, Float upper):
			mContent(), mLowerBoundType(), mUpperBoundType()
        {
            *this = Interval<double>(lower, BoundType::WEAK, upper, BoundType::WEAK);
			Policy::sanitize(*this);
        }

        /**
         * Constructor which constructs the interval according to the passed general
         * float bounds (e.g. FLOAT_T) with the passed bound types. Note that if the interval is a
         * pointinterval with both strict bounds or the content is invalid the
         * empty interval is constru
         * @param lower The desired lower bound.
         * @param lowerBoundType The desired lower bound type.
         * @param upper The desired upper bound.
         * @param upperBoundType The desired upper bound type.
         */
        template<typename Num = Number, typename Float, EnableIf<is_rational<Num >> = dummy, EnableIf<std::is_floating_point<Float >> = dummy, DisableIf<std::is_same<Num, Float >> = dummy, DisableIf<std::is_floating_point<Num >> = dummy>
        Interval(Float lower, BoundType lowerBoundType, Float upper, BoundType upperBoundType):
			mContent(), mLowerBoundType(lowerBoundType), mUpperBoundType(upperBoundType)
        {
            Num left = carl::rationalize<Num>(toDouble(lower));
            Num right = carl::rationalize<Num>(toDouble(upper));
            //if(left == -std::numeric_limits<double>::infinity()) mLowerBoundType = BoundType::INFTY;
            //if(right == std::numeric_limits<double>::infinity()) mUpperBoundType = BoundType::INFTY;
            if (mLowerBoundType == BoundType::INFTY && mUpperBoundType == BoundType::INFTY)
            {
                mContent = BoostInterval(carl::constant_zero<Num>().get());
            }
            else if (mLowerBoundType == BoundType::INFTY)
            {
                mContent = BoostInterval(right);
            }
            else if (mUpperBoundType == BoundType::INFTY)
            {
                mContent = BoostInterval(left);
            }
            else if ((lower == upper && lowerBoundType != upperBoundType) || lower > upper)
            {
                mLowerBoundType = BoundType::STRICT;
                mUpperBoundType = BoundType::STRICT;
                mContent = BoostInterval(carl::constant_zero<Num>().get());
            }
            else
            {
                mContent = BoostInterval(left, right);
            }
			Policy::sanitize(*this);
        }

		/**
         * Constructor which constructs a pointinterval from a passed general
         * float number (e.g. FLOAT_T).
         * @param n The passed double.
         */
        template<typename Num = Number, typename Rational, EnableIf<is_rational<Num >> = dummy, EnableIf<is_rational<Rational >> = dummy, DisableIf<std::is_same<Num, Rational >> = dummy>
        explicit Interval(Rational n):
			mContent(), mLowerBoundType(), mUpperBoundType()
        {
            *this = Interval<double>(n, n);
			Policy::sanitize(*this);
        }

        /**
         * Constructor which constructs an interval from the passed general float
         * bounds (e.g. FLOAT_T).
         * @param lower The desired lower bound.
         * @param upper The desired upper bound.
         */
        template<typename Num = Number, typename Rational, EnableIf<is_rational<Num >> = dummy, EnableIf<is_rational<Rational >> = dummy, DisableIf<std::is_same<Num, Rational >> = dummy>
        Interval(Rational lower, Rational upper):
			mContent(), mLowerBoundType(), mUpperBoundType()
        {
            *this = Interval<double>(lower, BoundType::WEAK, upper, BoundType::WEAK);
			Policy::sanitize(*this);
        }

        /**
         * Constructor which constructs the interval according to the passed general
         * float bounds (e.g. FLOAT_T) with the passed bound types. Note that if the interval is a
         * pointinterval with both strict bounds or the content is invalid the
         * empty interval is constru
         * @param lower The desired lower bound.
         * @param lowerBoundType The desired lower bound type.
         * @param upper The desired upper bound.
         * @param upperBoundType The desired upper bound type.
         */
        template<typename Num = Number, typename Rational, EnableIf<is_rational<Num >> = dummy, EnableIf<is_rational<Rational >> = dummy, DisableIf<std::is_same<Num, Rational >> = dummy>
        Interval(Rational lower, BoundType lowerBoundType, Rational upper, BoundType upperBoundType)
        {
            mLowerBoundType = lowerBoundType;
            mUpperBoundType = upperBoundType;
            Num left = carl::rationalize<Num>(toDouble(lower));
            Num right = carl::rationalize<Num>(toDouble(upper));
            if (mLowerBoundType == BoundType::INFTY && mUpperBoundType == BoundType::INFTY)
            {
                mContent = BoostInterval(carl::constant_zero<Num>().get());
            }
            else if (mLowerBoundType == BoundType::INFTY)
            {
                mContent = BoostInterval(right);
            }
            else if (mUpperBoundType == BoundType::INFTY)
            {
                mContent = BoostInterval(left);
            }
            else if ((lower == upper && lowerBoundType != upperBoundType) || lower > upper)
            {
                mLowerBoundType = BoundType::STRICT;
                mUpperBoundType = BoundType::STRICT;
                mContent = BoostInterval(carl::constant_zero<Num>().get());
            }
            else
            {
                mContent = BoostInterval(left, right);
            }
			Policy::sanitize(*this);
        }

        /**
         * Method which returns the unbounded interval rooted at 0.
         * @return Unbounded interval.
         */
        static Interval<Number> unboundedInterval()
        {
            return Interval<Number>(carl::constant_zero<Number>().get(), BoundType::INFTY, carl::constant_zero<Number>().get(), BoundType::INFTY);
        }

        /**
         * Method which returns the empty interval rooted at 0.
         * @return Empty interval.
         */
        static Interval<Number> emptyInterval()
        {
            return Interval<Number>(carl::constant_zero<Number>().get(), BoundType::STRICT, carl::constant_zero<Number>().get(), BoundType::STRICT);
        }

        /**
         * Method which returns the pointinterval rooted at 0.
         * @return Pointinterval(0).
         */
        static Interval<Number> zeroInterval()
        {
            return Interval<Number>(carl::constant_zero<Number>().get(), BoundType::WEAK, carl::constant_zero<Number>().get(), BoundType::WEAK);
        }

        /**
         * Destructor
         */
        ~Interval() = default;

        /***********************************************************************
         * Getter & Setter
         **********************************************************************/

        /**
         * The getter for the lower boundary of the interval.
         * @return Lower interval boundary.
         */
        inline const Number& lower() const
        {
            return mContent.lower();
        }

        /**
         * The getter for the upper boundary of the interval.
         * @return Upper interval boundary.
         */
        inline const Number& upper() const
        {
            return mContent.upper();
        }

        /**
         * Returns a reference to the included boost interval.
         * @return Boost interval reference.
         */
        inline BoostInterval& rContent()
        {
            return mContent;
        }

        /**
         * Returns a copy of the included boost interval.
         * @return Boost interval.
         */
        inline BoostInterval content() const
        {
            return mContent;
        }

        /**
         * The getter for the lower bound type of the interval.
         * @return Lower bound type.
         */
        inline BoundType lowerBoundType() const
        {
            return mLowerBoundType;
        }

        /**
         * The getter for the upper bound type of the interval.
         * @return Upper bound type.
         */
        inline BoundType upperBoundType() const
        {
            return mUpperBoundType;
        }

        /**
         * The setter for the lower boundary of the interval.
         * @param n Lower boundary.
         */
        inline void setLower(const Number& n)
        {
            this->set(n, mContent.upper());
        }

        /**
         * The setter for the upper boundary of the interval.
         * @param n Upper boundary.
         */
        inline void setUpper(const Number& n)
        {
            this->set(mContent.lower(), n);
        }

        /**
         * The setter for the lower boundary of the interval.
         * @param n Lower boundary.
         */
        inline void setLowerBound(const Number& n, BoundType b)
        {
			/// TODO: Fix this.
        	if(b == BoundType::INFTY) {
				this->set(mContent.upper(), mContent.upper());
        		mLowerBoundType = b;
        	} else {
        		if(mUpperBoundType == BoundType::INFTY){
        			this->set(n, n);
        			mLowerBoundType = b;
        			mUpperBoundType = BoundType::INFTY;
	        	} else {
	        		this->set(n, mContent.upper());
	        		mLowerBoundType = b;
	        	}
        	}
        }

        /**
         * The setter for the upper boundary of the interval.
         * @param n Upper boundary.
         */
        inline void setUpperBound(const Number& n, BoundType b)
        {
			/// TODO: Fix this.
            if(b == BoundType::INFTY) {
				this->set(mContent.lower(), mContent.lower());
        		mUpperBoundType = b;
        	} else {
        		if(mLowerBoundType == BoundType::INFTY){
        			this->set(n, n);
        			mUpperBoundType = b;
        			mLowerBoundType = BoundType::INFTY;
	        	} else {
	        		this->set(mContent.lower(), n);
	        		mUpperBoundType = b;
	        	}
        	}
        }

        /**
         * The setter for the lower bound type of the interval.
         * @param b Lower bound type.
         */
        inline void setLowerBoundType(BoundType b)
        {
			mLowerBoundType = b;
            if (b == BoundType::INFTY) {
				//setLower(carl::constant_zero<Number>().get());
				setLower(upper());
            }
        }

        /**
         * The setter for the upper bound type of the interval.
         * @param b Upper bound type.
         */
        inline void setUpperBoundType(BoundType b)
        {
			mUpperBoundType = b;
            if (b == BoundType::INFTY) {
				//setUpper(carl::constant_zero<Number>().get());
				setUpper(lower());
            }
        }

        /**
         * The assignment operator.
         * @param rhs Source interval.
         * @return
         */
        Interval<Number>& operator =(const Interval<Number>& rhs)
        {
            mContent = rhs.content();
            mLowerBoundType = rhs.lowerBoundType();
            mUpperBoundType = rhs.upperBoundType();
            return *this;
        }

        /*
         * Transformations and advanced getters/setters
         */

        /**
         * Advanced setter to modify both boundaries at once.
         * @param lower Lower boundary.
         * @param upper Upper boundary.
         */
        inline void set(const BoostInterval& content)
        {
			mContent = content;
        }

        /**
         * Advanced setter to modify both boundaries at once by passing a boost
         * interval
         * @param content Boost interval.
         */
        inline void set(const Number& lower, const Number& upper)
        {
			set(BoostInterval(lower, upper));
        }

        /**
         * Function which determines, if the interval is (-oo,oo).
         * @return True if both bounds are INFTY.
         */
        inline bool isInfinite() const
        {
            assert(this->isConsistent());
            return mLowerBoundType == BoundType::INFTY && mUpperBoundType == BoundType::INFTY;
        }

        /**
         * Function which determines, if the interval is unbounded.
         * @return True if at least one bound is INFTY.
         */
        inline bool isUnbounded() const
        {
            assert(this->isConsistent());
            return mLowerBoundType == BoundType::INFTY || mUpperBoundType == BoundType::INFTY;
        }

        /**
         * Function which determines, if the interval is half-bounded.
         * @return True if exactly one bound is INFTY.
         */
        inline bool isHalfBounded() const
        {
            assert(this->isConsistent());
            return (mLowerBoundType == BoundType::INFTY) != (mUpperBoundType == BoundType::INFTY);
        }

        /**
         * Function which determines, if the interval is empty.
         * @return True if the interval is empty.
         */
        inline bool isEmpty() const
        {
            assert(this->isConsistent());
			if (mContent.lower() == mContent.upper()) {
				if (mLowerBoundType == BoundType::STRICT && mUpperBoundType == BoundType::STRICT) return true;
				if (mLowerBoundType == BoundType::STRICT && mUpperBoundType == BoundType::WEAK) return true;
				if (mLowerBoundType == BoundType::WEAK && mUpperBoundType == BoundType::STRICT) return true;
			}
			return false;
        }

        /**
         * Function which determines, if the interval is a pointinterval.
         * @return True if this is a pointinterval.
         */
        inline bool isPointInterval() const
        {
            assert(this->isConsistent());
            return (this->isClosedInterval() && mContent.lower() == mContent.upper());
        }

        /**
         * Function which determines, if the interval is open.
         * @return True if both bounds are STRICT.
         */
        inline bool isOpenInterval() const
        {
            assert(this->isConsistent());
            return (mLowerBoundType == BoundType::STRICT && mUpperBoundType == BoundType::STRICT);
        }

        /**
         * Function which determines, if the interval is closed.
         * @return True if both bounds are WEAK.
         */
        inline bool isClosedInterval() const
        {
            assert(this->isConsistent());
            return (mLowerBoundType == BoundType::WEAK && mUpperBoundType == BoundType::WEAK);
        }

        /**
         * Function which determines, if the interval is the zero interval.
         * @return True if it is a pointinterval rooted at 0.
         */
        inline bool isZero() const
        {
            assert(this->isConsistent());
            return this->isPointInterval() && (mContent.lower() == carl::constant_zero<Number>().get());
        }

		/**
         * Function which determines, if the interval is the one interval.
         * @return True if it is a pointinterval rooted at 1.
         */
        inline bool isOne() const
        {
            assert(this->isConsistent());
            return this->isPointInterval() && (mContent.lower() == carl::constant_one<Number>().get());
        }

		/**
         * @return true, if it this interval contains only positive values.
         */
        inline bool isPositive() const
        {
            assert(this->isConsistent());
            if( mLowerBoundType == BoundType::WEAK )
                return mContent.lower() > carl::constant_zero<Number>().get();
            if( mLowerBoundType == BoundType::STRICT )
                return mContent.lower() >= carl::constant_zero<Number>().get();
            return false;
        }

		/**
         * @return true, if it this interval contains only negative values.
         */
        inline bool isNegative() const
        {
            assert(this->isConsistent());
            if( mUpperBoundType == BoundType::WEAK )
                return mContent.upper() < carl::constant_zero<Number>().get();
            if( mUpperBoundType == BoundType::STRICT )
                return mContent.upper() <= carl::constant_zero<Number>().get();
            return false;
        }

		/**
         * @return true, if it this interval contains only positive values or 0.
         */
        inline bool isSemiPositive() const
        {
            assert(this->isConsistent());
            if( mLowerBoundType != BoundType::INFTY )
                return mContent.lower() >= carl::constant_zero<Number>().get();
            return false;
        }

		/**
         * @return true, if it this interval contains only negative values or 0.
         */
        inline bool isSemiNegative() const
        {
            assert(this->isConsistent());
            if( mUpperBoundType != BoundType::INFTY )
                return mContent.upper() <= carl::constant_zero<Number>().get();
            return false;
        }

        /**
         * Determine whether the interval lays entirely left of 0 (NEGATIVE_SIGN), right of 0 (POSITIVE_SIGN) or contains 0 (ZERO_SIGN).
         * @return NEGATIVE_SIGN, if the interval lays entirely left of 0; POSITIVE_SIGN, if right of 0; or ZERO_SIGN, if contains 0.
         */
        inline Sign sgn() const;

        /**
         * Computes the integral part of the given interval.
         * @return Interval.
         */
        Interval<Number> integralPart() const;

        /**
         * Computes and assigns the integral part of the given interval.
         * @return Interval.
         */
        void integralPart_assign();

        /**
         * Checks if the interval contains at least one integer value.
         * @return true, if the interval contains an integer.
         */
        bool containsInteger() const;

        /**
         * Returns the diameter of the interval.
         * @return Diameter.
         */
        Number diameter() const;

        /**
         * Computes and assigns the diameter of the interval.
         */
        void diameter_assign();

        /**
         * Returns the ratio of the diameters of the given intervals.
         * @param rhs Other interval.
         * @return Ratio.
         */
        Number diameterRatio(const Interval<Number>& rhs) const;

        /**
         * Computes and assigns the ratio of the diameters of the given intervals.
         * @param rhs Other interval.
         */
        void diameterRatio_assign(const Interval<Number>& rhs);

        /**
         * Returns the magnitude of the interval.
         * @return Magnitude.
         */
        Number magnitude() const;

        /**
         * Computes and assigns the magnitude of the interval.
         */
        void magnitude_assign();

        /**
         * Returns the center point of the interval.
         * @return Center.
         */
        template<typename N = Number, DisableIf<std::is_floating_point<N>> = dummy>
		N center() const
		{
			assert(this->isConsistent());
			if (this->isInfinite()) return carl::constant_zero<Number>().get();
			if (this->mLowerBoundType == BoundType::INFTY)
				return N(carl::floor(this->mContent.upper()) - carl::constant_one<Number>().get());
			if (this->mUpperBoundType == BoundType::INFTY)
				return N(carl::ceil(this->mContent.lower()) + carl::constant_one<Number>().get());
			return boost::numeric::median(mContent);
		}

		template<typename N = Number, EnableIf<std::is_floating_point<N>> = dummy>
		N center() const
		{
			assert(this->isConsistent());
			if (this->isInfinite()) return carl::constant_zero<N>().get();
			if (this->mLowerBoundType == BoundType::INFTY)
				return N(std::nextafter(this->mContent.upper(), -INFINITY));
			if (this->mUpperBoundType == BoundType::INFTY)
				return N(std::nextafter(this->mContent.lower(), INFINITY));
			return boost::numeric::median(mContent);
		}

        /**
         * Computes and assigns the center point of the interval.
         */
        void center_assign();

        /**
         * Searches for some point in this interval, preferably near the midpoint and with a small representation.
		 * Checks the integers next to the midpoint, uses the midpoint if both are outside.
         * @return Some point within this interval.
         */
        Number sample(bool _includingBounds = true) const;

		/**
		 * Searches for some point in this interval, preferably near the midpoint and with a small representation.
		 * Uses a binary search based on the Stern-Brocot tree starting from the integer below the midpoint.
		 * @return Some point within this interval.
		 */
		Number sampleSB(bool _includingBounds = true) const;

        /**
         * Searches for some point in this interval, preferably near the midpoint and with a small representation and
         * assigns this interval the calculated point.
         */
        void sample_assign();

        /**
         * Checks if the interval contains the given value.
         * @param val Value to be checked.
         * @return True if the value is contained in this.
         */
        bool contains(const Number& val) const;

		template<typename Num = Number, DisableIf<std::is_same<Num, int >> = dummy>
		bool contains(int val) const
		{
			return this->contains(Number(val));
		}

        /**
         * Checks if the interval contains the given interval.
         * @param rhs Interval to be checked.
         * @return True if rhs is contained in this.
         */
        bool contains(const Interval<Number>& rhs) const;

        /**
         * Checks if the interval meets the given value, that is if the given value is contained in the <b>closed</b> interval defined by the bounds.
         * @param val Value to be checked.
         * @return True if val is fully contained in this.
         */
        bool meets(const Number& n) const;

        /**
         * Checks if the given interval is a subset of the calling interval.
         * @param rhs Interval to be checked.
         * @return True if rhs is contained in this.
         */
        bool isSubset(const Interval<Number>& rhs) const;

        /**
         * Checks if the given interval is a proper subset of the calling interval.
         * @param rhs Interval to be checked.
         * @return True if rhs is fully contained in this.
         */
        bool isProperSubset(const Interval<Number>& rhs) const;

        /**
         * Bloats the interval by the given value.
         * @param width Width.
         */
        void bloat_by(const Number& width);

        /**
         * Bloats the interval times the factor (multiplies the overall width).
         * @param factor Factor.
         */
        void bloat_times(const Number& factor);

        /**
         * Shrinks the interval by the given value.
         * @param width Width.
         */
        void shrink_by(const Number& width);

        /**
         * Shrinks the interval by a multiple of its width.
         * @param factor Factor.
         */
        void shrink_times(const Number& factor);

        /**
         * Splits the interval into 2 equally sized parts (strict-weak-cut).
         * @return pair<interval, interval>.
         */
        std::pair<Interval<Number>, Interval<Number >> split() const;

        /**
         * Splits the interval into n equally sized parts (strict-weak-cut).
         * @return list<interval>.
         */
        std::list<Interval<Number >> split(unsigned n) const;

        /**
         * Creates a string representation of the interval.
         * @return String representation of this.
         */
        std::string toString() const;

        /**
         * Operator which passes a string representation of this to the given ostream.
         * @param str The ostream.
         * @param i The interval.
         * @return A reference to ostream.
         */
        friend inline std::ostream& operator <<(std::ostream& str, const Interval<Number>& i)
        {
            switch (i.mLowerBoundType)
            {
                case BoundType::INFTY:
                    str << "]-INF, ";
                    break;
                case BoundType::STRICT:
                    str << "]" << i.mContent.lower() << ", ";
                    break;
                case BoundType::WEAK:
                    str << "[" << i.mContent.lower() << ", ";
            }
            switch (i.mUpperBoundType)
            {
                case BoundType::INFTY:
                    str << "INF[";
                    break;
                case BoundType::STRICT:
                    str << i.mContent.upper() << "[";
                    break;
                case BoundType::WEAK:
                    str << i.mContent.upper() << "]";
            }
            return str;
        }

        /*
         * Arithmetic functions
         */

        /**
         * Adds two intervals according to natural interval arithmetic.
         * @param rhs Interval.
         * @return Result.
         */
        Interval<Number> add(const Interval<Number>& rhs) const;
        void add_assign(const Interval<Number>& rhs);

        /**
         * Subtracts two intervals according to natural interval arithmetic.
         * @param rhs Interval.
         * @return Result.
         */
        Interval<Number> sub(const Interval<Number>& rhs) const;
        void sub_assign(const Interval<Number>& rhs);

        /**
         * Multiplies two intervals according to natural interval arithmetic.
         * @param rhs Interval.
         * @return Result.
         */
        Interval<Number> mul(const Interval<Number>& rhs) const;
        void mul_assign(const Interval<Number>& rhs);

        /**
         * Divides two intervals according to natural interval arithmetic.
         * @param rhs Interval.
         * @return Result.
         */
        Interval<Number> div(const Interval<Number>& rhs) const;
        void div_assign(const Interval<Number>& rhs);

        /**
         * Implements extended interval division with intervals containting zero.
         * @param rhs Interval.
         * @param a Result a.
         * @param b Result b.
         * @return True if split occurred.
         */
        bool div_ext(const Interval<Number>& rhs, Interval<Number>& a, Interval<Number>& b) const;

        /**
         * Calculates the additive inverse of an interval with respect to natural interval arithmetic.
         * @return Interval.
         */
        Interval<Number> inverse() const;

        /**
         * Calculates the absolute value of the interval.
         * @return Interval.
         */
        Interval<Number> abs() const;

        /**
         * Calculates and assigns the absolute value of the interval.
         */
        void abs_assign();

        /**
         * Calculates and assigns the additive inverse of an interval with respect
         * to natural interval arithmetic.
         */
        void inverse_assign();

        /**
         * Calculates the multiplicative inverse of an interval with respect to natural interval arithmetic.
         * @param a Result a.
         * @param b Result b.
         * @return True, if split occured.
         */
        bool reciprocal(Interval<Number>& a, Interval<Number>& b) const;

        /**
         * Calculates the power of the interval with respect to natural interval arithmetic.
         * @param exp Exponent.
         * @return Result.
         */
        Interval<Number> pow(uint exp) const;

        /**
         * Calculates and assigns the power of the interval with respect to natural interval arithmetic.
         * @param exp Exponent.
         */
        void pow_assign(uint exp);

        /**
         * Calculates the square root of the interval with respect to natural interval arithmetic.
         * @return Result.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        Interval<Number> sqrt() const;

        /**
         * Calculates and assigns the square root of the interval with respect to natural interval arithmetic.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        void sqrt_assign();

        /**
         * Calculates the nth root of the interval with respect to natural interval arithmetic.
         * @param deg Degree.
         * @return Result.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        Interval<Number> root(int deg) const;

        /**
         * Calculates and assigns the nth root of the interval with respect to natural interval arithmetic.
         * @param deg Degree.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        void root_assign(unsigned deg);

        /**
         * Calculates the logarithm of the interval with respect to natural interval arithmetic.
         * @return Result.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        Interval<Number> log() const;

        /**
         * Calculates and assigns the logarithm of the interval with respect to natural interval arithmetic.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        void log_assign();

        /*
         * Trigonometric functions
         */

        /**
         * Calculates the sine of the given interval with respect to natural interval arithmetic.
         * @return Result.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        Interval<Number> sin() const;

        /**
         * Calculates and assigns the sine of the given interval with respect to natural interval arithmetic.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        void sin_assign();

        /**
         * Calculates the cosine of the given interval with respect to natural interval arithmetic.
         * @return Result.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        Interval<Number> cos() const;

        /**
         * Calculates and assigns the cosine of the given interval with respect to natural interval arithmetic.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        void cos_assign();

        /**
         * Calculates the tangent of the given interval with respect to natural interval arithmetic.
         * @return Result.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        Interval<Number> tan() const;

        /**
         * Calculates and assigns the tangent of the given interval with respect to natural interval arithmetic.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        void tan_assign();

        /**
         * Calculates the arcus sine of the given interval with respect to natural interval arithmetic.
         * @return Result.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        Interval<Number> asin() const;

        /**
         * Calculates and assigns the arcus sine of the given interval with respect to natural interval arithmetic.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        void asin_assign();

        /**
         * Calculates the arcus cosine of the given interval with respect to natural interval arithmetic.
         * @return Result.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        Interval<Number> acos() const;

        /**
         * Calculates and assigns the arcus cosine of the given interval with respect to natural interval arithmetic.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        void acos_assign();

        /**
         * Calculates the arcus tangent of the given interval with respect to natural interval arithmetic.
         * @return Result.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        Interval<Number> atan() const;

        /**
         * Calculates and assigns the arcus tangent of the given interval with respect to natural interval arithmetic.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        void atan_assign();

        /**
         * Calculates the hyperbolic sine of the given interval with respect to natural interval arithmetic.
         * @return Result.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        Interval<Number> sinh() const;

        /**
         * Calculates and assigns the hyperbolic sine of the given interval with respect to natural interval arithmetic.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        void sinh_assign();

        /**
         * Calculates the hyperbolic cosine of the given interval with respect to natural interval arithmetic.
         * @return Result.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        Interval<Number> cosh() const;

        /**
         * Calculates and assigns the hyperbolic cosine of the given interval with respect to natural interval arithmetic.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        void cosh_assign();

        /**
         * Calculates the hyperbolic tangent of the given interval with respect to natural interval arithmetic.
         * @return Result.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        Interval<Number> tanh() const;

        /**
         * Calculates and assigns the hyperbolic tangent of the given interval with respect to natural interval arithmetic.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        void tanh_assign();

        /**
         * Calculates the hyperbolic arcus sine of the given interval with respect to natural interval arithmetic.
         * @return Result.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        Interval<Number> asinh() const;

        /**
         * Calculates and assigns the hyperbolic arcus sine of the given interval with respect to natural interval arithmetic.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        void asinh_assign();

        /**
         * Calculates the hyperbolic arcus cosine of the given interval with respect to natural interval arithmetic.
         * @return Result.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        Interval<Number> acosh() const;

        /**
         * Calculates and assigns the hyperbolic arcus cosine of the given interval with respect to natural interval arithmetic.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        void acosh_assign();

        /**
         * Calculates the hyperbolic arcus tangent of the given interval with respect to natural interval arithmetic.
         * @return Result.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        Interval<Number> atanh() const;

        /**
         * Calculates and assigns the hyperbolic arcus tangent of the given interval with respect to natural interval arithmetic.
         */
        template<typename Num = Number, EnableIf<std::is_floating_point<Num>> = dummy>
        void atanh_assign();

        /*
         * Boolean Operations
         */

        bool intersectsWith(const Interval<Number>& rhs) const;

        /**
         * Intersects two intervals in a set-theoretic manner.
         * @param rhs Righthand side.
         * @return Result.
         */
        Interval<Number> intersect(const Interval<Number>& rhs) const;

        /**
         * Intersects and assigns two intervals in a set-theoretic manner.
         * @param rhs Righthand side.
         * @return Reference to this.
         */
        Interval<Number>& intersect_assign(const Interval<Number>& rhs);

        /**
         * Unites two intervals in a set-theoretic manner (can result in two distinct intervals).
         * @param rhs Righthand side.
         * @param resultA Result a.
         * @param resultB Result b.
         * @return True, if the result is twofold.
         */
        bool unite(const Interval<Number>& rhs, Interval<Number>& resultA, Interval<Number>& resultB) const;

        /**
         * Calculates the difference of two intervals in a set-theoretic manner:
         * lhs - rhs (can result in two distinct intervals).
         * @param rhs Righthand side.
         * @param resultA Result a.
         * @param resultB Result b.
         * @return True, if the result is twofold.
         */
        bool difference(const Interval<Number>& rhs, Interval<Number>& resultA, Interval<Number>& resultB) const;

        /**
         * Calculates the complement in a set-theoretic manner (can result
         * in two distinct intervals).
         * @param resultA Result a.
         * @param resultB Result b.
         * @return True, if the result is twofold.
         */
        bool complement(Interval<Number>& resultA, Interval<Number>& resultB) const;

        /**
         * Calculates the symmetric difference of two intervals in a
         * set-theoretic manner (can result in two distinct intervals).
         * @param rhs Righthand side.
         * @param resultA Result a.
         * @param resultB Result b.
         * @return True, if the result is twofold.
         */
        bool symmetricDifference(const Interval<Number>& rhs, Interval<Number>& resultA, Interval<Number>& resultB) const;

        /**
         * A quick check for the bound values.
         * @return True if the lower bound is less or equal to the upper bound.
         */
        bool isConsistent() const
        {
            return this->lower() <= this->upper();
        }
    /**
    * Calculates the distance between two Intervals.
    * @param intervalA Interval to wich we want to know the distance.
    * @return distance to intervalA
    */
    Number distance(const Interval<Number>& intervalA);

    Interval<Number> convexHull(const Interval<Number>& interval) const;

    };

	template<typename T>
	struct is_number<Interval<T>> : std::true_type {};

    /*
     * Overloaded arithmetics operators
     */

    /**
     * Operator for the addition of two intervals.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number> operator+(const Interval<Number>& lhs, const Interval<Number>& rhs);

    /**
     * Operator for the addition of an interval and a number.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number> operator+(const Number& lhs, const Interval<Number>& rhs);

    /**
     * Operator for the addition of an interval and a number.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number> operator+(const Interval<Number>& lhs, const Number& rhs);

    /**
     * Operator for the addition of an interval and a number with assignment.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number>& operator+=(Interval<Number>& lhs, const Interval<Number>& rhs);

    /**
     * Operator for the addition of an interval and a number with assignment.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number>& operator+=(Interval<Number>& lhs, const Number& rhs);

    /**
     * Unary minus.
     * @param interval The operand.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number> operator-(const Interval<Number>& interval);

    /**
     * Operator for the subtraction of two intervals.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number> operator-(const Interval<Number>& lhs, const Interval<Number>& rhs);

    /**
     * Operator for the subtraction of an interval and a number.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number> operator-(const Number& lhs, const Interval<Number>& rhs);

    /**
     * Operator for the subtraction of an interval and a number.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number> operator-(const Interval<Number>& lhs, const Number& rhs);

	/**
     * Operator for the subtraction of two intervals with assignment.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
	template<typename Number>
    inline Interval<Number>& operator-=(Interval<Number>& lhs, const Interval<Number>& rhs);

    /**
     * Operator for the subtraction of an interval and a number with assignment.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number>& operator-=(Interval<Number>& lhs, const Number& rhs);

    /**
     * Operator for the multiplication of two intervals.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number> operator*(const Interval<Number>& lhs, const Interval<Number>& rhs);

    /**
     * Operator for the multiplication of an interval and a number.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number> operator*(const Number& lhs, const Interval<Number>& rhs);

    /**
     * Operator for the multiplication of an interval and a number.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number> operator*(const Interval<Number>& lhs, const Number& rhs);

    /**
     * Operator for the multiplication of an interval and a number with assignment.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number>& operator*=(Interval<Number>& lhs, const Interval<Number>& rhs);

    /**
     * Operator for the multiplication of an interval and a number with assignment.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number>& operator*=(Interval<Number>& lhs, const Number& rhs);

    /**
     * Operator for the division of an interval and a number.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number> operator/(const Interval<Number>& lhs, const Number& rhs);

    /**
     * Operator for the division of an interval and a number with assignment.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return Resulting interval.
     */
    template<typename Number>
    inline Interval<Number>& operator/=(Interval<Number>& lhs, const Number& rhs);

    /*
     * Comparison operators
     */

    /**
     * Operator for the comparison of two intervals.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return True if both intervals are equal.
     */
    template<typename Number>
    inline bool operator==(const Interval<Number>& lhs, const Interval<Number>& rhs);

    /**
     * Operator for the comparison of two intervals.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return True if both intervals are unequal.
     */
    template<typename Number>
    inline bool operator!=(const Interval<Number>& lhs, const Interval<Number>& rhs);

    /**
     * Operator for the comparison of two intervals.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return True if the righthand side has maximal one intersection with the lefthand side
     * at the upper bound of lhs.
     */
    template<typename Number>
    inline bool operator<=(const Interval<Number>& lhs, const Interval<Number>& rhs);

    /**
     * Operator for the comparison of two intervals.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return True if the lefthand side has maximal one intersection with the righthand side
     * at the lower bound of lhs.
     */
    template<typename Number>
    inline bool operator>=(const Interval<Number>& lhs, const Interval<Number>& rhs);

    /**
     * Operator for the comparison of two intervals.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return True if the lefthand side is smaller than the righthand side.
     */
    template<typename Number>
    inline bool operator<(const Interval<Number>& lhs, const Interval<Number>& rhs);

    /**
     * Operator for the comparison of two intervals.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return True if the lefthand side is larger than the righthand side.
     */
    template<typename Number>
    inline bool operator>(const Interval<Number>& lhs, const Interval<Number>& rhs);

    /**
     * Operator for the comparison of two intervals.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return True if the righthand side has maximal one intersection with the lefthand side
     * at the upper bound of lhs.
     */
    template<typename Number>
    inline bool operator<=(const Interval<Number>& lhs, const Number& rhs);
    template<typename Number>
    inline bool operator<=(const Number& lhs, const Interval<Number>& rhs)
    {
        return rhs>=lhs;
    }

    /**
     * Operator for the comparison of two intervals.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return True if the lefthand side has maximal one intersection with the righthand side
     * at the lower bound of lhs.
     */
    template<typename Number>
    inline bool operator>=(const Interval<Number>& lhs, const Interval<Number>& rhs);
    template<typename Number>
    inline bool operator>=(const Number& lhs, const Interval<Number>& rhs)
    {
        return rhs<=lhs;
    }

    /**
     * Operator for the comparison of two intervals.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return True if the lefthand side is smaller than the righthand side.
     */
    template<typename Number>
    inline bool operator<(const Interval<Number>& lhs, const Interval<Number>& rhs);
    template<typename Number>
    inline bool operator<(const Number& lhs, const Interval<Number>& rhs)
    {
        return rhs>lhs;
    }

    /**
     * Operator for the comparison of two intervals.
     * @param lhs Lefthand side.
     * @param rhs Righthand side.
     * @return True if the lefthand side is larger than the righthand side.
     */
    template<typename Number>
    inline bool operator>(const Interval<Number>& lhs, const Interval<Number>& rhs);
    template<typename Number>
    inline bool operator>(const Number& lhs, const Interval<Number>& rhs)
    {
        return rhs<lhs;
    }


	template<typename Number>
	inline bool isInteger(const Interval<Number>& n) {
		return n.isPointInterval() && carl::isInteger(n.lower());
	}

    /**
     * Implements the division which assumes that there is no remainder.
     * @param _lhs
     * @param _rhs
     * @return Interval which holds the result.
     */
    template<typename Number>
    inline Interval<Number> div(const Interval<Number>& _lhs, const Interval<Number>& _rhs)
    {
        return _lhs.div(_rhs);
    }

    /**
     * Implements the division with remainder.
     * @param _lhs
     * @param _rhs
     * @return Interval which holds the result.
     */
    template<typename Number>
    inline Interval<Number> quotient(const Interval<Number>& _lhs, const Interval<Number>& _rhs)
    {
        return _lhs.div(_rhs);
    }

    /**
     * Casts the Interval to an arbitrary integer type which has a constructor for
     * a native int.
     * @param _floatInterval
     * @return Integer type which holds floor(_float).
     */
    template<typename Integer, typename Number>
    inline Integer toInt(const Interval<Number>& _floatInterval)
    {
        return Interval<Integer>(_floatInterval.lower(), _floatInterval.lowerBoundType(), _floatInterval.upper(), _floatInterval.upperBoundType());
    }

    /**
     * Method which returns the absolute value of the passed number.
     * @param _in Number.
     * @return Number which holds the result.
     */
    template<typename Number>
    inline Interval<Number> abs(const Interval<Number>& _in)
    {
        return _in.abs();
    }

    /**
     * Method which returns the logarithm of the passed number.
     * @param _in Number.
     * @return Number which holds the result.
     */
    template<typename Number>
    inline Interval<Number> log(const Interval<Number>& _in)
    {
        return _in.log();
    }

    /**
     * Method which returns the square root of the passed number.
     * @param _in Number.
     * @return Number which holds the result.
     */
    template<typename Number>
    inline Interval<Number> sqrt(const Interval<Number>& _in)
    {
        return _in.sqrt();
    }


    template<typename Number>
    inline Interval<Number> pow(const Interval<Number>& _in, uint _exponent)
    {
        return _in.pow(_exponent);
    }

    /**
     * Method which returns the next smaller integer of this number or the number
     * itself, if it is already an integer.
     * @param _in Number.
     * @return Number which holds the result.
     */
    template<typename Number>
    inline Interval<Number> floor(const Interval<Number>& _in)
    {
        return Interval<Number>(floor(_in.lower()), _in.lowerBoundType(), floor(_in.upper()), _in.upperBoundType());
    }

    /**
     * Method which returns the next larger integer of the passed number or the
     * number itself, if it is already an integer.
     * @param _in Number.
     * @return Number which holds the result.
     */
    template<typename Number>
    inline Interval<Number> ceil(const Interval<Number>& _in)
    {
        return Interval<Number>(ceil(_in.lower()), _in.lowerBoundType(), ceil(_in.upper()), _in.upperBoundType());
    }

}

namespace std
{
	/**
	 * Specialization of `std::hash` for an interval.
	 */
	template<typename Number>
	struct hash<carl::Interval<Number>> {
		/**
		 * Calculates the hash of an interval.
		 * @param interval An interval.
		 * @return Hash of an interval.
		 */
		size_t operator()(const carl::Interval<Number>& interval) const
        {
        	size_t result = 0;
        	boost::hash_combine(result, static_cast<size_t>(interval.lowerBoundType()));
        	boost::hash_combine(result, static_cast<size_t>(interval.upperBoundType()));
        	std::hash<Number> h;
        	boost::hash_combine(result, h(interval.upper()));
        	boost::hash_combine(result, h(interval.lower()));

            return result;
		}
	};
} // namespace std

#include "Interval.tpp"
