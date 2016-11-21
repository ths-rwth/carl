namespace carl {

  template<typename Rational, typename Poly>
void substituteIn(PBConstraint& c, const carl::Model<Rational, Poly>& m){
        for(auto i = c.lhs.begin(); i != c.lhs.end();){
            auto it = m.find(i->first);
            //Look if model contains this variable.
            if(it != m.end()){
                //Check if the variable is a bool.
                if(it->second.isBool()){
                    //If the variable is true, substract the factor from the rhs.
                    if(it->second == true){
                    c.rhs -= i->second;
                }
                //Remove the monomial form the constraint.
                i = c.lhs.erase(i);
                }else{
                    i++;
                }
            }

        }
}


    template<typename Rational, typename Poly, typename Number>
void evaluate(carl::ModelValue<Rational,Poly>& res, PBConstraint& c, const
    carl::Model<Rational,Poly>& m){
    substituteIn(c, m);
    //Call evaluate function from Constraint.h
    if(res.isBool()){
        res = evaluate(Number(c.rhs), c.relation);
    }
}

}
