/**
 * The class implements interval arithmetic for rational types.
 *
 * @file ExactInterval.h
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 * @version 2013-12-19
 */

#pragma once
#include "../core/Sign.h"
#include "../numbers/numbers.h"
#include "BoundType.h"

namespace carl
{
    template<typename Numeric> class ExactInterval;
    
    template<typename Numeric>
    std::ostream& operator<<(std::ostream& str, const ExactInterval<Numeric>&);

    //////////////
    // Typedefs //
    //////////////
    
    template<typename Numeric>
    class ExactInterval
    {
        public:
            
            //////////////////////////
            // Con- and destructors //
            //////////////////////////
            
            ExactInterval();
            
            /**
             * Constructs the point interval [n, n].
             * @param n
             */
            ExactInterval( const Numeric& n ):
                mLeft( n ),
                mLeftType( BoundType::WEAK ),
                mRight( n ),
                mRightType( BoundType::WEAK )
            {}

            /**
             * Constructs interval ]l, r[, [l, r[, ]l, r], or [l, r]. Special case: ]0, 0[ = ]-infinity, infinity[.
             * Note that by this constructor, decimal representations of rational Numerics are rationalized by GiNaC::rationalize;
             * @param l left bound
             * @param lType type of the left bound
             * @param r right bound
             * @param rType type of the right bound
             */
            ExactInterval( const Numeric& l, BoundType lType, const Numeric& r, BoundType rType );

			/**
			 * Constructs interval ]l, r[ or [l, r], depending on the given bound type.
			 * @param l left bound
			 * @param r right bound
			 * @param type type of the bounds
			 */
			ExactInterval( const Numeric& l, const Numeric& r, BoundType type);

            /**
             * Constructs an open interval from another.
             * @param i other open interval
             */
            ExactInterval( const ExactInterval<Numeric>& i );

            /**
             * Destructor.
             */
            ~ExactInterval();

            ///////////////
            // Selectors //
            ///////////////

            /**
             * Set new left bound for the interval.
             * @param l new left bound
             */
            void setLeft( const Numeric& l )
            {
                mLeft = l;
            }

            /**
             * Set new left bound type for the interval.
             * @param lType new left bound type
             */
            void setLeftType( BoundType lType )
            {
                mLeftType = lType;
            }

            /**
             * Set new right bound for the interval.
             * @param r new right bound
             */
            void setRight( const Numeric& r )
            {
                mRight = r;
            }

            /**
             * Set new right bound type for the interval.
             * @param rType new right bound type
             */
            void setRightType( const BoundType& rType )
            {
                mRightType = rType;
            }

            /**
             * Selects the left bound.
             * @return Numeric
             */
            const Numeric& left() const
            {
                return mLeft;
            }

            /**
             * Selects the right bound.
             * @return Numeric
             */
            const Numeric& right() const
            {
                return mRight;
            }
            
            void set( const Numeric& _left, const Numeric& _right )
            {
                mLeft = _left;
                mRight = _right;
            }
            
            void set( const Numeric& _c )
            {
                mLeft = _c;
                mRight = _c;
            }
            
            /**
             * Selects the left bound type.
             * @return bound type of the left bound
             */
            BoundType leftType() const
            {
                return mLeftType;
            }

            /**
             * Selects the right bound type.
             * @return bound type of the right bound
             */
            BoundType rightType() const
            {
                return mRightType;
            }

            ////////////////
            // Operations //
            ////////////////

            /**
             * @return true if the bounds define an empty interval, false otherwise
             */
            bool empty() const
            {
                return !(mLeftType == BoundType::INFTY || mRightType == BoundType::INFTY || left() < right() || ( left() == right() && mLeftType != BoundType::STRICT && mRightType != BoundType::STRICT ));
//                return !( BOUNDS_OK( mLeft, mLeftType, mRight, mRightType ) || ( mLeftType != BoundType::STRICT && mRightType != BoundType::STRICT && mLeft == mRight ) );
            }

