/**
 * @file UFModel.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-24
 * @version 2014-10-24
 */

#pragma once

#include "SortValue.h"

#include "../../uninterpreted/UFManager.h"
#include "../../uninterpreted/UninterpretedFunction.h"

#include <iostream>
#include <map>
#include <utility>

namespace carl {

/**
 * Implements a sort value, being a value of the uninterpreted domain specified by this sort.
 */
class UFModel {
private:
	UninterpretedFunction mFunction;
	std::map<std::vector<SortValue>, SortValue> mValues;

public:
	explicit UFModel(const UninterpretedFunction& uf)
		: mFunction(uf) {}

	bool extend(const std::vector<SortValue>& _args, const SortValue& _value);

	SortValue get(const std::vector<SortValue>& _args) const;

	const auto& function() const {
		return mFunction;
	}
	const auto& values() const {
		return mValues;
	}

};

/**
 * @param _ufm The uninterpreted function model to compare with.
 * @return true, if this uninterpreted function model equals the given one.
 */
inline bool operator==(const UFModel& lhs, const UFModel& rhs) {
	return std::forward_as_tuple(lhs.function(), lhs.values())
		== std::forward_as_tuple(rhs.function(), rhs.values());
}
/**
 * @param _ufm The uninterpreted function model to compare with.
 * @return true, if this uninterpreted function model is less than the given one.
 */
inline bool operator<(const UFModel& lhs, const UFModel& rhs) {
	return std::forward_as_tuple(lhs.function(), lhs.values())
		<  std::forward_as_tuple(rhs.function(), rhs.values());
}

/**
 * Prints the given uninterpreted function model on the given output stream.
 * @param _os The output stream to print on.
 * @param _ufm The uninterpreted function model to print.
 * @return The output stream after printing the given uninterpreted function model on it.
 */
std::ostream& operator<<(std::ostream& os, const UFModel& ufm);

} // namespace carl

namespace std {
/**
 * Implements std::hash for uninterpreted function model.
 */
template<>
struct hash<carl::UFModel> {
	/**
	 * @param _ufm The uninterpreted function model to get the hash for.
	 * @return The hash of the given uninterpreted function model.
	 */
	std::size_t operator()(const carl::UFModel& ufm) const {
		std::size_t seed = 0;
		carl::hash_add(seed, ufm.function());
		for (const auto& v: ufm.values()) {
			carl::hash_add(seed, v.first);
			carl::hash_add(seed, v.second);
		}
		return seed;
	}
};
} // namespace std
