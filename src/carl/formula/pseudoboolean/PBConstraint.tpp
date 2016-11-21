namespace carl {

template<typename Rational, typename Poly>
void substituteIn(PBConstraint& c, const carl::Model<Rational, Poly>& m){
    std::vector<std::pair<Variable, int>> newLHS;
    for(auto it = c.getLHS().begin(); it != c.getLHS().end(); it++){
        auto element = m.find(it->first);
        if(element != m.end()){
            if(element->second.isBool()){
                if(element->second == true){
                    c.setRHS(c.getRHS() - it->second);
                }
                it = c.getLHS().erase(it);
            }
        }else{
            newLHS.push_back(std::make_pair(it->first, it->second));
        }
    }
    c.setLHS(newLHS);
}



template<typename Rational, typename Poly, typename Number>
void evaluate(carl::ModelValue<Rational,Poly>& res, PBConstraint& c, const
  /*  carl::Model<Rational,Poly>& m){
    substituteIn(c, m);
    //Call evaluate function from Constraint.h
    evaluate(Number(c.getRHS()), c.getRelation());
    */
}

}
