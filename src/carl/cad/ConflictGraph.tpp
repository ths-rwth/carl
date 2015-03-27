/**
 * @file ConflictGraph.tpp
 * @ingroup cad
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * Contains the implementation of all templated methods of the ConflictGraph class.
 */

#include "ConflictGraph.h"


#pragma once

namespace carl {
namespace cad {

template<typename Number>
template<class InputIterator>
void ConflictGraph<Number>::addEdges(const InputIterator& first, const InputIterator& last)
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

template<typename Number>
ConflictGraph<Number>::ConflictGraph() :
		data(),
		mInverted(false),
		mDegrees(),
		mSamplePointVertexCount(0)
{
}

template<typename Number>
ConflictGraph<Number>::ConflictGraph(const ConflictGraph& g) :
		data(g.data),
		mInverted(g.mInverted),
		mDegrees(g.mDegrees),
		mSamplePointVertexCount(g.mSamplePointVertexCount),
		mConstraints(g.mConstraints),
		mData(g.mData),
		mSampleCount(g.mSampleCount)
{
}

template<typename Number>
ConflictGraph<Number>::ConflictGraph(unsigned int m) :
		data(m, AdjacencyArray()),
		mInverted(false),
		mDegrees(m, 0),
		mSamplePointVertexCount(0)
{
}

template<typename Number>
typename ConflictGraph<Number>::Vertex ConflictGraph<Number>::addConstraintVertex() {
	this->data.push_back(AdjacencyArray(mSamplePointVertexCount, ConflictType::UNSATISFIED));
	mDegrees.push_back(0);
	return this->data.size()-1;
}

template<typename Number>
void ConflictGraph<Number>::removeConstraintVertex(const Vertex& i) {
	assert(this->data.size() > i);
	assert(mDegrees.size() > i);
	this->data.erase(this->data.begin() + (long)i); // erase the i-th element
	mDegrees.erase(mDegrees.begin() + (long)i);
}

template<typename Number>
void ConflictGraph<Number>::clearSampleVertices() {
	for (auto& constraintVertex: this->data) {
		constraintVertex.clear();
	}
	mDegrees.assign(mDegrees.size(), 0);
	mSamplePointVertexCount = 0;
}

template<typename Number>
typename ConflictGraph<Number>::Vertex ConflictGraph<Number>::maxDegreeVertex() const {
	if (this->isInverted()) {
		return (Vertex)(std::min_element(mDegrees.begin(), mDegrees.end()) - mDegrees.begin());
	}
	return (Vertex)(std::max_element(mDegrees.begin(), mDegrees.end()) - mDegrees.begin());
}

template<typename Number>
unsigned long ConflictGraph<Number>::degree(const Vertex& i) const {
	assert(this->data.size() > i);
	if (this->isInverted()) {
		return this->data.at(i).size() - mDegrees.at(i);
	}
	return mDegrees.at(i);
}

template<typename Number>
void ConflictGraph<Number>::invertConflictingVertices(const Vertex& vertex) {
	std::forward_list<Vertex> toClear;
	for(Vertex j = 0; j < mSamplePointVertexCount; ++j)
	{ // visit all sample-point vertices
		if(this->data.at(vertex).at(j) == ConflictType::UNSATISFIED)
		{ // found non-zero position, so store it for later removal
			toClear.push_front(j);
		}
	}
	// sort the sample-point index list descendantly
	toClear.sort(std::greater<Vertex>());
	// switch all constraint-sample connections from UNSATISFIABLE to SATISFIABLE
	for (auto& j: toClear) {
		// complete column j in adjacency matrix
		for (Vertex i = 0; i < this->data.size(); ++i) {
			if (this->data.at(i).at(j) == ConflictType::UNSATISFIED) {
				// update degree for the constraint vertex i
				mDegrees.at(i)++;
				// add adjacency information
				this->data.at(i).at(j) = ConflictType::SATISFIED;
			}
		}
	}
}

template<typename Number>
void ConflictGraph<Number>::invertSatisfyingVertices(const Vertex& vertex) {
	std::forward_list<Vertex> toClear;
	for(Vertex j = 0; j < mSamplePointVertexCount; ++j)
	{ // visit all sample-point vertices
		if (this->data.at(vertex).at(j) == ConflictType::SATISFIED)
		{ // found non-zero position, so store it for later removal
			toClear.push_front(j);
		}
	}
	// sort the sample-point index list descendantly
	toClear.sort(std::greater<Vertex>());
	// switch all constraint-sample connections from UNSATISFIABLE to SATISFIABLE
	for (auto j: toClear) {
		// clear column *j in adjacency matrix
		for(Vertex i = 0; i < this->data.size(); ++i) {
			if (this->data.at(i).at(j) == ConflictType::SATISFIED) {
				// update degree for the constraint vertex i
				assert(mDegrees.at(i) > 0);
				mDegrees.at(i)--;
			}
			// clear adjacency information
			this->data.at(i).at(j) = ConflictType::UNSATISFIED;
		}
	}
}

template<typename Number>
std::ostream& operator<<(std::ostream& os, const ConflictGraph<Number>& g) {
	if (g.data.empty()) {
		os << "{}";
	} else {
		std::string invertedStr = g.isInverted() ? " (inverted)" : "";
		for (unsigned vertex = 0; vertex != g.data.size(); ++vertex) {
			os << vertex << " -> " << g.degree( vertex ) << " points adjacent" << invertedStr << std::endl;
		}
	}
	return os;
}

}
}
