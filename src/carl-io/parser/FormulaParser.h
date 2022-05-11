/**
 * @file FormulaParser.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

#pragma once

#include "Common.h"

namespace carl::io {
namespace parser {

template<typename Pol>
struct FormulaParser: public qi::grammar<Iterator, Formula<Pol>(), Skipper> {
	FormulaParser():
		FormulaParser<Pol>::base_type(main, "formula"),
		varmap(), binaryop(), naryop(),
		varname(), variable(), formula_op(), formula(), main()
	{
        binaryop.add("IMPLIES", FormulaType::IMPLIES);
        binaryop.add("iff", FormulaType::IFF);
        binaryop.add("xor", FormulaType::XOR);
        naryop.add("and", FormulaType::AND);
        naryop.add("or", FormulaType::OR);

		varname = qi::lexeme[ (qi::alpha | qi::char_("~!@$%^&*_+=<>.?/-")) > *(qi::alnum | qi::char_("~!@$%^&*_+=<>.?/-"))];
        varname.name("varname");
//        qi::debug(varname);

		variable = (varmap[qi::_val = qi::_1]) | (varname[qi::_val = px::bind(&FormulaParser<Pol>::newVariable, px::ref(*this), qi::_1)]);
        variable.name("variable");
//        qi::debug(variable);

        formula_op =
                ((qi::lit("not") | qi::lit("NOT")) > formula)[qi::_val = px::construct<Formula<Pol>>(FormulaType::NOT, qi::_1)]
            |   formula[qi::_a = qi::_1] >> (
                    (binaryop > formula)[qi::_val = px::bind(&FormulaParser::createBinary, px::ref(*this), qi::_1, qi::_a, qi::_2)]
                |   (+("AND" > formula))[qi::_val = px::bind(&FormulaParser::createNary, px::ref(*this), FormulaType::AND, qi::_a, qi::_1)]
                |   (+("OR" > formula))[qi::_val = px::bind(&FormulaParser::createNary, px::ref(*this), FormulaType::OR, qi::_a, qi::_1)]
                |   qi::eps[qi::_val = qi::_a]
            );
        formula_op.name("formula operation");
//        qi::debug(formula_op);

		formula = (variable [qi::_val = px::construct<Formula<Pol>>(qi::_1)])
                | ("(" >> formula_op > ")")[qi::_val = qi::_1]
                //| ("(" >> formula > ")")[qi::_val = qi::_1]
                ;
        formula.name("formula");
//        qi::debug(formula);

		main = formula;
        main.name("main");
//        qi::debug(main);
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

    Formula<Pol> createBinary(FormulaType op, const Formula<Pol>& lhs, const Formula<Pol>& rhs) {
        return Formula<Pol>(op, {lhs, rhs});
    }
    Formula<Pol> createNary(FormulaType op, const Formula<Pol>& first, const std::vector<Formula<Pol>>& ops) {
        assert(!ops.empty());
		switch (op) {
            case FormulaType::AND: {
                if( first.type() == FormulaType::FALSE )
                    return Formula<Pol>( FormulaType::FALSE );
                Formulas<Pol> subFormulas;
                subFormulas.push_back( first );
                for (const auto& subop: ops) {
                    if( subop.type() == FormulaType::FALSE )
                        return Formula<Pol>( FormulaType::FALSE );
                    subFormulas.push_back( subop );
                }
                return Formula<Pol>( FormulaType::AND, subFormulas );
            }
            case FormulaType::OR: {
                if( first.type() == FormulaType::TRUE )
                    return Formula<Pol>( FormulaType::TRUE );
                Formulas<Pol> subFormulas;
                subFormulas.push_back( first );
                for (const auto& subop: ops) {
                    if( subop.type() == FormulaType::TRUE )
                        return Formula<Pol>( FormulaType::TRUE );
                    subFormulas.push_back( subop );
                }
                return Formula<Pol>( FormulaType::OR, subFormulas );
            }
            default:
                assert(false);
        }
        return Formula<Pol>(FormulaType::FALSE);
    }

    Formula<Pol> mkAnd(const Formula<Pol>& first, const std::vector<Formula<Pol>>& ops) {
//        std::cout << __func__ << " of " << first << " and " << ops << std::endl;
        assert(!ops.empty());
		if( first.type() == FormulaType::FALSE )
            return Formula<Pol>( FormulaType::FALSE );
        Formulas<Pol> subFormulas;
        subFormulas.push_back( first );
		for (const auto& op: ops) {
			if( op.type() == FormulaType::FALSE )
                return Formula<Pol>( FormulaType::FALSE );
            subFormulas.push_back( op );
		}
//        std::cout << __func__ << ":" << __LINE__ << std::endl;
		return Formula<Pol>( FormulaType::AND, subFormulas );
	}

    Formula<Pol> mkOr(const Formula<Pol>& first, const std::vector<Formula<Pol>>& ops) {
//        std::cout << __func__ << " of " << first << " and " << ops << std::endl;
        assert(!ops.empty());
		if( first.type() == FormulaType::TRUE )
            return Formula<Pol>( FormulaType::TRUE );
        Formulas<Pol> subFormulas;
        subFormulas.push_back( first );
		for (const auto& op: ops) {
			if( op.type() == FormulaType::TRUE )
                return Formula<Pol>( FormulaType::TRUE );
            subFormulas.push_back( op );
		}
//        std::cout << __func__ << ":" << __LINE__ << std::endl;
		return Formula<Pol>( FormulaType::OR, subFormulas );
	}

	qi::symbols<char, Variable> varmap;
    qi::symbols<char, FormulaType> binaryop;
    qi::symbols<char, FormulaType> naryop;
    qi::rule<Iterator, std::string(), Skipper> varname;
	qi::rule<Iterator, Variable(), Skipper> variable;
	qi::rule<Iterator, Formula<Pol>(), Skipper, qi::locals<Formula<Pol>>> formula_op;
	qi::rule<Iterator, Formula<Pol>(), Skipper> formula;
	qi::rule<Iterator, Formula<Pol>(), Skipper> main;
};


}
}
