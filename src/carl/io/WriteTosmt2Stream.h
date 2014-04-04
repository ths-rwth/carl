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
#include "../core/Variable.h"
#include "../core/Constraint.h"
#include "../core/MultivariatePolynomial.h"

namespace carl
{
namespace io
{
	enum class smt2flag { CHECKSAT, ASSERT };
	enum class smt2node { CLOSENODE, AND, OR };
	enum class smt2logic { QF_NRA, QF_LRA, QF_LIA, QF_NIA };
	
	class WriteTosmt2Stream
	{
		std::set<carl::Variable> mVars;
		std::stringstream mProblem;
		unsigned level = 0;
		bool commandOpened = false;
		smt2logic logic = smt2logic::QF_NRA;
		
		static std::string tosmt2string(const carl::VariableType vt)
		{
			switch(vt)
			{
				case carl::VariableType::VT_REAL:
					return "Real";
				default:
					assert(false);
			}
		}
		
		static std::string tosmt2string(smt2flag f)
		{
			switch(f)
			{
				case smt2flag::CHECKSAT:
					return "check-sat";
				case smt2flag::ASSERT:
					return "assert";
				default:
					assert(false);
			}
		}
		
		static std::string tosmt2string(const carl::Variable& v)
		{
			return "(declare-const " + varToString(v, true) + " " + tosmt2string(v.getType()) + ")";
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
		
		WriteTosmt2Stream& operator<<(smt2node n)
		{
			switch(n)
			{
				case smt2node::CLOSENODE:
				{
					if(level == 0) throw std::invalid_argument("Cannot close a node, no node open");
					--level;
					mProblem << ")";
					break;
				}
				case smt2node::AND:
				{
					++level;
					mProblem << "(and ";
					break;
				}
				case smt2node::OR:
				{
					++level;
					mProblem << "(or " ;
					break;
				}
					
			}
			return *this;
		}
		
		template<typename C, typename O, typename P>
		WriteTosmt2Stream& operator<<(const Constraint<MultivariatePolynomial<C,O,P>> c)
		{
			c.lhs().gatherVariables(mVars);
			mProblem << "(" << c.rel() << " " << c.lhs().toString(false, true) << " 0)";
			return *this;
		}
		
		WriteTosmt2Stream& operator<<(smt2flag f)
		{
			if(commandOpened) mProblem << ")\n";
			mProblem << "(" << tosmt2string(f);
			commandOpened = true;
			return *this;
		}
		
		friend std::ostream& operator<<(std::ostream& os, WriteTosmt2Stream& smt2stream)
		{
			os << "(set-logic " << tosmt2string(smt2stream.logic) << ")\n";
			for(carl::Variable v : smt2stream.mVars)
			{
				os << tosmt2string(v) << std::endl;
			}
			os << smt2stream.mProblem.str();
			if(smt2stream.commandOpened) os << ")";
			return os;
		}
	};
}
}