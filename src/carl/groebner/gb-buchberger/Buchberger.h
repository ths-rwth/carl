/*
 * GiNaCRA - GiNaC Real Algebra package
 * Copyright (C) 2010-2012  Ulrich Loup, Joachim Redies, Sebastian Junges
 *
 * This file is part of GiNaCRA.
 *
 * GiNaCRA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GiNaCRA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GiNaCRA.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * @file:   Buchberger.h
 * @author: Sebastian Junges
 *
 */

#pragma once
//#define BUCHBERGER_STATISTICS

#include <list>
#include <unordered_map>

#include "../Ideal.h"
#include "../Reductor.h"
#include "../GBUpdateProcedures.h"
#include "CriticalPairs.h"

//#include "BuchbergerStats.h"

namespace carl
{


template<typename BuchbergerProc>
struct UpdateFnct : UpdateFnc
{
private:
	BuchbergerProc* procedure;
public:
	UpdateFnct(BuchbergerProc* proc) : procedure(proc) {}
	
	void operator()(size_t index)
	{
		procedure->update(index);
	}
};


/** 
 * Standard settings used if the Buchberger object is not instantiated with another template parameter.
 */
struct DefaultBuchbergerSettings
{
	static const bool calculateRealRadical = true;
};



/**
 * Gebauer and Moeller style implementation of the Buchberger algorithm. For more information about this Algorithm.
 * More information can be found in the Bachelor Thesis On Groebner Bases in SMT-Compliant Decision Procedures. 
 * 
 */
template<typename Polynomial, template<typename> class AddingPolicy>
class Buchberger : private AddingPolicy<Polynomial>
{

protected:
	Ideal<Polynomial>* pGb;
	std::vector<size_t> mGbElementsIndices;
    CritPairs mCritPairs;
	UpdateFnct<Buchberger<Polynomial, AddingPolicy>> mUpdateCallBack;
#ifdef BUCHBERGER_STATISTICS
	BuchbergerStats* mStats;
#endif


public:
	Buchberger() :
	mUpdateCallBack(this)
	{
		
	}
	
	void calculate(const std::list<Polynomial>& scheduledForAdding);
	void setIdeal(Ideal<Polynomial>* ideal)
	{
		pGb = ideal;
	}

	//std::list<std::pair<BitVector, BitVector> > reduceInput();

	void update(size_t index);
protected:
	
	bool addToGb(const Polynomial& newPol)
	{
		 LOGMSG_DEBUG("carl.gb.buchberger", "Add to gb: " << newPol);
		 return AddingPolicy<Polynomial>::addToGb( newPol, pGb, &mUpdateCallBack);
	}
	void removeBuchbergerTriples(std::unordered_map<size_t, SPolPair>& spairs, std::vector<size_t>& primelist);

	void reduce();
};

}

#include "Buchberger.tpp"
