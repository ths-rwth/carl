#pragma once

#include "../core/MonomialPool.h"
#include "../core/Term.h"
#include "../core/Variable.h"
#include "../util/Common.h"
#include "../util/TimingCollector.h"

#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <map>

#ifdef USE_COCOA

//#include "CoCoA/library.H"
#include <CoCoA/BigInt.H>
#include <CoCoA/BigRat.H>
#include <CoCoA/factorization.H>
#include <CoCoA/ring.H>
#include <CoCoA/RingQQ.H>
#include <CoCoA/SparsePolyIter.H>
#include <CoCoA/SparsePolyOps-RingElem.H>
#include <CoCoA/SparsePolyRing.H>

namespace carl {
/**
 * This namespace contains wrapper for all heavy CoCoALib methods.
 * They are implemented in a separate source file, attempting to reduce the 
 * amount of code included from CoCoALib.
 */
namespace cocoawrapper {
	/// Calls CoCoA::gcd(p,q).
	CoCoA::RingElem gcd(const CoCoA::RingElem& p, const CoCoA::RingElem& q);
	/// Calls CoCoA::factor(p).
	CoCoA::factorization<CoCoA::RingElem> factor(const CoCoA::RingElem& p);
	/// Calls CoCoA::ReducedGBasis(CoCoA::ideal(p)).
	std::vector<CoCoA::RingElem> ReducedGBasis(const std::vector<CoCoA::RingElem>& p);
	/// Calls CoCoA::SqFreeFactor(p).
	CoCoA::factorization<CoCoA::RingElem> SqFreeFactor(const CoCoA::RingElem& p);
}

template<typename Poly>
class CoCoAAdaptor {
private:
	std::map<Variable, CoCoA::RingElem> mSymbolThere;
	std::vector<Variable> mSymbolBack;
	CoCoA::ring mQ = CoCoA::RingQQ();
	CoCoA::SparsePolyRing mRing;
public:
	CoCoA::BigInt convert(const mpz_class& n) const {
		return CoCoA::BigIntFromMPZ(n.get_mpz_t());
	}
	mpz_class convert(const CoCoA::BigInt& n) const {
		return mpz_class(CoCoA::mpzref(n));
	}
	CoCoA::BigRat convert(const mpq_class& n) const {
		return CoCoA::BigRatFromMPQ(n.get_mpq_t());
	}
	mpq_class convert(const CoCoA::BigRat& n) const {
		return mpq_class(CoCoA::mpqref(n));
	}

	void convert(mpz_class& res, const CoCoA::RingElem& n) const {
		CoCoA::BigInt i;
		CoCoA::IsInteger(i, n);
		res = convert(i);
	}
	void convert(mpq_class& res, const CoCoA::RingElem& n) const {
		CoCoA::BigRat r;
		CoCoA::IsRational(r, n);
		res = convert(r);
	}

	static std::vector<Variable> collectVariables(const std::vector<Poly>& polys) {
		std::set<Variable> vars;
		for (const auto& p: polys) p.gatherVariables(vars);
		return std::vector<Variable>(vars.begin(), vars.end());
	}

	CoCoA::RingElem convert(const Poly& p) const {
		CoCoA::RingElem res(mRing);
		for (const auto& t: p) {
			if (!t.monomial()) {
				res += convert(t.coeff());
				continue;
			}
			std::vector<long> exponents(mSymbolBack.size());
			for (const auto& p: *t.monomial()) {
				auto it = mSymbolThere.find(p.first);
				assert(it != mSymbolThere.end());
				long indetIndex;
				if (CoCoA::IsIndet(indetIndex, it->second)) {
					exponents[std::size_t(indetIndex)] = long(p.second);
				} else {
					assert(false && "The symbol is not an inderminant.");
				}
			}
			res += CoCoA::monomial(mRing, convert(t.coeff()), exponents);
		}
		return res;
	}

	Poly convert(const CoCoA::RingElem& p) const {
		Poly res;
		for (CoCoA::SparsePolyIter i = CoCoA::BeginIter(p); !CoCoA::IsEnded(i); ++i) {
			typename Poly::CoeffType coeff;
			convert(coeff, CoCoA::coeff(i));
			if (CoCoA::IsOne(CoCoA::PP(i))) {
				res += coeff;
			} else {
				std::vector<long> exponents;
				CoCoA::exponents(exponents, CoCoA::PP(i));
				Monomial::Content monContent;
				std::size_t tdeg = 0;
				for (std::size_t i = 0; i < exponents.size(); ++i) {
					if (exponents[i] == 0) continue;
					monContent.emplace_back(mSymbolBack[i], exponents[i]);
					tdeg += std::size_t(exponents[i]);
				}
				res += typename Poly::TermType(std::move(coeff), createMonomial(std::move(monContent), tdeg));
			}
		}
		return res;
	}

