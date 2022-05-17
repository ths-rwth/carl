/** 
 * @file   GBUpdateProcedures.h
 * @ingroup gb
 * @author Sebastian Junges
 */

#pragma once

#include "../poly/umvpoly/functions/SeparablePart.h"

namespace carl
{

struct UpdateFnc {
	virtual void operator()(std::size_t index) = 0;
	virtual ~UpdateFnc() = default;
};


template<typename Polynomial>
struct StdAdding
{
public:
	virtual ~StdAdding() = default;
	
	bool addToGb(const Polynomial& p, std::shared_ptr<Ideal<Polynomial>> gb, UpdateFnc* update)
	{
		if(p.isConstant())
		{
			gb->clear();
			Polynomial q(1);
			q.setReasons(p.getReasons());
			gb->addGenerator(q);
			return true;
		}
		else
		{
			size_t index = gb->addGenerator(p);
			(*update)(index);
		}
		return false;
	}
};

template<typename Polynomial>
struct RadicalAwareAdding
{
	
};

template<typename Polynomial>
struct RealRadicalAwareAdding
{
private:
	
public:
	virtual ~RealRadicalAwareAdding() 
	{
		
	}
	
	bool addToGb(const Polynomial& p, std::shared_ptr<Ideal<Polynomial>> gb, UpdateFnc* update)
	{
		if(p.isConstant())
		{
			gb->clear();
			Polynomial q(1);
			q.setReasons(p.getReasons());
			gb->addGenerator(q);
			return true;
		}
		else if(p.nrTerms() == 1)
		{
			assert(!p.isConstant());
			Polynomial q(carl::separable_part(*p.lmon()));
#ifdef BUCHBERGER_STATISTICS
			if(q.lterm().tdeg() != p.lterm().tdeg()) mStats->SingleTermSFP();
#endif
			q.setReasons(p.getReasons());
			size_t index = gb->addGenerator(q);
			(*update)(index);
		}
		else if(p.isTsos())
		{
			if(p.hasConstantTerm())
			{
#ifdef BUCHBERGER_STATISTICS
				if(p.nrOfTerms() > 1) mStats->TSQWithConstant();
#endif
				gb->clear();
				Polynomial q(1);
				q.setReasons(p.getReasons());
				gb->addGenerator(q);
				return true;
			}
			else
			{
#ifdef BUCHBERGER_STATISTICS
				mStats->TSQWithoutConstant();
#endif
				Polynomial remainder(p);
				while(!carl::is_zero(remainder))
				{
					Polynomial r1(carl::separable_part(*remainder.lmon()));
#ifdef BUCHBERGER_STATISTICS
					if(remainder.lterm().tdeg() != r1.lterm().tdeg()) mStats->SingleTermSFP();
#endif
					r1.setReasons(p.getReasons());
					remainder.stripLT();
					size_t index = gb->addGenerator(r1);
					(*update)(index);
				}
			}
		}
		else if(p.isReducibleIdentity())
		{
#ifdef BUCHBERGER_STATISTICS
			mStats->ReducibleIdentity();
#endif
			Polynomial r;
			CARL_LOG_NOTIMPLEMENTED();
			//Polynomial r(p.getReducibleIdentity());
			//r.setOrigins(p.getOrigins());
			size_t index = gb->addGenerator(p);
			(*update)(index);
		}
		else
		{
			size_t index = gb->addGenerator(p);
			(*update)(index);
		}
		return false;
	}
};
}
