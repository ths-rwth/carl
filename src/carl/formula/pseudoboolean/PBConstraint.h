#pragma once

#include <vector>
#include <utility>
#include "../../core/Relation.h"
#include "../../core/Variable.h"
#include "../../util/hash.h"

namespace carl {
   
    class PBConstraint{
		friend std::ostream& operator<<(std::ostream& os, const PBConstraint& pbc);
    private:
        Relation relation;
        int rhs;
        std::vector<std::pair<Variable, int>> lhs;
        
    public:
        PBConstraint() {}
        PBConstraint(const std::vector<std::pair<Variable, int>>& ls, Relation rel, int rs):
			relation(rel),
			rhs(rs),
		    lhs(ls)
	    {}
        std::vector<Variable> gatherVariables() const {
	        std::vector<Variable> varVector;
	        const std::vector<std::pair<Variable, int>>& ls = this->lhs;
	        for(auto it = ls.begin(); it != ls.end(); ++it){
	            varVector.push_back(it->first);
	        }
	        return varVector;
	    }
        PBConstraint negate() {
	        Relation nRel = inverse(this->relation);
	        PBConstraint negConst(this->lhs, nRel, this->rhs);
	        return negConst;
		}
        void setLHS(const std::vector<std::pair<Variable, int>>& l) {
			lhs = l;
		}
        void setRelation(Relation r) {
			relation = r;
		}
        void setRHS(int r) {
			rhs = r;
		}
        const std::vector<std::pair<Variable, int>>& getLHS() const {
			return lhs;
		}
        Relation getRelation() const {
			return relation;
		}
        int getRHS() const {
			return rhs;
		}
		
		std::string toString(bool, bool, bool) const {
			std::stringstream ss;
			ss << *this;
			return ss.str();
		}

		bool isTrue() const {
			if(relation == Relation::GEQ && rhs <= 0 ){
				for(auto it = lhs.begin(); it != lhs.end(); it++){
					if(it->second < 0){
						return false;
					}
				}
				return true;
			}
			return false;
		}

		bool isFalse() const {
			if(relation == Relation::GEQ && rhs >= 1){
				for(auto it = lhs.begin(); it != lhs.end(); it++){
					if(it->second > 0){
						return false;
					}
				}
				return true;
			}
			return false;
		}
    };
        
    inline std::ostream& operator<<(std::ostream& os, const PBConstraint& pbc) {
		return os << pbc.getLHS() << pbc.getRelation() << pbc.getRHS();
	}
	
	inline bool operator==(const PBConstraint& lhs, const PBConstraint& rhs) {
		return lhs.getRelation() == rhs.getRelation() && 
			lhs.getRHS() == rhs.getRHS() && 
			lhs.getLHS() == rhs.getLHS();
	}
	inline bool operator<(const PBConstraint& lhs, const PBConstraint& rhs) {
		if (lhs.getRHS() != rhs.getRHS()) return (lhs.getRHS() < rhs.getRHS());
		if (lhs.getRelation() != rhs.getRelation()) return lhs.getRelation() < rhs.getRelation();
		return lhs.getLHS() < rhs.getLHS();
	}
}

namespace std {
	template<>
    struct hash<carl::PBConstraint> {
    public:
        /**
         * @param _pbc The constraint to get the hash for.
         * @return The hash of the given constraint.
         */
        std::size_t operator()(const carl::PBConstraint& _pbc) const {
            return carl::hash_all(_pbc.getRelation(), _pbc.getRHS());
        }
    };
}

#include "PBConstraint.tpp"
