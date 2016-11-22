#include <vector>
#include <utility>
#include "../../core/Relation.h"
#include "../model/Model.h"
#include "../Constraint.h"

namespace carl {
   
    class PBConstraint{
    private:
        Relation relation;
        int rhs;
        std::vector< std::pair<Variable, int> > lhs;
        
    public:
        PBConstraint();
        PBConstraint(const std::vector< std::pair<Variable, int> >& , Relation, int);
        std::vector<Variable> getVariables();
        PBConstraint negate();
        void setLHS(const std::vector< std::pair<Variable, int> >&);
        void setRelation(Relation);
        void setRHS(int);
        const std::vector<std::pair<Variable, int>>& getLHS() const;
        Relation getRelation() const;
        int getRHS() const;
        template<typename Rational, typename Poly>
        void substituteIn(const Model<Rational,Poly>& m);
        template<typename Rational, typename Poly>
        void evaluate(ModelValue<Rational,Poly>& res, const Model<Rational,Poly>& m);
    };
 
    PBConstraint::PBConstraint(){}
    
    PBConstraint::PBConstraint(const std::vector< std::pair<Variable, int> >& ls, Relation rel, int rs):
	relation(rel),
	rhs(rs),
    lhs(ls)
    {}
    
    std::vector<Variable> PBConstraint::getVariables(){
        std::vector<Variable> varVector;
        const std::vector<std::pair<Variable, int>>& ls = this->lhs;
        for(auto it = ls.begin(); it != ls.end(); ++it){
            varVector.push_back(it->first);
        }
        return varVector;
    }

    PBConstraint PBConstraint::negate(){
        Relation nRel = inverse(this->relation);
        PBConstraint negConst(this->lhs, nRel, this->rhs);
        return negConst;
    }

    const std::vector< std::pair<Variable, int>>& PBConstraint::getLHS() const {
        return this->lhs;
    }
    
    Relation PBConstraint::getRelation() const {
        return this->relation;
    }
    
    int PBConstraint::getRHS() const {
        return this->rhs;
    }

    void PBConstraint::setLHS(const std::vector< std::pair<Variable, int> >& ls){
        this->lhs = ls;
    }
    
    void PBConstraint::setRelation(Relation r){
        this->relation = r;
    }
    
    void PBConstraint::setRHS(int rs){
        this->rhs = rs;
    }
        
    std::ostream& operator<<(std::ostream& os, const PBConstraint& pbc) {
		return os << pbc.getLHS() << pbc.getRelation() << pbc.getRHS();
	}
}
#include "PBConstraint.tpp"
