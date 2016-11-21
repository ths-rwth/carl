#include <vector>
#include <utility>
#include "../../core/Relation.h"
#include "../model/Model.h"
#include "../Constraint.h"

namespace carl {
   

    class PBConstraint{
        
        
    public:
        PBConstraint(const std::vector< std::pair<Variable, int> >& , carl::Relation, int);
        std::vector<Variable> getVariables();
        PBConstraint negate();

        template<typename Rational, typename Poly>
        void substituteIn(PBConstraint& c, const carl::Model<Rational,Poly>& m);
      
        template<typename Rational, typename Poly, typename Number>
        void evaluate(carl::ModelValue<Rational,Poly>& res, PBConstraint& c, const
                      carl::Model<Rational,Poly>& m);

        carl::Relation relation;
        int rhs;
        std::vector< std::pair<Variable, int> > lhs;
        
    };
 
    
    PBConstraint::PBConstraint(const std::vector< std::pair<Variable, int> >& ls, carl::Relation rel, int rs):
    lhs(ls),
    relation(rel),
    rhs(rs)
    {}
    
    
    std::vector<Variable> PBConstraint::getVariables(){
        std::vector<Variable> varVector;
        for(auto i = this->lhs.begin(); i != this->lhs.end(); ++i){
            varVector.push_back(i->first);
        }
        return varVector;
    }
    
    PBConstraint PBConstraint::negate(){
        carl::Relation nRel = inverse(this->relation);
        PBConstraint negConst(this->lhs, nRel, this->rhs);
        return negConst;
    }
    

    
}
#include "PBConstraint.tpp"
