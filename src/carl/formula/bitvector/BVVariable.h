/**
 * @file UVariable.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "../../core/Variable.h"
#include "../../core/VariablePool.h"

namespace carl
{
    class BVVariable
    {
        private:
            Variable mVar;
            size_t mWidth;
            
        public:
            BVVariable(): mVar( Variable::NO_VARIABLE ) {}

            BVVariable( Variable::Arg _variable, size_t _width = 1 ):
                mVar( _variable ), mWidth( _width ) {}

            Variable operator()() const
            {
                return mVar;
            }
            
            size_t width() const
            {
                return mWidth;
            }

            /**
             * @return The string representation of this bit vector variable.
             */
            std::string toString( bool _friendlyNames ) const
            {
                return VariablePool::getInstance().getName(mVar, _friendlyNames);
            }

            /**
             * Prints the given bit vector variable on the given output stream.
             * @param _os The output stream to print on.
             * @param _bvvar The bit vector variable to print.
             * @return The output stream after printing the given bit vector variable on it.
             */
            friend std::ostream& operator<<( std::ostream& _os, const BVVariable& _bvvar )
            {
                return (_os << _bvvar());
            }
    };
} // end namespace carl
