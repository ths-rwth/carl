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
    class UVariable {
        private:
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
		          mDomain() {}

         explicit UVariable(Variable var):
              mVar(var),
	            mDomain(SortManager::getInstance().getInterpreted(var.type())) {}

        /**
         * Constructs an uninterpreted variable.
         * @param _var The variable of the uninterpreted variable to construct.
         * @param _domain The domain of the uninterpreted variable to construct.
         */
        UVariable(Variable var, Sort domain):
            mVar(var),
            mDomain(domain) {}

        /**
         * @return The according variable, hence, the actual content of this class.
         */
        Variable variable() const {
            return mVar;
        }

        /**
         * @return The domain of this uninterpreted variable.
         */
        const Sort& domain() const {
            return mDomain;
        }

        /**
         * @param uvar The uninterpreted variable to compare with.
         * @return true, if this and the given uninterpreted variable are equal.
         */
        bool operator==( const UVariable& uvar ) const {
            return mVar == uvar.variable();
        }
        bool operator==(Variable::Arg var) const {
            return mVar == var;
        }

        /**
         * @param uvar The uninterpreted variable to compare with.
         * @return true, if this uninterpreted variable is less than the given one.
         */
        bool operator<( const UVariable& uvar ) const {
            return mVar < uvar.variable();
        }

        /**
         * @return The string representation of this uninterpreted variable.
         */ 
        std::string toString(bool friendlyNames = true) const {
	          return VariablePool::getInstance().getName(mVar, friendlyNames);
        }

        /**
         * Prints the given uninterpreted variable on the given output stream.
         * @param os The output stream to print on.
         * @param uvar The uninterpreted variable to print.
         * @return The output stream after printing the given uninterpreted variable on it.
         */
        friend std::ostream& operator<<( std::ostream& os, const UVariable& uvar ) {
            return (os << uvar.variable());
        }
    };
} // end namespace carl

namespace std
{
    /**
     * Implements std::hash for uninterpreted variables.
     */
    template<>
    struct hash<carl::UVariable> {
      public:
          /**
           * @param uvar The uninterpreted variable to get the hash for.
           * @return The hash of the given uninterpreted variable.
           */
          size_t operator()( const carl::UVariable& uvar ) const
          {
              return carl::hash_all(uvar.variable());
          }
    };
}
