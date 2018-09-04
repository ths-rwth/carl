#pragma once

#include "../../formula/model/Model.h"
#include "../../formula/model/evaluation/ModelEvaluation.h"

#ifdef USE_COCOA

#include <CoCoA/library.H>

namespace carl {
namespace detail_lazard {
	
	struct CoCoAConverter {
		struct ConversionInfo {
			CoCoA::SparsePolyRing mRing;
			std::map<Variable, CoCoA::RingElem> mSymbolThere;
			std::map<std::pair<long,std::size_t>, Variable> mSymbolBack;
		};
		
		CoCoA::BigRat convert(const mpq_class& n) const {
			return CoCoA::BigRatFromMPQ(n.get_mpq_t());
		}
		mpq_class convert(const CoCoA::BigRat& n) const {
			return mpq_class(CoCoA::mpqref(n));
		}

		template<typename Poly>
		Poly convertMV(const CoCoA::RingElem& p, const ConversionInfo& ci) const {
			Poly res;
			for (CoCoA::SparsePolyIter i = CoCoA::BeginIter(p); !CoCoA::IsEnded(i); ++i) {
				Poly coeff;
				CoCoA::BigRat numcoeff;
				if (CoCoA::IsRational(numcoeff, CoCoA::coeff(i))) {
					coeff = Poly(convert(numcoeff));
				} else {
					coeff = convertMV<Poly>(CoCoA::CanonicalRepr(CoCoA::coeff(i)), ci);
				}
				if (CoCoA::IsOne(CoCoA::PP(i))) {
					res += coeff;
				} else {
					std::vector<long> exponents;
					CoCoA::exponents(exponents, CoCoA::PP(i));
					Monomial::Content monContent;
					std::size_t tdeg = 0;
					for (std::size_t i = 0; i < exponents.size(); ++i) {
						if (exponents[i] == 0) continue;
						const auto& ring = CoCoA::owner(p);
						monContent.emplace_back(ci.mSymbolBack.at(std::make_pair(CoCoA::RingID(ring),i)), exponents[i]);
						tdeg += std::size_t(exponents[i]);
					}
					res += coeff * createMonomial(std::move(monContent), tdeg);
				}
			}
			return res;
		}
		
		template<typename Poly>
		CoCoA::RingElem convertUV(const Poly& p, const ConversionInfo& ci) const {
			CoCoA::RingElem res(ci.mRing);
			CoCoA::RingElem exp(ci.mRing, 1);
			CoCoA::RingElem var = ci.mSymbolThere.at(p.mainVar());
			for (std::size_t deg = 0; deg <= p.degree(); ++deg) {
				res += convert(p.coefficients()[deg]) * exp;
				exp *= var;
			}
			return res;
		}
	};
}

template<typename Rational, typename Poly>
class LazardEvaluation {
private:
	CoCoA::ring mQ = CoCoA::RingQQ();
	Model<Rational, Poly> mModel;
	
	detail_lazard::CoCoAConverter cc;
	std::map<Variable, CoCoA::RingElem> mSymbolsThere;
	std::map<std::pair<long,std::size_t>, Variable> mSymbolsBack;
	
	Poly mLiftingPoly;
	
	auto buildPolyRing(Variable v, const RealAlgebraicNumber<Rational>& r) {
		CoCoA::SparsePolyRing ring = CoCoA::NewPolyRing(mQ, {CoCoA::NewSymbol()});
		mSymbolsThere.emplace(v, CoCoA::indets(ring)[0]);
		mSymbolsBack.emplace(std::make_pair(CoCoA::RingID(ring), 0), v);
		return detail_lazard::CoCoAConverter::ConversionInfo({
			ring, mSymbolsThere, mSymbolsBack
		});
	}
	
	bool evaluatesToZero(const CoCoA::RingElem& p, const detail_lazard::CoCoAConverter::ConversionInfo& ci) const {
		auto mp = cc.convertMV<Poly>(p, ci);
		auto res = carl::model::evaluate(mp, mModel);
		CARL_LOG_DEBUG("carl.lazard", "Evaluated " << p << " -> " << mp << " -> " << res);
		assert(res.isRational() || res.isRAN());
		if (res.isRational()) return carl::isZero(res.asRational());
		return carl::isZero(res.asRAN());
	}
	
	void extendRing(const CoCoA::ring& ring, const CoCoA::RingElem& p) {
		mQ = CoCoA::NewQuotientRing(ring, CoCoA::ideal(p));
	}
	
	/**
	 * Analyzes whether we have to construct a field extension.
	 * We may have one of two cases:
	 * - We can eliminate v by substitution with some term
	 * - We create a new field extension and may have to reduce the lifting polynomial
	 *
	 * In the first case, we return true and the term to substitute with.
	 * In the second case, we return false and the reduction polynomial.
	 */
	std::pair<bool,Poly> getSubstitution(Variable v, const RealAlgebraicNumber<Rational>& r) {
		if (r.isNumeric()) {
			CARL_LOG_DEBUG("carl.lazard", "Is numeric: " << v << " -> " << r);
			return std::make_pair(true, Poly(r.value()));
		}
		detail_lazard::CoCoAConverter::ConversionInfo ci = buildPolyRing(v, r);
		CoCoA::RingElem p = cc.convertUV(r.getIRPolynomial().replaceVariable(v), ci);
		auto factorization = CoCoA::factor(p);
		CARL_LOG_DEBUG("carl.lazard", "Factorization of " << p << " on " << ci.mRing << ": " << factorization);
		for (const auto& f: factorization.myFactors()) {
			if (evaluatesToZero(f, ci)) {
				CARL_LOG_DEBUG("carl.lazard", "Factor " << f << " is zero in assignment.");
				if (CoCoA::deg(f) == 1) {
					auto cf = -(f - CoCoA::LF(f));
					return std::make_pair(true, cc.convertMV<Poly>(cf, ci));
				} else {
					extendRing(ci.mRing, f);
					return std::make_pair(false, cc.convertMV<Poly>(f, ci));
				}
			}
		}
		assert(false);
		return std::make_pair(false, Poly());
	}
	
public:
	LazardEvaluation(const Poly& p): mLiftingPoly(p) {}
	
	void substitute(Variable v, const RealAlgebraicNumber<Rational>& r) {
		mModel.emplace(v, r);
		auto red = getSubstitution(v, r);
		Poly newPoly;
		if (red.first) {
			CARL_LOG_DEBUG("carl.lazard", "Substituting " << v << " by " << red.second);
			newPoly = mLiftingPoly.substitute(v, red.second);
		} else {
			CARL_LOG_DEBUG("carl.lazard", "Obtained reductor " << red.second);
			newPoly = mLiftingPoly.remainder(red.second);
		}
		while (newPoly.isZero()) {
			if (red.first) {
				mLiftingPoly = mLiftingPoly.quotient(v - red.second);
				newPoly = mLiftingPoly.substitute(v, red.second);
			} else {
				mLiftingPoly = mLiftingPoly.quotient(red.second);
				newPoly = mLiftingPoly.remainder(red.second);
			}
			CARL_LOG_DEBUG("carl.lazard", "Reducing to " << mLiftingPoly);
		}
		mLiftingPoly = newPoly;
		CARL_LOG_DEBUG("carl.lazard", "Remaining poly: " << mLiftingPoly);
	}
	
	const auto& getLiftingPoly() const {
		return mLiftingPoly;
	}
};

}

#endif
