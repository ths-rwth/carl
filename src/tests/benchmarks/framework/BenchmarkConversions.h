#include <cassert>
#include <map>

#include "../config.h"
#include <carl-arith/converter/CArLConverter.h>
#ifdef USE_COCOA
#include <carl-arith/poly/umvpoly/CoCoAAdaptor.h>
#endif
#ifdef USE_GINAC
#include <carl-arith/converter/GiNaCConverter.h>
#endif
#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>
#include <carl-common/util/tuple_util.h>

#include "Common.h"

namespace carl {
	
/**
 * Information needed for all of the convert methods below.
 */
struct ConversionInformation {
	CArLConverter carl;
	#ifdef USE_COCOA
	CoCoAAdaptor<CMP<mpq_class>> cocoa;
	#endif
    #ifdef USE_GINAC
	std::map<carl::Variable, GiNaC::ex> ginacVariables;
	GiNaCConverter ginac;
    #endif
	
	ConversionInformation(const std::vector<Variable>& vars)
	#ifdef USE_COCOA
	: cocoa(vars) 
	#endif
	{}
};
typedef std::shared_ptr<ConversionInformation> CIPtr;

struct Conversion {
	template<typename Dst, typename Src>
	static Dst convert(const Src& s, const CIPtr& ci);
};

template<>
inline bool Conversion::convert<bool,std::vector<bool>::reference>(const std::vector<bool>::reference& b, const CIPtr&) {
	return b;
}

#ifdef USE_CLN_NUMBERS
template<>
inline CMP<mpq_class> Conversion::convert<CMP<mpq_class>, CMP<cln::cl_RA>>(const CMP<cln::cl_RA>& p, const CIPtr& ci) {
	CMP<mpq_class> res;
	for (auto t: p) {
		res += Term<mpq_class>(ci->carl.toGMP(t.coeff()), t.monomial());
	}
	return res;
}
#endif


#ifdef USE_COCOA
//template<>
//inline CoMP Conversion::convert<CoMP, CMP<cln::cl_RA>>(const CMP<cln::cl_RA>& m, const CIPtr& ci) {
//	return ci->cocoa.convert(m);
//}
template<>
inline CoMP Conversion::convert<CoMP, CMP<mpq_class>>(const CMP<mpq_class>& m, const CIPtr& ci) {
	return ci->cocoa.convert(m);
}
//template<>
//inline CoVAR Conversion::convert<CoVAR, carl::Variable>(const carl::Variable& v, const CIPtr& ci) {
//	return ci->cocoa.convert(v);
//}
//template<>
//inline CoMP Conversion::convert<CoMP, CUMP<cln::cl_RA>>(const CUMP<cln::cl_RA>& m, const CIPtr& ci) {
//	return ci->cocoa.convert(m);
//}
#endif

#ifdef USE_GINAC
template<>
inline GMP Conversion::convert<GMP, CMP<cln::cl_RA>>(const CMP<cln::cl_RA>& m, const CIPtr& ci) {
	return ci->ginac(m);
}
template<>
inline GMP Conversion::convert<GMP, CMP<mpq_class>>(const CMP<mpq_class>& m, const CIPtr& ci) {
	return ci->ginac(m);
}
template<>
inline GVAR Conversion::convert<GVAR, carl::Variable>(const carl::Variable& v, const CIPtr& ci) {
	return ci->ginac(v);
}
template<>
inline GMP Conversion::convert<GMP, CUMP<mpq_class>>(const CUMP<mpq_class>& m, const CIPtr& ci) {
	return ci->ginac(m);
}
template<>
inline GMP Conversion::convert<GMP, CUMP<cln::cl_RA>>(const CUMP<cln::cl_RA>& m, const CIPtr& ci) {
	return ci->ginac(m);
}
#endif

template<>
inline unsigned Conversion::convert(const unsigned& n, const CIPtr&) {
	return n;
}

struct BaseConverter {
public:
	typedef void type;
protected:
	CIPtr ci;
public:
	BaseConverter(const CIPtr& ci): ci(ci) {}
	type operator()() {}
};

template<typename... Out>
struct TupleConverter: public BaseConverter {
public:
	typedef std::tuple<Out...> type;
private:
	tuple_convert<Conversion, CIPtr, Out...> convert;
public:
	TupleConverter(const CIPtr& ci): BaseConverter(ci), convert(ci) {}
	template<typename... In>
	type operator()(const std::tuple<In...>& t) {
		return convert(t);
	}
};

}
