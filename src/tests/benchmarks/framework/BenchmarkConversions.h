#include <cassert>
#include <map>

#include "../config.h"
#include "carl/converter/CArLConverter.h"
#include "carl/converter/GiNaCConverter.h"
#include "carl/converter/Z3Converter.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/util/tuple_util.h"

#include "Common.h"

namespace carl {
	
/**
 * Information needed for all of the convert methods below.
 */
struct ConversionInformation {
	std::map<carl::Variable, GiNaC::ex> ginacVariables;
	CArLConverter carl;
	GiNaCConverter ginac;
	Z3Converter z3;
};
typedef std::shared_ptr<ConversionInformation> CIPtr;

struct Conversion {
	template<typename Dst, typename Src>
	static Dst convert(const Src& s, const CIPtr& ci);
};

template<>
CMP<mpq_class> Conversion::convert<CMP<mpq_class>, CMP<cln::cl_RA>>(const CMP<cln::cl_RA>& p, const CIPtr& ci) {
	CMP<mpq_class> res;
	for (auto t: p) {
		res += Term<mpq_class>(ci->carl.toGMP(t->coeff()), t->monomial());
	}
	return res;
}
template<>
GMP Conversion::convert<GMP, CMP<cln::cl_RA>>(const CMP<cln::cl_RA>& m, const CIPtr& ci) {
	return ci->ginac(m);
}
template<>
GMP Conversion::convert<GMP, CMP<mpq_class>>(const CMP<mpq_class>& m, const CIPtr& ci) {
	return ci->ginac(m);
}
template<>
GVAR Conversion::convert<GiNaC::symbol, carl::Variable>(const carl::Variable& v, const CIPtr& ci) {
	return ci->ginac(v);
}
template<>
GMP Conversion::convert<GMP, CUMP<cln::cl_RA>>(const CUMP<cln::cl_RA>& m, const CIPtr& ci) {
	return ci->ginac(m);
}
template<>
ZMP Conversion::convert<ZMP, CMP<cln::cl_RA>>(const CMP<cln::cl_RA>& m, const CIPtr& ci) {
	return ci->z3(m);
}
template<>
ZMP Conversion::convert<ZMP, CMP<mpq_class>>(const CMP<mpq_class>& m, const CIPtr& ci) {
	return ci->z3(m);
}
template<>
ZMP Conversion::convert<ZMP, CUMP<cln::cl_RA>>(const CUMP<cln::cl_RA>& m, const CIPtr& ci) {
	return ci->z3(m);
}
template<>
ZVAR Conversion::convert<ZVAR, CVAR>(const CVAR& v, const CIPtr& ci) {
	return ci->z3(v);
}

template<>
unsigned Conversion::convert(const unsigned& n, const CIPtr&) {
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