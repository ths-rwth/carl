#pragma once

#include "../core/MonomialPool.h"
#include "../core/Term.h"
#include "../core/Variable.h"
#include "../util/Common.h"

#include <map>

#ifdef USE_COCOA

#include "CoCoA/library.H"

namespace carl {

template<typename Poly>
class CoCoAAdaptor {
private:
	std::map<Variable, CoCoA::RingElem> mSymbolThere;
	std::vector<Variable> mSymbolBack;
	CoCoA::ring mQ = CoCoA::RingQQ();
	CoCoA::SparsePolyRing mRing;
	
	CoCoA::BigInt convert(const mpz_class& n) const {
		return CoCoA::BigInt(n.get_mpz_t());
	}
	mpz_class convert(const CoCoA::BigInt& n) const {
		return mpz_class(CoCoA::mpzref(n));
	}
	CoCoA::BigRat convert(const mpq_class& n) const {
		return CoCoA::BigRat(n.get_mpq_t());
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
				for (std::size_t i = 0; i < exponents.size(); i++) {
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
	explicit CoCoAAdaptor(const std::vector<Poly>& polys):
		mSymbolBack(collectVariables(polys)),
		mRing(CoCoA::NewPolyRing(mQ, long(mSymbolBack.size())))
	{
		auto indets = CoCoA::indets(mRing);

		for (std::size_t i = 0; i < mSymbolBack.size(); i++) {
			mSymbolThere.emplace(mSymbolBack[i], indets[i]);
		}
	}
	CoCoAAdaptor(const std::initializer_list<Poly>& polys):
		CoCoAAdaptor(std::vector<Poly>(polys))
	{}
	
	Poly gcd(const Poly& p1, const Poly& p2) const {
		return convert(CoCoA::gcd(convert(p1), convert(p2)));
	}
	Factors<Poly> factorize(const Poly& p, bool includeConstants = true) const {
		auto finfo = CoCoA::factor(convert(p));
		Factors<Poly> res;
		if (includeConstants && !CoCoA::IsOne(finfo.myRemainingFactor())) {
			res.emplace(convert(finfo.myRemainingFactor()), 1);
		}
		for (std::size_t i = 0; i < finfo.myFactors().size(); i++) {
			res.emplace(convert(finfo.myFactors()[i]), finfo.myMultiplicities()[i]);
		}
		return res;
	}
	Poly squareFreePart(const Poly& p) const {
		auto finfo = CoCoA::SqFreeFactor(convert(p));
		Poly res(1);
		for (const auto& f: finfo.myFactors()) {
			res *= convert(f);
		}
		return res;
	}
	auto GBasis(const std::vector<Poly>& p) const {
		return convert(CoCoA::GBasis(CoCoA::ideal(convert(p))));
	}
};

}

#endif