			bool isZero() const {
				return (this->left() == 0) && (this->right() == 0);
			}

            /**
             * @return true if the bounds define the whole real line, false otherwise
             */
            bool unbounded() const
            {
                return mLeftType == BoundType::INFTY && mRightType == BoundType::INFTY;
            }

            /**
             * @return true if one side of the interval is unbounded
             */
            bool halfunbounded() const
            {
                return mLeftType == BoundType::INFTY || mRightType == BoundType::INFTY;
            }

            ////////////////
            // Operations //
            ////////////////

            /**
             * Determine whether the interval lays entirely left of 0 (NEGATIVE_SIGN), right of 0 (POSITIVE_SIGN) or contains 0 (ZERO_SIGN).
             * @return NEGATIVE_SIGN, if the interval lays entirely left of 0; POSITIVE_SIGN, if right of 0; or ZERO_SIGN, if contains 0.
             */
            Sign sgn();

            /**
             * @param n
             * @return true in case n is contained in this ExactInterval
             */
            bool contains( const Numeric& n ) const;

            /**
             * @param o
             * @return true in case o is a subset of this ExactInterval
             */
            bool contains( const ExactInterval<Numeric>& o ) const;

            /**
             * Checks whether n is contained in the <b>closed</b> interval defined by the bounds.
             * @param n
             * @return true in case n meets the interval bounds or a point inbetween
             */
            bool meets( const Numeric& n ) const;

            /**
             * @param o
             * @return intersection with the given ExactInterval and this ExactInterval, or (0, 0) in case the intersection is empty
             */
            ExactInterval<Numeric> intersect( const ExactInterval<Numeric>& o ) const;

            /**
             * @return the midpoint of this interval
             */
            Numeric midpoint() const;

			/**
			 * Searches for some point in this interval, preferably near the midpoint and with a small representation.
			 * @return some point within this interval
			 */
			Numeric sample() const;

            /**
             * Computes the absolute value of this interval, i.e. the maximum of the absolute values of its bounds.
             * @return absolute value of the interval
             * @see Marc Daumas, Guillaume Melquiond, and Cesar Munoz - "Guaranteed Proofs Using ExactInterval Arithmetic".
             */
            ExactInterval<Numeric> abs() const;

            ///////////////////////////
            // Arithmetic Operations //
            ///////////////////////////

            /** Adds two intervals and returns their sum.
             * @param o
             * @return sum
             */
            ExactInterval<Numeric> add( const ExactInterval<Numeric>& o ) const;

            /** Returns the negative value.
             * @return negative value
             */
            ExactInterval<Numeric> inverse() const;

            /** Multiplies two intervals and returns their product.
             * @param o
             * @return product
             */
            ExactInterval<Numeric> mul( const ExactInterval<Numeric>& o ) const;

            /** Divides two intervals.
             * @param o
             * @return this interval divided by the argument
             * @throws invalid_argument in case the argument interval contains zero
             */
            ExactInterval<Numeric> div( const ExactInterval<Numeric>& o ) const throw ( std::invalid_argument );

            /** Extended ExactIntervaldivision
             * @param a
             * @param b
             * @param o
             * @return true if interval splitting (results in a and b), false else (results only in a)
             */
            bool div_ext( ExactInterval<Numeric>& a, ExactInterval<Numeric>& b, const ExactInterval<Numeric>& o );

            /** Computes the power to <code>e</code> of this interval.
             * @param e exponent
             * @return power to <code>e</code> of this interval
             */
            ExactInterval<Numeric> power( unsigned e ) const;

            /** Computes the reciprocal to the interval with respect to division by zero and infinity
             * @param a first result reference
             * @param b second result reference
             * @return true if the result contains two intervals, else false
             */
            bool reciprocal( ExactInterval<Numeric>& a, ExactInterval<Numeric>& b ) const;

