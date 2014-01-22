/**
 * @file ConflictGraph.tpp
 * @ingroup cad
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#include "ConflictGraph.h"


#pragma once

namespace carl {
namespace cad {

template<class InputIterator>
void ConflictGraph::addEdges(const InputIterator& first, const InputIterator& last)
{
	Vertex jIndex = mSamplePointVertexCount++;
	// add new column to adjacency matrix
	for (unsigned v = 0; v < this->data.size(); ++v) {
		this->data.at(v).push_back(ConflictType::UNSATISFIED);
	}
	for (InputIterator i = first; i != last; ++i) {
		this->data.at(*i).at(jIndex) = ConflictType::SATISFIED;
		mDegrees.at(*i)++;
	}
}

}
}