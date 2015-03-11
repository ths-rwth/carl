/**
 * @file FormulaParser.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

#pragma once

#include "Common.h"

namespace carl {
namespace parser {

template<typename Pol>
struct FormulaParser: public qi::grammar<Iterator, Formula<Pol>(), Skipper> {
	FormulaParser(): FormulaParser<Pol>::base_type(main, "formula") {
        
		varname = qi::lexeme[ (qi::alpha | qi::char_("~!@$%^&*_+=<>.?/-")) > *(qi::alnum | qi::char_("~!@$%^&*_+=<>.?/-"))];
        
		variable = (varmap[qi::_val = qi::_1]) | (varname[qi::_val = px::bind(&FormulaParser<Pol>::newVariable, px::ref(*this), qi::_1)]);
        
        formula_op = ((qi::lit("not") | qi::lit("NOT")) > formula)[qi::_val = px::construct<Formula<Pol>>(FormulaType::NOT, qi::_1)]
                    | ((qi::lit("implies") | qi::lit("IMPLIES")) > formula > formula)[qi::_val = px::construct<Formula<Pol>>(carl::FormulaType::IMPLIES, qi::_1, qi::_2)]
                    | ((qi::lit("iff") | qi::lit("IFF")) > formula > formula)[qi::_val = px::construct<Formula<Pol>>(carl::FormulaType::IFF, qi::_1, qi::_2)]
                    | ((qi::lit("xor") | qi::lit("XOR")) > formula > formula)[qi::_val = px::construct<Formula<Pol>>(carl::FormulaType::XOR, qi::_1, qi::_2)]
                    | (formula >> +((qi::lit("and") | qi::lit("AND")) >> formula))[qi::_val = px::bind(&FormulaParser<Pol>::mkAnd, px::ref(*this), qi::_1, qi::_2)]
                    | (formula >> +((qi::lit("or") | qi::lit("OR")) >> formula))[qi::_val = px::bind(&FormulaParser<Pol>::mkOr, px::ref(*this), qi::_1, qi::_2)]
                ;
        formula_op.name("formula operation");
        
		formula = (variable [qi::_val = px::construct<Formula<Pol>>(qi::_1)])
                | ("(" >> formula_op >> ")")[qi::_val = qi::_1]
                ;
        formula.name("formula");
        
		main = formula;
	}
	
	void addVariable(Variable::Arg v) {
		varmap.add(VariablePool::getInstance().getName(v), v);
	}
	
private:
	
	Variable newVariable(const std::string& s) {
		Variable v = freshBooleanVariable(s);
		varmap.add(s, v);
		return v;
	}
    
    Formula<Pol> mkAnd(const Formula<Pol>& first, const std::vector<Formula<Pol>>& ops) {
        assert(!ops.empty());
		if( first.getType() == FormulaType::FALSE )
            return Formula<Pol>( FormulaType::FALSE );
        Formulas<Pol> subFormulas;
        subFormulas.insert( first );
		for (const auto& op: ops) {
			if( op.getType() == FormulaType::FALSE )
                return Formula<Pol>( FormulaType::FALSE );
            subFormulas.insert( op );
		}
		return Formula<Pol>( FormulaType::AND, subFormulas );
	}
    
    Formula<Pol> mkOr(const Formula<Pol>& first, const std::vector<Formula<Pol>>& ops) {
        assert(!ops.empty());
		if( first.getType() == FormulaType::TRUE )
            return Formula<Pol>( FormulaType::TRUE );
        Formulas<Pol> subFormulas;
        subFormulas.insert( first );
		for (const auto& op: ops) {
			if( op.getType() == FormulaType::TRUE )
                return Formula<Pol>( FormulaType::TRUE );
            subFormulas.insert( op );
		}
		return Formula<Pol>( FormulaType::OR, subFormulas );
	}
	
	qi::symbols<char, Variable> varmap;
	qi::rule<Iterator, std::string(), Skipper> varname;
	qi::rule<Iterator, Variable(), Skipper> variable;
	qi::rule<Iterator, Formula<Pol>(), Skipper> formula_op;
	qi::rule<Iterator, Formula<Pol>(), Skipper> formula;
	qi::rule<Iterator, Formula<Pol>(), Skipper> main;
};


}
}