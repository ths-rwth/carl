/** 
 * @file:   WriteTosmt2Stream.h
 * @author: Sebastian Junges
 *
 * @since April 4, 2014
 */

#pragma once

#include <set>
#include <iostream>
#include <sstream>
#include "../core/MultivariatePolynomial.h"
#include "../core/RationalFunction.h"
#include "../core/Variable.h"
#include "../formula/Constraint.h"

namespace carl
{
namespace io
{
	enum class smt2flag { CHECKSAT, UNSAT_CORE, MODEL };
	enum class smt2node { CLOSENODE, AND, OR };
	enum class smt2logic { QF_NRA, QF_LRA, QF_LIA, QF_NIA };
	
	typedef std::string SMT2AssertionName;

	struct NamedAssertion
	{
		NamedAssertion(const SMT2AssertionName& assertion_name)
		: name(assertion_name)
		{
			
		}
		
		SMT2AssertionName name;
		std::stringstream assertion;
	};
	
	class WriteTosmt2Stream
	{
		std::set<carl::Variable> mVars;
		unsigned level = 0;
		
		// Options.
		
		/// In the smt2 standard, constants are actually functions. Many solvers support also the
		/// declare-const keyword, which is more intuitive to read, but not standardconforming.
		bool mConstantsAsParameterlessFunctions = true;
		/// To support readability, line breaks will be automatically added.
		bool mAutoLineBreaks = false;

		smt2logic logic = smt2logic::QF_NRA;
		
		
		std::vector<std::unique_ptr<NamedAssertion>> mAssertions = {};
		std::vector<std::vector<smt2flag>> mCommands = {};
		
		static std::string tosmt2string(const carl::VariableType vt)
		{
			switch(vt)
			{
				case carl::VariableType::VT_REAL:
					return "Real";
				default:
					assert(false);
					return "???";
			}
		}
		
		static std::string tosmt2string(smt2flag f)
		{
			switch(f)
			{
				case smt2flag::CHECKSAT:
					return "check-sat";
				case smt2flag::UNSAT_CORE:
					return "get-unsat-core";
				case smt2flag::MODEL:
					return "get-model";
				default:
					assert(false);
			}
		}
		
		std::string tosmt2string(const carl::Variable& v)
		{
			if(mConstantsAsParameterlessFunctions)
			{
				return "(declare-fun " + v.name() + " () " + tosmt2string(v.type()) + ")";
			}
			else
			{		
				return "(declare-const " + v.name() + " " + tosmt2string(v.type()) + ")";
			}
			
		}
		
		
		static std::string tosmt2string(const smt2logic& l)
		{
			switch(l)
			{
				case smt2logic::QF_LIA:
					return "QF_LIA";
				case smt2logic::QF_LRA:
					return "QF_LRA";
				case smt2logic::QF_NIA:
					return "QF_NIA";
				case smt2logic::QF_NRA:
					return "QF_NRA";
			}
		}
		
	public:
		
		void setAutomaticLineBreaks(bool value)
		{
			mAutoLineBreaks = value;
		}
		
		WriteTosmt2Stream& operator<<(SMT2AssertionName assertionName)
		{
			mAssertions.emplace_back(new NamedAssertion(assertionName));
			mCommands.push_back({});
			return *this;
		}
		
		WriteTosmt2Stream& operator<<(smt2node n)
		{
			switch(n)
			{
				case smt2node::CLOSENODE:
				{
					if(level == 0) throw std::invalid_argument("Cannot close a node, no node open");
					--level;
					mAssertions.back()->assertion  << ")";
					break;
				}
				case smt2node::AND:
				{
					++level;
					mAssertions.back()->assertion  << "(and ";
					break;
				}
				case smt2node::OR:
				{
					++level;
					mAssertions.back()->assertion  << "(or " ;
					break;
				}
					
			}
			return *this;
		}
		
		template<typename C, typename O, typename P>
		WriteTosmt2Stream& operator<<(const Constraint<MultivariatePolynomial<C,O,P>> c)
		{
			c.lhs().gatherVariables(mVars);
			mAssertions.back()->assertion  << "(" << c.relation() << " " << c.lhs() << " 0)";
			if(mAutoLineBreaks) mAssertions.back()->assertion << "\n";
			return *this;
		}
		
		template<typename Pol>
		WriteTosmt2Stream& operator<<(const Constraint<RationalFunction<Pol>> c)
		{
			c.lhs().gatherVariables(mVars);
			if(c.lhs().denominator() == Pol(1))
			{
				mAssertions.back()->assertion << "(" << c.rel() << c.lhs().nominator() << " 0)";
			}
			else
			{
				mAssertions.back()->assertion << "(" << c.rel() << " (/ " << c.lhs().nominator() << " " << c.lhs().denominator() << ") 0)";
			}
			if(mAutoLineBreaks) mAssertions.back()->assertion  << "\n";
			return *this;
		}
		
		WriteTosmt2Stream& operator<<(smt2flag f)
		{
			mCommands.back().push_back(f);
			return *this;
		}
		
		friend std::ostream& operator<<(std::ostream& os, WriteTosmt2Stream& smt2stream)
		{
			os << "(set-logic " << tosmt2string(smt2stream.logic) << ")\n";
			for(carl::Variable v : smt2stream.mVars)
			{
				os << smt2stream.tosmt2string(v) << std::endl;
			}
			for(unsigned i = 0; i < smt2stream.mAssertions.size(); ++i)
			{
				auto& namedAssertion = smt2stream.mAssertions[i];
				os << "(assert (! " << namedAssertion->assertion.str() << " :named " << namedAssertion->name << "))\n";
				auto& commands = smt2stream.mCommands[i];
				for(auto command : commands)
				{
					os << "(" << tosmt2string(command) << ")\n" << std::endl;
				}
			}
			return os;
		}
	};
}
}
