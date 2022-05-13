#pragma once

#include <carl/core/Variable.h>
#include <carl/core/VariablePool.h>

#include "../sort/SortManager.h"

namespace carl {

	/**
	 * Represent a BitVector-Variable
	 */
	class BVVariable {
	private:
		Variable mVar = Variable::NO_VARIABLE;
		Sort mSort;
		std::size_t mWidth = 0;

	public:

		BVVariable() = default;

		BVVariable(Variable _variable, const Sort& _sort):
			mVar(_variable), mSort(_sort)
		{
			assert(SortManager::getInstance().getType(_sort) == VariableType::VT_BITVECTOR);

			const std::vector<std::size_t>* indices = SortManager::getInstance().getIndices(_sort);
			assert(indices != nullptr && indices->size() == 1 && indices->front() > 0);

			mWidth = indices->front();
		}

		Variable variable() const {
			return mVar;
		}
		explicit operator Variable() const {
			return mVar;
		}

        /**
        * @return The sort (domain) of this uninterpreted variable.
        */
        const Sort& sort() const {
            return mSort;
        }

		std::size_t width() const {
			return mWidth;
		}

		/**
		 * @return The string representation of this bit vector variable.
		 */
		std::string toString(bool _friendlyNames) const	{
			return VariablePool::getInstance().get_name(mVar, _friendlyNames);
		}

		/**
		 * Print the given bit vector variable on the given output stream.
		 * @param os The output stream to print on.
		 * @param v The bit vector variable to print.
		 * @return The output stream after printing the given bit vector variable on it.
		 */
		friend std::ostream& operator<<(std::ostream& os, const BVVariable& v) {
			return os << v.variable();
		}
	};

	inline bool operator==(const BVVariable& lhs, const BVVariable& rhs) {
		return lhs.variable() == rhs.variable();
	}
	inline bool operator==(const BVVariable& lhs, const Variable& rhs) {
		return lhs.variable() == rhs;
	}
	inline bool operator==(const Variable& lhs, const BVVariable& rhs) {
		return lhs == rhs.variable();
	}
	inline bool operator<(const BVVariable& lhs, const BVVariable& rhs) {
		return lhs.variable() < rhs.variable();
	}
	inline bool operator<(const BVVariable& lhs, const Variable& rhs) {
		return lhs.variable() < rhs;
	}
	inline bool operator<(const Variable& lhs, const BVVariable& rhs) {
		return lhs < rhs.variable();
	}
} // end namespace carl

namespace std {
    /**
     * Implement std::hash for bitvector variables.
     */
    template<>
    struct hash<carl::BVVariable> {
        /**
         * @param v The bitvector variable to get the hash for.
         * @return The hash of the given bitvector variable.
         */
        std::size_t operator()(const carl::BVVariable& v) const {
            return std::hash<carl::Variable>()(v.variable());
        }
    };
}
