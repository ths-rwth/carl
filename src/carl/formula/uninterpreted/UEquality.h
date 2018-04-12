/**
 * @file UEquality.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-20
 * @version 2014-10-22
 */

#pragma once

#include "UVariable.h"
#include "UFInstance.h"
#include <boost/variant.hpp>

namespace carl
{
    /**
     * Implements an uninterpreted equality, that is an equality of either 
     *     two uninterpreted function instances,
     *     two uninterpreted variables,
     *     or an uninterpreted function instance and an uninterpreted variable. 
     */
    class UEquality
    {
        public:
            /// The type of the left and right-hand side of an uninterpreted equality.
            using Arg = boost::variant<UVariable, UFInstance>;

            /**
             * Checks whether the given argument is an uninterpreted variable.
             */
            struct IsUVariable: public boost::static_visitor<bool> 
            {
                /**
                 * @param An uninterpreted variable.
                 * @return true
                 */
                bool operator()(const UVariable& /*unused*/) const
                {
                    return true;
                }
                
                /**
                 * @param An uninterpreted function instance.
                 * @return false
                 */
                bool operator()(const UFInstance& /*unused*/) const 
                {
                    return false;
                }
                
                bool operator()(const Arg& arg) const
                {
                    return boost::apply_visitor(*this, arg);
                }
            };

            /**
             * Checks whether the given argument is an uninterpreted function instance.
             */
            struct IsUFInstance: public boost::static_visitor<bool> 
            {
                /**
                 * @param An uninterpreted variable.
                 * @return false
                 */
                bool operator()(const UVariable& /*unused*/) const
                {
                    return false;
                }
                
                /**
                 * @param An uninterpreted function instance.
                 * @return true
                 */
                bool operator()(const UFInstance& /*unused*/) const 
                {
                    return true;
                }
                
                bool operator()(const Arg& arg) const
                {
                    return boost::apply_visitor(*this, arg);
                }
            };

        private:

            // Member.

            /// A flag indicating whether this equality shall hold (if being false) or its negation (if being true), hence the inequality, shall hold.
            bool mNegated = false;
            /// The left-hand side of this uninterpreted equality.
            Arg mLhs = UVariable(Variable::NO_VARIABLE, Sort());
            /// The right-hand side of this uninterpreted equality.
            Arg mRhs = UVariable(Variable::NO_VARIABLE, Sort());

        public:
            
            UEquality() = default;
            
	    /**
             * Constructs an uninterpreted equality.
             * @param _negated true, if the negation of this equality shall hold, which means that it is actually an inequality.
             * @param _uvarA An uninterpreted variable, which is going to be the left-hand side of this uninterpreted equality.
             * @param _uvarB An uninterpreted variable, which is going to be the right-hand side of this uninterpreted equality.
             */
            UEquality(Arg _uvarA, Arg _uvarB, bool _negated):
                mNegated(_negated),
				mLhs(std::move(_uvarA)),
				mRhs(std::move(_uvarB))
			{
				if( lhsIsUV() && rhsIsUV() )
                {
                    assert( lhsAsUV().domain() == rhsAsUV().domain() );
                    if( rhsAsUV() < lhsAsUV() )
                        std::swap( mLhs, mRhs );
                }
				else if( lhsIsUV() && rhsIsUF() )
                {
                    assert( lhsAsUV().domain() == rhsAsUF().uninterpretedFunction().codomain() );
                }
                else if( lhsIsUF() && rhsIsUV() )
                {
                    assert( lhsAsUF().uninterpretedFunction().codomain() == rhsAsUV().domain() );
                    std::swap( mLhs, mRhs );
                }
                else if( lhsIsUF() && rhsIsUF() )
                {
                    assert( lhsAsUF().uninterpretedFunction().codomain() == rhsAsUF().uninterpretedFunction().codomain() );
                    if( rhsAsUF() < lhsAsUF() )
                        std::swap( mLhs, mRhs );
                }
                
			}

            /**
             * Constructs an uninterpreted equality.
             * @param _negated true, if the negation of this equality shall hold, which means that it is actually an inequality.
             * @param _uvarA An uninterpreted variable, which is going to be the left-hand side of this uninterpreted equality.
             * @param _uvarB An uninterpreted variable, which is going to be the right-hand side of this uninterpreted equality.
             */
            UEquality( const UVariable& _uvarA, const UVariable& _uvarB, bool _negated, bool _orderCorrect = false ):
                mNegated( _negated ),
                mLhs( _uvarA ),
                mRhs( _uvarB )
            {
                assert( _uvarA.domain() == _uvarB.domain() );
                if( !_orderCorrect && rhsAsUV() < lhsAsUV() )
                        std::swap( mLhs, mRhs );
            }
            
            /**
             * Constructs an uninterpreted equality.
             * @param _negated true, if the negation of this equality shall hold, which means that it is actually an inequality.
             * @param _uvar An uninterpreted variable, which is going to be the left-hand side of this uninterpreted equality.
             * @param _ufun An uninterpreted function instance, which is going to be the right-hand side of this uninterpreted equality.
             */
            UEquality( const UVariable& _uvar, const UFInstance& _ufun, bool _negated ):
                mNegated( _negated ),
                mLhs( _uvar ),
                mRhs( _ufun )
            {
                assert( _uvar.domain() == _ufun.uninterpretedFunction().codomain() );
            }
            
