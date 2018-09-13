#pragma once

#include "../../core/Relation.h"
#include "../../core/Variable.h"
#include "../../util/hash.h"

#include <utility>
#include <vector>

namespace carl {
   
	template<typename Pol>
    class PBConstraint{
		using Number = typename UnderlyingNumberType<Pol>::type;
		template<typename Poly>
		friend std::ostream& operator<<(std::ostream& os, const PBConstraint<Poly>& pbc);
    private:
        Relation relation = Relation::EQ;
        Number rhs;
        std::vector<std::pair<Number,Variable>> lhs;
    public:
        PBConstraint() = default;
        PBConstraint(std::vector<std::pair<Number,Variable>> ls, Relation rel, Number rs):
			relation(rel),
			rhs(rs),
			lhs(std::move(ls))
	    {
            normalize();
        }
		PBConstraint(std::vector<std::pair<Number,Variable>>&& ls, Relation rel, Number rs):
			relation(rel),
			rhs(rs),
		    lhs(std::move(ls))
	    {
            normalize();
        }
        std::vector<Variable> gatherVariables() const {
	        std::vector<Variable> varVector;
            for (const auto& ls: lhs) {
	            varVector.push_back(ls.second);
	        }
	        return varVector;
	    }
        PBConstraint negate() {
	        Relation nRel = inverse(this->relation);
	        PBConstraint negConst(this->lhs, nRel, this->rhs);
	        return negConst;
		}
        void setLHS(const std::vector<std::pair<Number, Variable>>& l) {
			lhs = l;
		}
        void setRelation(Relation r) {
			relation = r;
		}
        void setRHS(Number r) {
			rhs = r;
		}
        const std::vector<std::pair<Number, Variable>>& getLHS() const {
			return lhs;
		}
        Relation getRelation() const {
			return relation;
		}
        Number getRHS() const {
			return rhs;
		}
		bool hasVariable(Variable v) const {
			for (const auto& t: lhs) {
				if (t.second == v) return true;
			}
			return false;
		}
		const Number& getCoefficient(Variable v) const {
			for (const auto& t: lhs) {
				if (t.second == v) return t.first;
			}
			return constant_zero<Number>::get();
		}
		
		void collectVariables(Variables& vars) const {
			for (const auto& term: getLHS()) {
				vars.insert(term.second);
			}
		}
        
        void normalize() {
            for (auto it = lhs.begin(); it != lhs.end(); ) {
                if (it->first == 0) {
                    it = lhs.erase(it);
                } else {
                    it++;
                }
            }
            if(lhs.size() == 0 && rhs != 0){//fasle ==> 0 x = 1
            	lhs.push_back(std::pair<Number, Variable>(0, freshVariable(carl::VariableType::VT_BOOL)));
            	rhs = 1;
            	relation = Relation::EQ;
            }else if(lhs.size() == 0 && rhs == 0){//true 0 x = 0
				lhs.push_back(std::pair<Number, Variable>(0, freshVariable(carl::VariableType::VT_BOOL)));
            	rhs = 0;
            	relation = Relation::EQ;
            }
        }
		
		std::string toString(bool, bool, bool) const {
			std::stringstream ss;
			ss << *this;
			return ss.str();
		}

		bool isTrue() const {
			if (relation == Relation::GEQ && rhs <= 0){
				for (const auto& lh: lhs) {
					if(lh.first < 0) return false;
				}
				return true;
			}
			return false;
		}

		bool isFalse() const {
			if (relation == Relation::GEQ && rhs >= 1){
				for (const auto& lh: lhs) {
					if (lh.first > 0) return false;
				}
				return true;
			}
			return false;
		}
    };
    
	template<typename Pol>
    inline std::ostream& operator<<(std::ostream& os, const PBConstraint<Pol>& pbc) {
		return os << pbc.getLHS() << pbc.getRelation() << pbc.getRHS();
	}
	
	template<typename Pol>
	inline bool operator==(const PBConstraint<Pol>& lhs, const PBConstraint<Pol>& rhs) {
		return lhs.getRelation() == rhs.getRelation() && 
			lhs.getRHS() == rhs.getRHS() && 
			lhs.getLHS() == rhs.getLHS();
	}
	template<typename Pol>
	inline bool operator<(const PBConstraint<Pol>& lhs, const PBConstraint<Pol>& rhs) {
		if (lhs.getRHS() != rhs.getRHS()) return (lhs.getRHS() < rhs.getRHS());
		if (lhs.getRelation() != rhs.getRelation()) return lhs.getRelation() < rhs.getRelation();
		return lhs.getLHS() < rhs.getLHS();
	}
}

namespace std {
	template<typename Pol>
    struct hash<carl::PBConstraint<Pol>> {
    public:
        /**
         * @param _pbc The constraint to get the hash for.
         * @return The hash of the given constraint.
         */
        std::size_t operator()(const carl::PBConstraint<Pol>& _pbc) const {
			std::size_t seed = 0;
			for (const auto& l: _pbc.getLHS()) {
				carl::hash_add(seed, l.first, l.second);
			}
            return carl::hash_all(seed, _pbc.getRelation(), _pbc.getRHS());
        }
    };
}

#include "PBConstraint.tpp"
