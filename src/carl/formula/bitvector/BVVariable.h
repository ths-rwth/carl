/**
 * @file UVariable.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "../../core/Variable.h"
#include "../../core/VariablePool.h"
#include "../SortManager.h"

namespace carl {

	class BVVariable {
	private:
		Variable mVar;
		Sort mSort;
		std::size_t mWidth;

	public:

		BVVariable():
			mVar(Variable::NO_VARIABLE), mSort(), mWidth(0)
		{
		}

		BVVariable(Variable _variable, const Sort& _sort):
			mVar(_variable), mSort(_sort), mWidth(0)
		{
			assert(SortManager::getInstance().getType(_sort) == VariableType::VT_BITVECTOR);

			const std::vector<std::size_t>* indices = SortManager::getInstance().getIndices(_sort);
			assert(indices != nullptr && indices->size() == 1 && indices->front() > 0);

			mWidth = indices->front();
		}

		Variable variable() const {
			return mVar;
		}
		operator Variable() const {
			return mVar;
		}

        /**
        * @return The sort (domain) of this uninterpreted variable.
        */
        const Sort& sort() const {
            return mSort;
        }

		bool operator==(const BVVariable& _other) const {
			return mVar == _other.mVar;
		}
		bool operator<(const BVVariable& _other) const {
			return mVar < _other.mVar;
		}

		std::size_t width() const {
			return mWidth;
		}

		/**
		 * @return The string representation of this bit vector variable.
		 */
		std::string toString(bool _friendlyNames) const	{
			return VariablePool::getInstance().getName(mVar, _friendlyNames);
		}

		/**
		 * Prints the given bit vector variable on the given output stream.
		 * @param os The output stream to print on.
		 * @param v The bit vector variable to print.
		 * @return The output stream after printing the given bit vector variable on it.
		 */
		friend std::ostream& operator<<(std::ostream& os, const BVVariable& v) {
			return os << v.variable();
		}
	};
} // end namespace carl

namespace std {
    /**
     * Implements std::hash for bitvector variables.
     */
    template<>
    struct hash<carl::BVVariable> {
        /**
         * @param v The bitvector variable to get the hash for.
         * @return The hash of the given bitvector variable.
         */
        std::size_t operator()(const carl::BVVariable& v) const {
            return std::hash<carl::Variable>()(v);
        }
    };
}