            /**
             * Constructs an uninterpreted equality.
             * @param _negated true, if the negation of this equality shall hold, which means that it is actually an inequality.
             * @param _ufun An uninterpreted function instance, which is going to be the left-hand side of this uninterpreted equality.
             * @param _uvar An uninterpreted variable, which is going to be the right-hand side of this uninterpreted equality.
             */
            UEquality( const UFInstance& _ufun, const UVariable& _uvar, bool _negated ):
                mNegated( _negated ),
                mLhs( _uvar ),
                mRhs( _ufun )
            {
                assert( _uvar.domain() == _ufun.uninterpretedFunction().codomain() );
            }
            
            /**
             * Constructs an uninterpreted equality.
             * @param _negated true, if the negation of this equality shall hold, which means that it is actually an inequality.
             * @param _ufunA An uninterpreted function instance, which is going to be the left-hand side of this uninterpreted equality.
             * @param _ufunB An uninterpreted function instance, which is going to be the right-hand side of this uninterpreted equality.
             */
            UEquality( const UFInstance& _ufunA, const UFInstance& _ufunB, bool _negated, bool _orderCorrect = false ):
                mNegated( _negated ),
                mLhs( _ufunA ),
                mRhs( _ufunB )
            {
                assert( _ufunA.uninterpretedFunction().codomain() == _ufunB.uninterpretedFunction().codomain() );
                if( !_orderCorrect && rhsAsUF() < lhsAsUF() )
                    std::swap( mLhs, mRhs );
            }
            
            /**
             * Copies the given uninterpreted equality.
             * @param _ueq The uninterpreted equality to copy.
             * @param _invert true, if the inverse of the given uninterpreted equality shall be constructed. (== -> != resp. != -> ==)
             */
            UEquality( const UEquality& _ueq, bool _invert = false ):
                mNegated( _invert ? !_ueq.mNegated : _ueq.mNegated ),
                mLhs( _ueq.mLhs ),
                mRhs( _ueq.mRhs )
            {}
            
            /**
             * @return true, if the negation of this equation shall hold, that is, it is actually an inequality.
             */
            bool negated() const
            {
                return mNegated;
            }
            
            /**
             * @return The left-hand side of this equality.
             */
            const Arg& lhs() const
            {
                return mLhs;
            }
            
            /**
             * @return The right-hand side of this equality.
             */
            const Arg& rhs() const
            {
                return mRhs;
            }

            /**
             * @return true, if the left-hand side is an uninterpreted variable.
             */
            bool lhsIsUV() const 
            {
                return boost::apply_visitor(IsUVariable(), mLhs);
            }

            /**
             * @return true, if the right-hand side is an uninterpreted variable.
             */
            bool rhsIsUV() const 
            {
                return boost::apply_visitor(IsUVariable(), mRhs);
            }

            /**
             * @return true, if the left-hand side is an uninterpreted function instance.
             */
            bool lhsIsUF() const 
            {
                return boost::apply_visitor(IsUFInstance(), mLhs);
            }

            /**
             * @return true, if the right-hand side is an uninterpreted function instance.
             */
            bool rhsIsUF() const 
            {
                return boost::apply_visitor(IsUFInstance(), mRhs);
            }

            /**
             * @return The left-hand side, which must be an uninterpreted variable.
             */
            const UVariable& lhsAsUV() const 
            {
                return boost::get<UVariable>(mLhs);
            }

            /**
             * @return The right-hand side, which must be an uninterpreted variable.
             */
            const UVariable& rhsAsUV() const 
            {
                return boost::get<UVariable>(mRhs);
            }

            /**
             * @return The left-hand side, which must be an uninterpreted function instance.
             */
            const UFInstance& lhsAsUF() const 
            {
                return boost::get<UFInstance>(mLhs);
            }

            /**
             * @return The right-hand side, which must be an uninterpreted function instance.
             */
            const UFInstance& rhsAsUF() const 
            {
                return boost::get<UFInstance>(mRhs);
            }
            
            /**
             * @return An approximation of the complexity of this uninterpreted equality.
             */
            std::size_t complexity() const;
			
			UEquality negation() const {
				return UEquality(lhs(), rhs(), !negated());
			}
            
            /**
             * @param _ueq The uninterpreted equality to compare with.
             * @return true, if this and the given equality instance are equal.
             */
            bool operator==( const UEquality& _ueq ) const;
            
            /**
             * @param _ueq The uninterpreted equality to compare with.
             * @return true, if this uninterpreted equality is less than the given one.
             */
            bool operator<( const UEquality& _ueq ) const;
            
            std::string toString( unsigned _unequalSwitch, bool _infix, bool _friendlyNames ) const;
            
            void collectUVariables( std::set<UVariable>& _uvars ) const;

            /**
             * Prints the given uninterpreted equality on the given output stream.
             * @param _os The output stream to print on.
             * @param _ueq The uninterpreted equality to print.
             * @return The output stream after printing the given uninterpreted equality on it.
             */
            friend std::ostream& operator<<( std::ostream& _os, const UEquality& _ueq );
    };
} // end namespace carl

namespace std
{
    /**
     * Implements std::hash for uninterpreted equalities.
     */
    template<>
    struct hash<carl::UEquality>
    {
    public:
        /**
         * @param _ueq The uninterpreted equality to get the hash for.
         * @return The hash of the given uninterpreted equality.
         */
        size_t operator()( const carl::UEquality& _ueq ) const 
        {
            size_t result = _ueq.lhsIsUV() ? std::hash<carl::UVariable>()( _ueq.lhsAsUV() ) : std::hash<carl::UFInstance>()( _ueq.lhsAsUF() );
            result ^= _ueq.rhsIsUV() ? std::hash<carl::UVariable>()( _ueq.rhsAsUV() ) : std::hash<carl::UFInstance>()( _ueq.rhsAsUF() );
            return result;
        }
    };
}
