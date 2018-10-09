#include <cassert>
#include <map>

#include "../config.h"
#include "carl/converter/CArLConverter.h"
#ifdef USE_COCOA
#include "carl/converter/CoCoAAdaptor.h"
#endif
#ifdef USE_GINAC
#include "carl/converter/GiNaCConverter.h"
#endif
#ifdef COMPARE_WITH_Z3
#include "carl/converter/Z3Converter.h"
#endif
#include "carl/core/MultivariatePolynomial.h"
#include "carl/util/tuple_util.h"

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
    #ifdef COMPARE_WITH_Z3
	Z3Converter z3;
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
#ifdef USE_Z3_NUMBERS
template<>
inline CMP<rational> Conversion::convert<CMP<rational>, CMP<cln::cl_RA>>(const CMP<cln::cl_RA>& p, const CIPtr& ci) {
	CMP<rational> res;
	for (auto t: p) {
		res += Term<rational>(ci->carl.toZ3Rational(t.coeff()), t.monomial());
	}
	return res;
}
#endif
#endif

#ifdef USE_Z3_NUMBERS
template<>
inline CMP<mpq_class> Conversion::convert<CMP<mpq_class>, CMP<rational>>(const CMP<rational>& p, const CIPtr& ci) {
	CMP<mpq_class> res;
	for (auto t: p) {
		res += Term<mpq_class>(ci->carl.toGMP(t.coeff()), t.monomial());
	}
	return res;
}
template<>
inline CMP<rational> Conversion::convert<CMP<rational>, CMP<mpq_class>>(const CMP<mpq_class>& p, const CIPtr& ci) {
	CMP<rational> res;
	for (auto t: p) {
		res += Term<rational>(ci->carl.toZ3Rational(t.coeff()), t.monomial());
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
#ifdef USE_Z3_NUMBERS
#ifdef USE_CLN_NUMBERS
template<>
ZMP Conversion::convert<ZMP, CMP<cln::cl_RA>>(const CMP<cln::cl_RA>& m, const CIPtr& ci) {
	return ci->z3(m);
}
template<>
ZMP Conversion::convert<ZMP, CUMP<cln::cl_RA>>(const CUMP<cln::cl_RA>& m, const CIPtr& ci) {
	return ci->z3(m);
}
#endif

template<>
ZMP Conversion::convert<ZMP, CMP<mpq_class>>(const CMP<mpq_class>& m, const CIPtr& ci) {
	return ci->z3(m);
}
template<>
ZMP Conversion::convert<ZMP, CMP<rational>>(const CMP<rational>& m, const CIPtr& ci) {
	return ci->z3(m);
}
template<>
ZMP Conversion::convert<ZMP, CUMP<mpq_class>>(const CUMP<mpq_class>& m, const CIPtr& ci) {
	return ci->z3(m);
}
template<>
ZMP Conversion::convert<ZMP, CUMP<rational>>(const CUMP<rational>& m, const CIPtr& ci) {
	return ci->z3(m);
}
template<>
ZVAR Conversion::convert<ZVAR, CVAR>(const CVAR& v, const CIPtr& ci) {
	return ci->z3(v);
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
