/** 
 * @file:   RealRadicalAwareAdding.h
 * @author: Sebastian Junges
 *
 * @since August 28, 2013
 */

#pragma once

namespace carl
{

struct UpdateFnc
{
	virtual void operator()(size_t index) = 0;
};


template<typename Polynomial>
struct StdAdding
{
private:
	
public:
	bool addToGb(const Polynomial& p, Ideal<Polynomial>* gb, UpdateFnc* update)
	{
		size_t index = gb->addGenerator(p);
		(*update)(index);
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
	bool addToGb(const Polynomial& p, Ideal<Polynomial>* gb, UpdateFnc* update)
	{
		
		if(!p.isConstant() && p.nrTerms() == 1)
		{
			Polynomial q(std::shared_ptr<Monomial>(p.lmon()->seperablePart()));
#ifdef BUCHBERGER_STATISTICS
			if(q.lterm().tdeg() != p.lterm().tdeg()) mStats->SingleTermSFP();
#endif
			//q.setOrigins(p.getOrigins());
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
				//q.setOrigins(p.getOrigins());
				gb->addGenerator(q);
				return true;
			}
			else
			{
#ifdef BUCHBERGER_STATISTICS
				mStats->TSQWithoutConstant();
#endif
				Polynomial remainder(p);
				while(!remainder.isZero())
				{
					Polynomial r1(std::shared_ptr<Monomial>(remainder.lmon()->seperablePart()));
#ifdef BUCHBERGER_STATISTICS
					if(remainder.lterm().tdeg() != r1.lterm().tdeg()) mStats->SingleTermSFP();
#endif
					//r1.setOrigins(p.getOrigins());
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
			LOG_NOTIMPLEMENTED();
			//Polynomial r(p.getReducibleIdentity());
			//r.setOrigins(p.getOrigins());
			size_t index = gb->addGenerator(r);
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