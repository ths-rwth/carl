#include "OPBImporter.h"

#include "SpiritHelper.h"

#include <tuple>
#include <vector>

#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

namespace carl::io {
	namespace spirit = boost::spirit;
	namespace qi = boost::spirit::qi;
	namespace px = boost::phoenix;

	using BaseIteratorType = spirit::istream_iterator;
	using PositionIteratorType = spirit::line_pos_iterator<BaseIteratorType>;
	using Iterator = PositionIteratorType;
	using ErrorHandler = carl::io::helper::ErrorHandler;
	
	struct Skipper: public qi::grammar<Iterator> {
		Skipper(): Skipper::base_type(main, "skipper") {
			main = (qi::space | (qi::lit("*") > *(qi::char_ - qi::eol) > qi::eol));
		};
		qi::rule<Iterator> main;
	};
	
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
			mTerm = qi::int_ >> (qi::lexeme[mVariables >> !(qi::alnum | qi::char_("_"))] | mNewVarWrapper);
			mPolynomial = +mTerm;
			mConstraint = mPolynomial > mRelation > qi::int_;
			mObjective = -(qi::lit("min:") > mPolynomial > qi::lit(";"));
			mMain = (mObjective >> *(mConstraint > ";"))[qi::_val = px::bind(&OPBParser::createFile, px::ref(*this), qi::_1, qi::_2)];
			qi::on_error<qi::fail>(mMain, errorHandler(qi::_1, qi::_2, qi::_3, qi::_4));
		}
		std::optional<OPBFile> parse(std::istream& in) {
			in.unsetf(std::ios::skipws);
			Skipper skipper;
			BaseIteratorType basebegin(in);
			Iterator begin(basebegin);
			Iterator end;
			OPBFile res;
			if (qi::phrase_parse(begin, end, *this, skipper, res)) {
				if (begin != end) {
					std::cout << "Failed to parse:" << std::endl;
					std::cout << "\"" << std::string(begin, end) << "\"" << std::endl;
					return std::nullopt;
				}
				return res;
			} else {
				std::cout << "Failed to parse:" << std::endl;
				std::cout << "\"" << std::string(begin, end) << "\"" << std::endl;
				return std::nullopt;
			}
		}
		
	private:
		Variable addVariable(const std::string& s) {
			carl::Variable var = freshIntegerVariable(s);
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
		qi::rule<Iterator, OPBPolynomial(), Skipper> mObjective;
		qi::rule<Iterator, OPBFile(), Skipper> mMain;
		px::function<ErrorHandler> errorHandler;
	};

	std::optional<OPBFile> parseOPBFile(std::ifstream& in) {
		OPBParser parser;
		return parser.parse(in);
	}

}