            /**
             * Computes the exp of the interval.
             * @return 
             */
            ExactInterval<Numeric> exp() const;
            
            /**
             * Computes the logarithm of the interval.
             * @return 
             */
            ExactInterval<Numeric> log() const;
            
            /**
            * Split the interval at the midpoint.
            * @param _left
            * @param _right
            */
           void split(ExactInterval<Numeric>& _left, ExactInterval<Numeric>& _right) const;

           /**
            * Split the interval in n uniform intervals contained in the _result vector.
            * @param _result
            * @param n
            */
           void split(std::vector<ExactInterval<Numeric> >& _result, const unsigned n) const;

           /**
            * Bloat the interval by the given width.
            * @param _width
            */
           void bloat(const Numeric& _width);
           
           /**
            * Calculates the diameter of the interval
            * @return the diameter of the interval
            */
           Numeric diameter() const;

           Numeric diameterRatio( const ExactInterval<Numeric>& _interval) const;

           /**
            * Calculate the magnitude of the interval.
            * @return 
            */
           Numeric magnitude() const;
            
            ///////////////////////////
            // Relational Operations //
            ///////////////////////////

            /**
             * @param o
             * @return true in case the other interval equals this
             */
            bool isEqual( const ExactInterval<Numeric>& o ) const;

            /** Checks whether the left bound of this interval is less or equal the left bound of the other interval.
             * @param o
             * @return true if the left bound of this interval is less or equal the left bound of the other interval
             */
            bool isLessOrEqual( const ExactInterval<Numeric>& o ) const;

            /** Checks whether the right bound of this interval is greater or equal the right bound of the other interval
             * @param o
             * @return true if the right bound of this interval is greater or equal the right bound of the other interval
             */
            bool isGreaterOrEqual( const ExactInterval<Numeric>& o ) const;

            ////////////////////
            // Static Methods //
            ////////////////////

            /**
             * Creates the empty interval denoted by ]0,0[
             * @return empty interval
             */
            static ExactInterval<Numeric> emptyExactInterval()
            {
                return ExactInterval( Numeric(0), BoundType::STRICT, Numeric(0), BoundType::STRICT );
            }

            /**
             * Creates the unbounded interval denoted by ]-infty,infty[
             * @return empty interval
             */
            static ExactInterval<Numeric> unboundedExactInterval()
            {
                return ExactInterval( Numeric(-1), BoundType::INFTY, Numeric(1), BoundType::INFTY );
            }

		void operator +=(const ExactInterval<Numeric>& o);
		void operator -=(const ExactInterval<Numeric>& o);
		void operator *=(const ExactInterval<Numeric>& o);
		
            friend std::ostream& operator<< <>(std::ostream& str, const ExactInterval<Numeric>&);
            
        protected:
            ////////////////
            // Attributes //
            ////////////////

            Numeric   mLeft;    // left bound of the interval
            BoundType mLeftType;    // type of the left bound (BoundType::STRICT or BoundType::WEAK)
            Numeric   mRight;    // right bound of the interval
            BoundType mRightType;    // type of the right bound (BoundType::STRICT or BoundType::WEAK)

        private:

            /////////////////////////
            // AUXILIARY FUNCTIONS //
            /////////////////////////

