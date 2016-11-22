namespace carl {

template<typename Rational, typename Poly>
void PBConstraint::substituteIn(const Model<Rational, Poly>& m){
    std::vector<std::pair<Variable, int>> newLHS;
    for(auto it = getLHS().begin(); it != getLHS().end(); it++){
		auto element = m.find(it->first);
        if(element != m.end()) {
			assert(element->second.isBool());
            if(element->second.asBool() == true){
                setRHS(getRHS() - it->second);
            }
        } else {
            newLHS.push_back(*it);
        }
    }
    setLHS(newLHS);
}



template<typename Rational, typename Poly>
void PBConstraint::evaluate(carl::ModelValue<Rational,Poly>& res, const carl::Model<Rational,Poly>& m){
	PBConstraint tmp = *this;
    tmp.substituteIn(m);
    res = carl::evaluate(-tmp.getRHS(), tmp.getRelation());
}

}
