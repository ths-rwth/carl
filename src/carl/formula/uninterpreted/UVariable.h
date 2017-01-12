/**
 * @file UVariable.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once

#include "../../core/VariablePool.h"
#include "../../util/Common.h"
#include "../Sort.h"
#include "../SortManager.h"

namespace carl
{
    /**
     * Implements an uninterpreted variable.
     */
    class UVariable
    {
        private:
            
            // Members.
            
            /// The according variable, hence, the actual content of this class.
            Variable mVar;
            /// The domain.
            Sort mDomain;
            
        public:
			/**
			 * Default constructor.
			 * The resulting object will not be a valid variable, but a dummy object.
             */
            UVariable():
				mVar(Variable::NO_VARIABLE),
				mDomain()
			{
			}

			explicit UVariable(Variable var):
                mVar(var),
				mDomain(SortManager::getInstance().getInterpreted(var.getType()))
            {
			}
            
            /**
             * Constructs an uninterpreted variable.
             * @param _var The variable of the uninterpreted variable to construct.
             * @param _domain The domain of the uninterpreted variable to construct.
             */
            UVariable(Variable _var, Sort _domain):
                mVar(_var),
                mDomain(_domain)
            {}
            
            /**
             * @return The according variable, hence, the actual content of this class.
             */
            Variable operator()() const 
            {
                return mVar;
            }
            
            /**
             * @return The domain of this uninterpreted variable.
             */
            const Sort& domain() const
            {
                return mDomain;
            }
            
            /**
             * @param _uvar The uninterpreted variable to compare with.
             * @return true, if this and the given uninterpreted variable are equal.
             */
            bool operator==( const UVariable& _uvar ) const
            {
                return mVar == _uvar();
            }
            bool operator==(Variable::Arg var) const {
                return mVar == var;
            }
            
            /**
             * @param _uvar The uninterpreted variable to compare with.
             * @return true, if this uninterpreted variable is less than the given one.
             */
            bool operator<( const UVariable& _uvar ) const
            {
                return mVar < _uvar();
            }
            
            /**
             * @return The string representation of this uninterpreted variable.
             */
            std::string toString( bool _friendlyNames = true ) const
            {
				return VariablePool::getInstance().getName(mVar, _friendlyNames);
            }
            
            /**
             * Prints the given uninterpreted variable on the given output stream.
             * @param _os The output stream to print on.
             * @param _uvar The uninterpreted variable to print.
             * @return The output stream after printing the given uninterpreted variable on it.
             */
            friend std::ostream& operator<<( std::ostream& _os, const UVariable& _uvar )
            {
                return (_os << _uvar());
            }
    };
} // end namespace carl

namespace std
{
    /**
     * Implements std::hash for uninterpreted variables.
     */
    template<>
    struct hash<carl::UVariable>
    {
    public:
        /**
         * @param _uvar The uninterpreted variable to get the hash for.
         * @return The hash of the given uninterpreted variable.
         */
        size_t operator()( const carl::UVariable& _uvar ) const 
        {
            return hash<carl::Variable>()( _uvar() ) ;
        }
    };
}