            /**
             * Return the bound type which corresponds to the weakest-possible type when combining all elements in two intervals.
             * @param type1
             * @param type2
             * @return BoundType::INFTY if one of the given types is INIFNITY_BOUND, BoundType::STRICT if one of the given types is BoundType::STRICT
             */
            inline static BoundType getWeakestBoundType( BoundType type1, BoundType type2 )
            {
                return (type1 == BoundType::INFTY || type2 == BoundType::INFTY)
                       ? BoundType::INFTY : (type1 == BoundType::STRICT || type2 == BoundType::STRICT) ? BoundType::STRICT : BoundType::WEAK;
            }

    };    // class ExactInterval
    
// Arithmetic operators
template<typename Numeric>
inline const ExactInterval<Numeric> operator +(const ExactInterval<Numeric>& lh, const ExactInterval<Numeric>& rh)
{
	return lh.add(rh);
}

template<typename Numeric>
inline const ExactInterval<Numeric> operator +(const ExactInterval<Numeric>& lh, const Numeric& rh)
{
	// TODO optimization potential
	return lh.add(ExactInterval<Numeric>(rh));
}

template<typename Numeric>
inline const ExactInterval<Numeric> operator +(const Numeric& lh, const ExactInterval<Numeric>& rh)
{
	// TODO optimization potential
	return rh.add(ExactInterval<Numeric>(lh));
}

template<typename Numeric>
inline const ExactInterval<Numeric> operator -(const ExactInterval<Numeric>& lh, const ExactInterval<Numeric>& rh)
{
	return lh.add(rh.inverse());
}

template<typename Numeric>
inline const ExactInterval<Numeric> operator -(const ExactInterval<Numeric>& lh, const Numeric& rh)
{
	return lh + (-rh);
}

template<typename Numeric>
inline const ExactInterval<Numeric> operator -(const Numeric& lh, const ExactInterval<Numeric>& rh)
{
	return rh.inverse() + lh;
}

template<typename Numeric>
inline const ExactInterval<Numeric> operator *(const ExactInterval<Numeric>& lh, const ExactInterval<Numeric>& rh)
{
	return lh.mul(rh);
}

template<typename Numeric>
inline const ExactInterval<Numeric> operator *(const ExactInterval<Numeric>& lh, const Numeric& rh)
{
	return ExactInterval<Numeric>(lh.mul(ExactInterval<Numeric>(rh)));
}

template<typename Numeric>
inline const ExactInterval<Numeric> operator *(const Numeric& lh, const ExactInterval<Numeric>& rh)
{
	return rh * lh;
}

template<typename Numeric>
inline const ExactInterval<Numeric> operator /(const ExactInterval<Numeric>& lh, const Numeric& rh) throw ( std::overflow_error)
{
	return lh.div(ExactInterval<Numeric>(rh));
}

template<typename Numeric>
inline const ExactInterval<Numeric> operator /(const Numeric& lh, const ExactInterval<Numeric>& rh) throw ( std::overflow_error)
{
	ExactInterval<Numeric> result = ExactInterval<Numeric>(lh);
	result.div(rh);
	return result;
}

// relational operators

    template<typename Numeric>
inline bool operator ==(const ExactInterval<Numeric>& lh, const ExactInterval<Numeric>& rh)
{
	return lh.isEqual(rh);
}

    template<typename Numeric>
inline bool operator !=(const ExactInterval<Numeric>& lh, const ExactInterval<Numeric>& rh)
{
	return !lh.isEqual(rh);
}

    template<typename Numeric>
inline bool operator <=(const ExactInterval<Numeric>& lh, const ExactInterval<Numeric>& rh)
{
	return lh.isLessOrEqual(rh);
}

    template<typename Numeric>
inline bool operator >=(const ExactInterval<Numeric>& lh, const ExactInterval<Numeric>& rh)
{
	return lh.isGreaterOrEqual(rh);
}

template<typename Numeric>
inline bool operator <(const ExactInterval<Numeric>& lh, const ExactInterval<Numeric>& rh)
{
	if(rh.leftType() != BoundType::INFTY)
	{
		if(lh.rightType() != BoundType::INFTY)
		{
			return lh.right() < rh.left() || (lh.right() == rh.left() && lh.rightType() == BoundType::STRICT && rh.leftType() == BoundType::STRICT);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

template<typename Numeric>
inline bool operator >(const ExactInterval<Numeric>& lh, const ExactInterval<Numeric>& rh)
{
	return rh < lh;
}
	
}

#include "ExactInterval.tpp"

