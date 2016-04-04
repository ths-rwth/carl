/**
 * @file ConflictGraph.h
 * @ingroup cad
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * Contains the ConflictType and the declaration of the ConflictGraph class.
 */

#pragma once

#include <iostream>
#include <map>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "Constraint.h"

namespace carl {
namespace cad {

/**
 * Content of a ConflictGraph vertex.
 */
enum ConflictType
{
	/// indicates that the given constraint was satisfied by the given sample point
	SATISFIED = true,
	/// indicates that the given constraint was not satisfied by the given sample point	
	UNSATISFIED = false
};

/**
 * Representation of a bipartite graph (U+W, E) of vertices U, representing the constraints, stored as their unsigned index in a vector;
 * and W, representing the sample points, stored as a long index so that each index corresponds to exactly one sample point but a sample point
 * must not neccessarily refer to one index (indices over-approximate the sample points).
 *  
 * A vertex from U and a vertex from W are connected by an edge E iff the corresponding constraint is satisfied by the
 * corresponding sample point. This information is fixed, however, we can invert the reading behavior by using invert().
 *
 * There is no explicit storage of sample point information. Thus, the graph cannot be used for memoaization of satisfiability results.
 * 
 */

template<typename Number>
class ConflictGraph {
private:
	/// Maps constraints to IDs used in mData
	std::map<Constraint<Number>, std::size_t> mConstraints;
	/// Stores for each constraints, which sample points violate the constraint
	std::vector<boost::dynamic_bitset<>> mData;
	/// Stores the number of samples that have been registered
	std::size_t mSampleCount = 0;
public:

	/**
	 * Constructs an empty graph.
	 */
	ConflictGraph() {}
		
	/**
	 * Copy constructor.
	 */
	ConflictGraph(const ConflictGraph& g):
		mConstraints(g.mConstraints),
		mData(g.mData),
		mSampleCount(g.mSampleCount)
	{
		CARL_LOG_FUNC("carl.cad.cg", "Copied " << *this);
	}
	/**
	 * Returns the constraint ID for the given constraint.
	 */
	std::size_t getConstraint(const cad::Constraint<Number>& c) {
		auto it = mConstraints.find(c);
		if (it == mConstraints.end()) {
			it = mConstraints.insert(std::make_pair(c, mConstraints.size())).first;
		}
		CARL_LOG_TRACE("carl.cad.cg", c << " -> " << it->second);
		return it->second;
	}
	/**
	 * Returns the constraint for the given constraint ID.
	 */
	const cad::Constraint<Number>& getConstraint(std::size_t id) const {
		for (const auto& it: mConstraints) {
			if (it.second == id) return it.first;
		}
		assert(false);
		return mConstraints.begin()->first;
	}
	/**
	 * Registers a new sample point and returns its ID.
	 */
	std::size_t newSample() {
		return mSampleCount++;
	}
	void set(std::size_t constraint, std::size_t sample, bool value) {
		if (constraint >= mData.size()) {
			mData.resize(constraint+1);
		}
		if (sample >= mData[constraint].size()) {
			mData[constraint].resize(sample+1);
		}
		CARL_LOG_TRACE("carl.cad.cg", "Set " << constraint << " / " << sample << " to " << value);
		mData[constraint][sample] = value;
	}
	/**
	 * Retrieves the constraint that covers the most samples.
	 */
	std::size_t getMaxDegreeConstraint() const {
		assert(mData.size() > 0);
		std::size_t maxID = 0;
		std::size_t maxDegree = mData[0].count();
		for (std::size_t id = 1; id < mData.size(); id++) {
			std::size_t deg = mData[id].count();
			if (deg > maxDegree) {
				maxDegree = deg;
				maxID = id;
			}
		}
		return maxID;
	}
	/**
	 * Removes the given constraint and disable all sample points covered by this constraint.
	 */
	void selectConstraint(std::size_t id) {
		assert(mData.size() > id);
		// Store all samples point IDs
		std::vector<std::size_t> queue;
		queue.reserve(mData[id].count());
		for (std::size_t i = mData[id].find_first(); i != boost::dynamic_bitset<>::npos; i = mData[id].find_next(i)) {
			assert(mData[id][i]);
			queue.push_back(i);
		}
		// Remove this constraint
		mData[id].clear();
		// Disable sample points for other constraints
		for (auto& d: mData) {
			for (std::size_t i: queue) {
				if (i >= d.size()) continue;
				d[i] = false;
			}
		}
	}
	/**
	 * Checks if there are samples still uncovered.
	 */
	bool hasRemainingSamples() const {
		for (const auto& d: mData) {
			if (!d.none()) return true;
		}
		return false;
	}

	
	/**
	 * Remove the specified constraint vertex by removing the respective index.
	 * All other constraint indices are decreased by one.
	 * @param i constraint vertex index
	 * @complexity constant
	 */
	void removeConstraint(const cad::Constraint<Number>& c) {
		auto it = mConstraints.find(c);
		if (it == mConstraints.end()) return;
		CARL_LOG_FUNC("carl.cad.cg", c);
		std::size_t cid = it->second;
		mConstraints.erase(it);
		assert(mData.size() > cid);
		mData.erase(mData.begin() + (long)cid);
		
		for (auto& it: mConstraints) {
			if (it.second > cid) it.second--;
		}
	}
	
	template<typename T>
	friend std::ostream& operator<<(std::ostream& os, const ConflictGraph<T>& cg) {
		os << "Print CG with " << cg.mData.size() << " constraints" << std::endl;
		for (std::size_t i = 0; i < cg.mData.size(); i++) {
			os << cg.getConstraint(i) << ":" << std::endl;
			os << "\t" << cg.mData[i] << std::endl;
		}
		return os;
	}
};

}
}
