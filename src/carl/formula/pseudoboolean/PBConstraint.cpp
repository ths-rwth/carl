#include "PBConstraint.h"


PBConstraint::PBConstraint( std::list< std::pair<bool, int> >  ls, carl::Relation rel, int rs){
 this->lhs = ls;
 this->relation = rel;
 this->rhs = rs;
 }

std::list<bool> PBConstraint::getVariables(){
    std::list<bool> varList;
    for(auto i = this->lhs.begin(); i != this->lhs.end(); ++i){
        varList.push_back(i->first);
    }
    return varList;
}

PBConstraint PBConstraint::negateConst(){
    carl::Relation nRel = inverse(this->relation);
    PBConstraint negConst(this->lhs, nRel, this->rhs);
    return negConst;
}

template<typename Rational, typename Poly>
void PBConstraint::substituteIn(PBConstraint& c, const carl::Model<Rational,Poly>& m){
    int newRHS = c.rhs;
    std::list< std::pair<bool, int> > constLHS = c.lhs;
    
    for(auto i = m.begin(); i != m.end(); ++i){
        if(i->second.asBool()){
            //Look for the variable i->first in the lhs.
            for(auto j = constLHS.begin(); j != constLHS.end(); ++j){
                if(j->first == i->first){
                    newRHS -= j->second;
                    constLHS.erase(j);
                }
            }
        }
        
    }
    
}


template<typename Rational, typename Poly>
void PBConstraint::evaluate(carl::ModelValue<Rational,Poly>& res, PBConstraint& c, const
                            carl::Model<Rational,Poly>& m){
    
//TODO
    
    
}

