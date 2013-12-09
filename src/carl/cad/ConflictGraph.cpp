#include "ConflictGraph.h"

#include <algorithm>
#include <cassert>
#include <forward_list>

namespace carl {
namespace CAD {

ConflictGraph::ConflictGraph() :
		data(),
		mInverted(false),
		mDegrees(),
		mSamplePointVertexCount(0)
{
}

ConflictGraph::ConflictGraph(const ConflictGraph& g) :
		data(g.data),
		mInverted(g.mInverted),
		mDegrees(g.mDegrees),
		mSamplePointVertexCount(g.mSamplePointVertexCount)
{
}

ConflictGraph::ConflictGraph(unsigned int m) :
		data(m, AdjacencyArray()),
		mInverted(false),
		mDegrees(m, 0),
		mSamplePointVertexCount(0)
{
}

ConflictGraph::Vertex ConflictGraph::addConstraintVertex() {
	this->data.push_back(AdjacencyArray(mSamplePointVertexCount, ConflictType::UNSATISFIED));
	mDegrees.push_back(0);
	return this->data.size()-1;
}

void ConflictGraph::removeConstraintVertex(const Vertex& i) {
	assert(this->data.size() > i);
	assert(mDegrees.size() > i);
	this->data.erase(this->data.begin() + (long)i); // erase the i-th element
	mDegrees.erase(mDegrees.begin() + (long)i);
}

void ConflictGraph::clearSampleVertices() {
	for (auto constraintVertex: this->data) {
		constraintVertex.clear();
	}
	mDegrees.assign(mDegrees.size(), 0);
	mSamplePointVertexCount = 0;
}

ConflictGraph::Vertex ConflictGraph::maxDegreeVertex() const {
	if (this->isInverted()) {
		return (Vertex)(std::min_element(mDegrees.begin(), mDegrees.end()) - mDegrees.begin());
	}
	return (Vertex)(std::max_element(mDegrees.begin(), mDegrees.end()) - mDegrees.begin());
}

unsigned long ConflictGraph::degree(const Vertex& i) const {
	assert(this->data.size() > i);
	if (this->isInverted()) {
		return this->data.at(i).size() - mDegrees.at(i);
	}
	return mDegrees.at(i);
}

void ConflictGraph::invertConflictingVertices(const Vertex& vertex) {
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
	for (auto j: toClear) {
		// complete column j in adjacency matrix
		for(Vertex i = 0; i < this->data.size(); ++i) {
			if(this->data.at(i).at(j) == ConflictType::UNSATISFIED) {
				// update degree for the constraint vertex i
				mDegrees.at(i)++;
				// add adjacency information
				this->data.at(i).at(j) = ConflictType::SATISFIED;
			}
		}
	}
}

void ConflictGraph::invertSatisfyingVertices(const Vertex& vertex) {
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

std::ostream& operator<<(std::ostream& os, const ConflictGraph& g) {
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