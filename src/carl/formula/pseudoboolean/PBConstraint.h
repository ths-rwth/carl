#include <list>
#include <utility>
#include "../../core/Relation.h"
#include "../model/Model.h"

class PBConstraint{
    
    
public:
    PBConstraint( std::list< std::pair<bool, int> >, carl::Relation, int);
    std::list<bool> getVariables();
    PBConstraint negateConst();
    
    template<typename Rational, typename Poly>
    void substituteIn(PBConstraint& c, const carl::Model<Rational,Poly>& m);
    
    template<typename Rational, typename Poly>
    void evaluate(carl::ModelValue<Rational,Poly>& res, PBConstraint& c, const
                  carl::Model<Rational,Poly>& m);
    
    
private:
    carl::Relation relation;
    int rhs;
    std::list< std::pair<bool, int> > lhs;

    

    
};


