
#include "PBConstraint.h"

namespace carl {

    PBConstraint::PBConstraint(const std::vector< std::pair<Variable, int> >&  ls, carl::Relation rel, int rs):
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

    PBConstraint PBConstraint::negateConst(){
        carl::Relation nRel = inverse(this->relation);
        PBConstraint negConst(this->lhs, nRel, this->rhs);
        return negConst;
    }


}


