#include "OPBImporter.h"

#include <tuple>
#include <vector>

#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace carl {
	namespace spirit = boost::spirit;
	namespace qi = boost::spirit::qi;
	namespace px = boost::phoenix;

	typedef boost::spirit::istream_iterator Iterator;
	typedef boost::spirit::qi::space_type Skipper;
	
	struct OPBParser: public qi::grammar<Iterator, OPBFile(), Skipper> {
		OPBParser(): OPBParser::base_type(mMain, "OPBFile") {
			mRelation.add
				("=", Relation::EQ)
				("!=", Relation::NEQ)
				("<", Relation::LESS)
				("<=", Relation::LEQ)
				(">", Relation::GREATER)
				(">=", Relation::GEQ)
			;
			mVarname = qi::lexeme[ qi::alpha > *(qi::alnum | qi::char_("_"))];
			mNewVarWrapper = mVarname[qi::_val = px::bind(&OPBParser::addVariable, px::ref(*this), qi::_1)];
			mTerm = qi::int_ >> ((mVariables >> qi::space) | mNewVarWrapper);
			mPolynomial = +mTerm;
			mConstraint = mPolynomial >> mRelation >> qi::int_;
			mMain = ("min:" >> mPolynomial >> ";" >> *(mConstraint >> ";"))[qi::_val = px::bind(&OPBParser::createFile, px::ref(*this), qi::_1, qi::_2)];
		}
		OPBFile parse(std::istream& in) {
			Skipper skipper;
			Iterator begin(in);
			Iterator end;
			OPBFile res;
			if (qi::phrase_parse(begin, end, *this, skipper, res)) {
				return res;
			} else {
				std::cout << "Remaining to parse:" << std::endl;
				std::cout << std::string(begin, end) << std::endl;
				return res;
			}
		}
		
	private:
		Variable addVariable(const std::string& s) {
			carl::Variable var = freshBooleanVariable(s);
			mVariables.add(s, var);
			return var;
		}
		OPBFile createFile(const OPBPolynomial& obj, const std::vector<OPBConstraint>& constraints) {
			return OPBFile(obj, constraints);
		}

		qi::symbols<char, Relation> mRelation;
		qi::symbols<char, Variable> mVariables;
		qi::rule<Iterator, std::string(), Skipper> mVarname;
		qi::rule<Iterator, Variable(), Skipper> mNewVarWrapper;
		qi::rule<Iterator, std::pair<int,Variable>(), Skipper> mTerm;
		qi::rule<Iterator, OPBPolynomial(), Skipper> mPolynomial;
		qi::rule<Iterator, OPBConstraint(), Skipper> mConstraint;
		qi::rule<Iterator, OPBFile(), Skipper> mMain;
	};

	OPBFile parseOPBFile(std::ifstream& in) {
		OPBParser parser;
		return parser.parse(in);
	}

}
