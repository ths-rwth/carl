/* 
 * File:   ConflictGraph.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <iostream>
#include <vector>

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

class ConflictGraph {
public:
	/// type for constraint and sample-point index vertices in a conflict graph
	typedef long unsigned Vertex;
	/// type for the adjacency list of one constraint vertex
	typedef std::vector<ConflictType> AdjacencyArray;
	
private:
	std::vector<AdjacencyArray> data;
	
	/**
	 *  Flag that indicates whether the entries of the adjacency arrays are read inverted (true) or not.
	 */
	bool mInverted;
	
	/**
	 * Contains the degrees of all constraint vertices.
	 * The degree of a constraint vertex is the number of true entries in the adjacency matrix.
	 */
	std::vector<unsigned int> mDegrees;
	
	/**
	 * Current count of sample point vertices.
	 */
	long unsigned mSamplePointVertexCount;
	
public:
	/**
	 * Constructs an empty graph.
	 */
	ConflictGraph();
		
	/**
	 * Copy constructor.
	 */
	ConflictGraph(const ConflictGraph& g);
	
	/**
	 * Constructs the adjacency matrix of this conflict graph.
	 * @param m initial number of vertices in U (number of constraints)
	 */
	ConflictGraph(unsigned int m);

	/**
	 * Re returns the number of vertices in this graph.
	 * @return Number of vertices.
	 */
	long unsigned size() const {
		return this->data.size();
	}

	/**
	 * Invert the conflict graph.
	 * Note that this operation does <b>not</b> change the current adjacency matrix, only the operations from now on are inverted.
	 *
	 * Is the graph inverted, all operations have the opposite outcome or effect.
	 */
	void invert() {
	   mInverted = !mInverted;
	}
	
	/**
	 * Give the inverted status of the conflict graph.
	 * Is the graph inverted, all operations have the opposite outcome or effect.
	 * @return
	 */
	bool isInverted() const {
	   return mInverted;
	}
	
	/**
	 * Add edges between the constraint indices from [first, last[ and a newly generated sample point index,
	 * indicating that the corresponding constraints are satisfied by some sample point.
	 *
	 * Note that this method does not regard whether the graph is inverted as the inverted flag only concerns reading operations.
	 * 
	 * @param first constraint node to be connected Make sure that i does not exceed the maximum index of the constraint nodes.
	 *      Otherwise you have to use addConstraintVertex in advance.
	 * @param last constraint node to be connected Make sure that i does not exceed the maximum index of the constraint nodes.
	 *      Otherwise you have to use addConstraintVertex in advance.
	 * @complexity amortized linear in the number of constraints plus the constraints added
	 */
	template<class InputIterator>
	void addEdges(const InputIterator& first, const InputIterator& last);
	
	/**
	 * Add a new constraint vertex by enlarging the respective index.
	 * The new constraint receives the index returned.
	 * @return index of the new constraint
	 * @complexity constant
	 */
	Vertex addConstraintVertex();
	
	/**
	 * Remove the specified constraint vertex by removing the respective index.
	 * All other constraint indices are decreased by one.
	 * @param i constraint vertex index
	 * @complexity constant
	 */
	void removeConstraintVertex(const Vertex& i);
	
	/**
	 * Remove all sample point vertices while keeping the constraint vertices, in particular, remove all edges.
	 * @complexity linear in the number of constraints
	 */
	void clearSampleVertices();
	
	/**
	  * Gives the vertex adjacent to the maximum number of all constraint-representing vertices present in this graph.
	 * The degree of a constraint vertex is the number of true entries in the adjacency matrix.
	 *
	 * Is the graph inverted, all reading operations have the opposite outcome, i.e., maxDegreeVertex has the oposite outcome as well.
	 *
	 * @return the vertex with the highest degree
	 * @complexity linear in the number of constraint vertices present
	 */
	Vertex maxDegreeVertex() const;
	
	/**
	 * Give the degree of the specified constraint vertex, i.e. the number of true entries in the adjacency matrix.
	 *
	 * Is the graph inverted, all reading operations have the opposite outcome, i.e., degree has the inverted outcome as well.
	 *
	 * @param i
	 * @return degree of vertex
	 * @complexity constant
	 */
	unsigned long degree(const Vertex& i) const;
	
	/**
	 * Turns any sample point vertex whose corresponding sample point <b>not</b> satisfies the given constraint vertex' constraint into a satisfying vertex.
	 *
	 * This is a destructive method!
	 * @param vertex constraint vertex to be cleared
	 * @complexity number of constraint vertices times the number of sample point vertices
	 */
	void invertConflictingVertices(const Vertex& vertex);
	
	/**
	 * Turns any sample point vertex whose corresponding sample point satisfies the given constraint vertex' constraint into a non-satisfying one.
	 *
	 * This is a destructive method!
	 * @param vertex constraint vertex to be cleared
	 * @complexity number of constraint vertices times the number of sample point vertices
	 */
	void invertSatisfyingVertices(const Vertex& vertex);
	
	/**
	 * Output the graph in incidence list representation to output stream os.
	 * @param os output stream
	 * @param g the conflict graph
	 * @return output stream containing the graph representation
	 */
	friend std::ostream& operator<<(std::ostream& os, const ConflictGraph& g);
};

}
}

#include "ConflictGraph.tpp"
