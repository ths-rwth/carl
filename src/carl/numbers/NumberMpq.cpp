#include "Number.h"
#include "NumberMpq.h"
#include "NumberClI.h"
#include "NumberClRA.h"
#include "NumberMpz.h"



namespace carl {

#ifdef USE_CLN_NUMBERS
		Number<mpq_class>::Number(const Number<cln::cl_RA>& n) : Number(n.toString()) {} 
		Number<mpq_class>::Number(const Number<cln::cl_I>& n) : Number(n.toString()) {} 
#endif


	//TODO: probably also add possibility to construct from fraction strings; is there a reason for this complicated method? instead of using mpq_class constructor??
	Number<mpq_class>::Number(const std::string& s) {
		std::vector<std::string> strs;
		boost::split(strs, s, boost::is_any_of("."));

		if(strs.size() > 2)
		{
		    throw std::invalid_argument("More than one delimiter in the string.");
		}
		mpq_class result;
		if(!strs.front().empty())
		{
		    result += mpq_class(strs.front());
		}
		if(strs.size() > 1)
		{
		    //if(strs.back().size() > )
		    result += (mpq_class(strs.back())/carl::pow(mpz_class(10),static_cast<unsigned>(strs.back().size())));
		}
		mData = result;
	}

	//constructs a/b:
	Number<mpq_class>::Number(const Number<mpz_class>& a,const Number<mpz_class>& b) { mData = mpq_class(a.getValue(),b.getValue()); }


	Number<mpq_class>::Number(const Number<mpz_class>& n) { mData = mpq_class(n.getValue()); }
	Number<mpq_class>::Number(const mpz_class& n) { mData = mpq_class(n); }



	





   

	//TODO: doesn't mpq_class have a standard "output" as well?
	std::string Number<mpq_class>::toString(bool _infix) const
	{
		std::stringstream s;
		bool negative = (mData < mpq_class(0));
		if(negative) s << "(-" << (_infix ? "" : " ");
		if(_infix) s << this->abs();
		else
		{
		    mpz_class d = mData.get_den();
		    if(constant_one<mpz_class>::get() != mData) {
			mpz_class abs1, abs2;
			mpz_abs(abs1.get_mpz_t(), mData.get_num().get_mpz_t());
			mpz_abs(abs2.get_mpz_t(), d.get_mpz_t());
			s << "(/ " << abs1 << " " << abs2 << ")";
		    } else {
			s << this->abs().mData;
			}
		}
		if(negative)
		    s << ")";
		return s.str();
	}


	 bool Number<mpq_class>::sqrt_exact(Number<mpq_class>& b) const
	    {
		if( mpq_sgn(mData.__get_mp()) < 0 ) return false;
		mpz_class den = mData.get_den();
		mpz_class num = mData.get_num();
		mpz_class root_den;
		mpz_class root_den_rem;
		mpz_sqrtrem(root_den.__get_mp(), root_den_rem.__get_mp(), den.__get_mp());
		if( !Number( root_den_rem ).isZero() )
		    return false;

		mpz_class root_num;
		mpz_class root_num_rem;
		mpz_sqrtrem(root_num.__get_mp(), root_num_rem.__get_mp(), num.__get_mp());
		if( !Number( root_num_rem ).isZero() )
		    return false;

		mpq_class resNum;
		mpq_set_z(resNum.get_mpq_t(), root_num.get_mpz_t());
		mpq_class resDen;
		mpq_set_z(resDen.get_mpq_t(), root_den.get_mpz_t());
		
		mpq_class fraction;
		mpq_div(fraction.get_mpq_t(), resNum.get_mpq_t(), resDen.get_mpq_t());

		b = Number<mpq_class>(fraction);
		return true;
	    } 

	    Number<mpq_class> Number<mpq_class>::sqrt() const {
		std::pair<Number<mpq_class>,Number<mpq_class>> r = this->sqrt_safe();
		return (r.first + r.second) / 2;
	    }

	    std::pair<Number<mpq_class>,Number<mpq_class>> Number<mpq_class>::sqrt_safe() const
	    {
		assert( mpq_sgn(mData.__get_mp()) > 0 );
		mpz_class den = mData.get_den();
		mpz_class num = mData.get_num();
		mpz_class root_den;
		mpz_class root_den_rem;
		mpz_sqrtrem(root_den.__get_mp(), root_den_rem.__get_mp(), den.__get_mp());

		mpz_class root_num;
		mpz_class root_num_rem;
		mpz_sqrtrem(root_num.__get_mp(), root_num_rem.__get_mp(), num.__get_mp());

		mpq_class lower;
		mpq_class upper;

		lower = root_num;
		if(root_den_rem == 0)
		    lower /= root_den;
		else
		    lower /= root_den+1;

		if(root_num_rem == 0)
		    upper = root_num;
		else
		    upper = root_num+1;

		upper /= root_den;

		return std::make_pair(Number(lower),Number(upper));
	    }

	    std::pair<Number<mpq_class>, Number<mpq_class>> Number<mpq_class>::sqrt_fast() const
	    {
		assert(mData >= 0);
	#if 1
		return sqrt_safe(); //NOTE: there was something (probably) equivalent to sqrt_safe() here, so this replacement should be ok
	#else
		mpq_class exact_root;
		if (carl::sqrtp(mData, exact_root)) {
		    // root can be computed exactly.
		    return std::make_pair(exact_root, exact_root);
		} else {
		    // compute an approximation with sqrt(). we can assume that the surrounding integers contain the actual root.
		    mpf_class af = sqrt(mpf_class(mData));
		    mpq_class lower(af - carl::constant_one<mpf_class>::get());
		    mpq_class upper(af + carl::constant_one<mpf_class>::get());
		    assert(lower * lower < mData);
		    assert(upper * upper > mData);
		    return std::make_pair(lower, upper);
		}
	#endif
	    }



}

