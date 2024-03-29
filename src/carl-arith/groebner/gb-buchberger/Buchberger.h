

/**
 * @file   Buchberger.h
 * @ingroup gb
 * @author Sebastian Junges
 *
 *
 */

#pragma once
//#define BUCHBERGER_STATISTICS


#include "../GBUpdateProcedures.h"
#include "../Ideal.h"
#include "../Reductor.h"
#include "CriticalPairs.h"

#include <list>
#include <unordered_map>

namespace carl
{


/**
 * @ingroup gb 
 */
template<typename BuchbergerProc>
struct UpdateFnct : UpdateFnc
{
private:
	BuchbergerProc* procedure;
public:
	explicit UpdateFnct(BuchbergerProc* proc) : procedure(proc) {}
	~UpdateFnct() override = default;
	
	void operator()(std::size_t index) override
	{
		procedure->update(index);
	}
};


/** 
 * Standard settings used if the Buchberger object is not instantiated with another template parameter.
 * @ingroup gb
 */
struct DefaultBuchbergerSettings
{
	static const bool calculateRealRadical = true;
};



/**
 * Gebauer and Moeller style implementation of the Buchberger algorithm. For more information about this Algorithm.
 * More information can be found in the Bachelor Thesis On Groebner Bases in SMT-Compliant Decision Procedures. 
 * @ingroup gb
 */
template<typename Polynomial, template<typename> class AddingPolicy>
class Buchberger : private AddingPolicy<Polynomial>
{

protected:
	std::shared_ptr<Ideal<Polynomial>> pGb;
	std::vector<size_t> mGbElementsIndices;
    std::shared_ptr<CritPairs> pCritPairs;
	UpdateFnct<Buchberger<Polynomial, AddingPolicy>> mUpdateCallBack;
#ifdef BUCHBERGER_STATISTICS
	BuchbergerStats* mStats;
#endif


public:
	Buchberger():
		pGb(),
		mGbElementsIndices(),
	    pCritPairs(new CritPairs()),
		mUpdateCallBack(this)
	{
		
	}
	
	virtual ~Buchberger() = default;
	
	Buchberger(const Buchberger& rhs):
		pGb(new Ideal<Polynomial>(*rhs.pGb)),
		mGbElementsIndices(rhs.mGbElementsIndices),
		pCritPairs(new CritPairs(*rhs.pCritPairs)),
		mUpdateCallBack(this)
	{
	}
	
	void calculate(const std::list<Polynomial>& scheduledForAdding);
	void setIdeal(const std::shared_ptr<Ideal<Polynomial>>& ideal)
	{
		pGb = ideal;
	}
	void setCriticalPairs(const std::shared_ptr<CritPairs>& criticalPairs)
	{
		pCritPairs = criticalPairs;
	}

	//std::list<std::pair<BitVector, BitVector> > reduceInput();

	void update(size_t index);
protected:
	
	bool addToGb(const Polynomial& newPol)
	{
		 CARL_LOG_DEBUG("carl.gb.buchberger", "Add to gb: " << newPol);
		 return AddingPolicy<Polynomial>::addToGb( newPol, pGb, &mUpdateCallBack);
	}
	void removeBuchbergerTriples(std::unordered_map<size_t, SPolPair>& spairs, std::vector<size_t>& primelist);

	void reduce();
};

}

#include "Buchberger.tpp"
