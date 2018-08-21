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
			std::vector<Variable> mSymbolBack;
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
						monContent.emplace_back(ci.mSymbolBack[i], exponents[i]);
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
	std::vector<Variable> mSymbolsBack;
	
	Poly mLiftingPoly;
	std::vector<Poly> mReductors;
	
	auto buildPolyRing(Variable v, const RealAlgebraicNumber<Rational>& r) {
		CoCoA::SparsePolyRing ring = CoCoA::NewPolyRing(mQ, {CoCoA::NewSymbol()});
		mSymbolsThere.emplace(v, CoCoA::indets(ring)[0]);
		mSymbolsBack.emplace_back(v);
		return detail_lazard::CoCoAConverter::ConversionInfo({
			ring, mSymbolsThere, mSymbolsBack
		});
	}
	
	bool evaluatesToZero(const CoCoA::RingElem& p, const detail_lazard::CoCoAConverter::ConversionInfo& ci) const {
		auto mp = cc.convertMV<Poly>(p, ci);
		auto res = carl::model::evaluate(mp, mModel);
		CARL_LOG_TRACE("carl.lazard", "Evaluated " << p << " -> " << mp << " -> " << res);
		assert(res.isRational() || res.isRAN());
		if (res.isRational()) return carl::isZero(res.asRational());
		return carl::isZero(res.asRAN());
	}
	
	void extendRing(const CoCoA::ring& ring, const CoCoA::RingElem& p) {
		mQ = CoCoA::NewQuotientRing(ring, CoCoA::ideal(p));
	}
	
	boost::optional<Poly> getSubstitution(Variable v, const RealAlgebraicNumber<Rational>& r) {
		if (r.isNumeric()) {
			CARL_LOG_TRACE("carl.lazard", "Is numeric: " << v << " -> " << r);
			return Poly(r.value());
		}
		Poly res;
		detail_lazard::CoCoAConverter::ConversionInfo ci = buildPolyRing(v, r);
		CoCoA::RingElem p = cc.convertUV(r.getIRPolynomial().replaceVariable(v), ci);
		auto factorization = CoCoA::factor(p);
		CARL_LOG_TRACE("carl.lazard", "Factorization of " << p << " on " << ci.mRing << ": " << factorization);
		for (const auto& f: factorization.myFactors()) {
			if (evaluatesToZero(f, ci)) {
				if (CoCoA::deg(f) == 1) {
					auto cf = -(f - CoCoA::LF(f));
					return cc.convertMV<Poly>(cf, ci);
				} else {
					mReductors.emplace_back(cc.convertMV<Poly>(f, ci));
					extendRing(ci.mRing, f);
					return boost::none;
				}
			}
		}
		return res;
	}
	
public:
	LazardEvaluation(const Poly& p): mLiftingPoly(p) {}
	
	void substitute(Variable v, const RealAlgebraicNumber<Rational>& r) {
		mModel.emplace(v, r);
		auto subs = getSubstitution(v, r);
		if (subs) {
			CARL_LOG_DEBUG("carl.lazard", "Substituting " << v << " by " << *subs);
			Poly newPoly = mLiftingPoly.substitute(v, *subs);
			while (newPoly.isZero()) {
				mLiftingPoly = mLiftingPoly.quotient(v - *subs);
				newPoly = mLiftingPoly.substitute(v, *subs);
				CARL_LOG_DEBUG("carl.lazard", "Reducing to " << mLiftingPoly);
			}
			mLiftingPoly = newPoly;
		} else {
			CARL_LOG_DEBUG("carl.lazard", "Reducing " << v << " by " << mReductors.back());
			mLiftingPoly = mLiftingPoly.remainder(mReductors.back());
		}
		CARL_LOG_DEBUG("carl.lazard", "Remaining poly: " << mLiftingPoly);
	}
	
	const auto& getLiftingPoly() const {
		return mLiftingPoly;
	}
};

}

#endif