	std::vector<CoCoA::RingElem> convert(const std::vector<Poly>& p) const {
		std::vector<CoCoA::RingElem> res;
		for (const auto& poly: p) res.emplace_back(convert(poly));
		return res;
	}
	std::vector<Poly> convert(const std::vector<CoCoA::RingElem>& p) const {
		std::vector<Poly> res;
		for (const auto& poly: p) res.emplace_back(convert(poly));
		return res;
	}

	const auto& variables() const {
		return mSymbolBack;
	}

public:
	explicit CoCoAAdaptor(const std::vector<Variable>& vars):
		mSymbolBack(vars),
		mRing(CoCoA::NewPolyRing(mQ, long(mSymbolBack.size())))
	{
		auto indets = CoCoA::indets(mRing);

		for (std::size_t i = 0; i < mSymbolBack.size(); ++i) {
			mSymbolThere.emplace(mSymbolBack[i], indets[i]);
		}
	}
	CoCoAAdaptor(const std::vector<Poly>& polys):
		CoCoAAdaptor(collectVariables(polys))
	{}
	CoCoAAdaptor(const std::initializer_list<Poly>& polys):
		CoCoAAdaptor(std::vector<Poly>(polys))
	{}

	void resetVariableOrdering(const std::vector<Variable>& ordering) {
		assert(ordering.size() == mSymbolBack.size());
		assert(ordering.size() == mSymbolThere.size());
		mSymbolBack = ordering;

		auto indets = CoCoA::indets(mRing);
		for (std::size_t i = 0; i < mSymbolBack.size(); ++i) {
			mSymbolThere[mSymbolBack[i]] = indets[i];
		}
	}
	
	Poly gcd(const Poly& p1, const Poly& p2) const {
		auto start = CARL_TIME_START();
		auto res = convert(cocoawrapper::gcd(convert(p1), convert(p2)));
		CARL_TIME_FINISH("cocoa.gcd", start);
		return res;
	}

	Poly makeCoprimeWith(const Poly& p1, const Poly& p2) const {
		CoCoA::RingElem res = convert(p1);
		return convert(res / cocoawrapper::gcd(res, convert(p2)));
	}

	/**
	 * Break down a polynomial into its irreducible factors together with
	 * their exponents/multiplicities.
	 * E.g. "x^3 + 4 x^2 + 5 x + 2" factorizes into "(x+1)^2 * (x+2)^1"
	 * where "(x+1)", "(x+2)" are the irreducible factors and "2" and "1" are
	 * their exponents.
	 *
	 * @param includeConstants One of those factors is a constant-polynomial
	 * (degree 0), which is included by default but can be left out by setting
	 * the flag 'includeConstantFlag' to false, e.g., for root computations.
	 *
	 * @return A map whose keys are the irreducible factors and whose values are
	 * the exponents.
	 */
	Factors<Poly> factorize(const Poly& p, bool includeConstants = true) const {
		auto start = CARL_TIME_START();
		auto finfo = cocoawrapper::factor(convert(p));
		Factors<Poly> res;
		if (includeConstants && !CoCoA::IsOne(finfo.myRemainingFactor())) {
			res.emplace(convert(finfo.myRemainingFactor()), 1);
		}
		for (std::size_t i = 0; i < finfo.myFactors().size(); ++i) {
			res.emplace(convert(finfo.myFactors()[i]), finfo.myMultiplicities()[i]);
		}
		CARL_TIME_FINISH("cocoa.factorize", start);
		return res;
	}

	/**
	 * Break down a polynomial into its unique, non-constant,irreducible factors
	 * without their exponents/multiplicities.
	 * E.g. "3*x^3 + 12*x^2 + 15*x + 6" has the unique, non-constant, irreducible
	 * factors "(x+1)", "(x+2)", and a constant factor "3" that is omited.
	 */
	std::vector<Poly> irreducibleFactorsOf(const Poly& p) const {
		std::vector<Poly> res;
		auto cocoaFactors = cocoawrapper::factor(convert(p)).myFactors();
		for (const auto& f: cocoaFactors) {
			res.emplace_back(convert(f));
		}
		return res;
	}

	Poly squareFreePart(const Poly& p) const {
		auto finfo = cocoawrapper::SqFreeFactor(convert(p));
		Poly res(1);
		for (const auto& f: finfo.myFactors()) {
			res *= convert(f);
		}
		return res;
	}

	auto GBasis(const std::vector<Poly>& p) const {
		auto start = CARL_TIME_START();
		auto res = convert(cocoawrapper::ReducedGBasis(convert(p)));
		CARL_TIME_FINISH("cocoa.gbasis", start);
		return res;
	}
};

} // namespace carl

#endif
