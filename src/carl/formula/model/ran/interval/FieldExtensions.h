#pragma once

#include "ran_interval_evaluation.h"

#include <carl/core/polynomialfunctions/Representation.h>

#ifdef USE_COCOA

#include <CoCoA/library.H>

namespace carl {
namespace detail_field_extensions {
	
	struct CoCoAConverter {
		struct ConversionInfo {
			CoCoA::SparsePolyRing mRing;
			const std::map<Variable, CoCoA::RingElem>& mSymbolThere;
			const std::map<std::pair<long,std::size_t>, Variable>& mSymbolBack;
		};
		
		CoCoA::BigRat convert(const mpq_class& n) const {
			return CoCoA::BigRatFromMPQ(n.get_mpq_t());
		}
		template<typename T>
		T convert(const CoCoA::BigRat& n) const {
			if constexpr(std::is_same<T,mpq_class>::value) {
				return mpq_class(CoCoA::mpqref(n));
			}
			#ifdef USE_CLN_NUMBERS
			else if constexpr(std::is_same<T,cln::cl_RA>::value) {
				std::stringstream ss;
				ss << n;
				return T(ss.str().c_str());
			}
			#endif
			else {
				CARL_LOG_ERROR("carl.lazard", "Unsupported number type.");
			}
		}
		#ifdef USE_CLN_NUMBERS
		CoCoA::BigRat convert(const cln::cl_RA& n) const {
			std::stringstream ss;
			ss << n;
			return CoCoA::BigRatFromString(ss.str());
		}
		#endif

		template<typename Poly>
		Poly convertMV(const CoCoA::RingElem& p, const ConversionInfo& ci) const {
			Poly res;
			for (CoCoA::SparsePolyIter i = CoCoA::BeginIter(p); !CoCoA::IsEnded(i); ++i) {
				Poly coeff;
				CoCoA::BigRat numcoeff;
				if (CoCoA::IsRational(numcoeff, CoCoA::coeff(i))) {
					coeff = Poly(convert<typename Poly::CoeffType>(numcoeff));
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

/**
 * This class can be used to construct iterated field extensions from a sequence of real algebraic numbers.
 * In particular it makes sure that the minimal polynomials are "reduced", i.e. making sure that they are minimal polynomial w.r.t. the current extension field.
 */
template<typename Rational, typename Poly>
class FieldExtensions {
private:
	CoCoA::ring mQ = CoCoA::RingQQ();
	std::map<Variable,real_algebraic_number_interval<Rational>> mModel;
	
	detail_field_extensions::CoCoAConverter cc;
	std::map<Variable, CoCoA::RingElem> mSymbolsThere;
	std::map<std::pair<long,std::size_t>, Variable> mSymbolsBack;
	
	auto buildPolyRing(Variable v) {
		CoCoA::SparsePolyRing ring = CoCoA::NewPolyRing(mQ, {CoCoA::NewSymbol()});
		mSymbolsThere.emplace(v, CoCoA::indets(ring)[0]);
		mSymbolsBack.emplace(std::make_pair(CoCoA::RingID(ring), 0), v);
		return detail_field_extensions::CoCoAConverter::ConversionInfo({
			ring, mSymbolsThere, mSymbolsBack
		});
	}
	
	bool evaluatesToZero(const CoCoA::RingElem& p, const detail_field_extensions::CoCoAConverter::ConversionInfo& ci) const {
		auto mp = cc.convertMV<Poly>(p, ci);
		auto res = carl::evaluate(Constraint<Poly>(mp, Relation::EQ), mModel);
		CARL_LOG_DEBUG("carl.lazard", "Evaluated " << p << " -> " << mp << " == 0 -> " << res);
		return res;
	}
	
	void extendRing(const CoCoA::ring& ring, const CoCoA::RingElem& p) {
		mQ = CoCoA::NewQuotientRing(ring, CoCoA::ideal(p));
	}
public:
	/**
	 * Extend the current number field with the field extension defined by r.
	 * The minimal polynomial of r (with is a minimal polynomials in Q[x]) is 
	 * embedded into the current number field and the minimal polynomial for r within this number field is computed.
	 * The resulting polynomial is this minimal polynomial over the current number field.
	 * 
	 * We may have one of two cases:
	 * - We can eliminate v by substitution with some term
	 * - We create a new field extension and may have to reduce the lifting polynomial
	 *
	 * In the first case, we return true and the term to substitute with.
	 * In the second case, we return false and the new minimal polynomial.
	 */
	std::pair<bool,Poly> extend(Variable v, const real_algebraic_number_interval<Rational>& r) {
		mModel.emplace(v, r);
		if (r.is_numeric()) {
			CARL_LOG_DEBUG("carl.lazard", "Is numeric: " << v << " -> " << r);
			return std::make_pair(true, Poly(r.value()));
		}
		detail_field_extensions::CoCoAConverter::ConversionInfo ci = buildPolyRing(v);
		CoCoA::RingElem p = cc.convertUV(replace_main_variable(r.polynomial(), v), ci);
		CARL_LOG_DEBUG("carl.lazard", "Factorization of " << p << " on " << ci.mRing);
		auto factorization = CoCoA::factor(p);
		CARL_LOG_DEBUG("carl.lazard", "-> " << factorization);
		for (const auto& f: factorization.myFactors()) {
			if (evaluatesToZero(f, ci)) {
				CARL_LOG_DEBUG("carl.lazard", "Factor " << f << " is zero in assignment.");
				if (CoCoA::deg(f) == 1) {
					auto cf =-(f -CoCoA::LF(f)) / CoCoA::CoeffEmbeddingHom(CoCoA::owner(f))(CoCoA::LC(f));
					return std::make_pair(true, cc.convertMV<Poly>(cf, ci));
				} else {
					extendRing(ci.mRing, f);
					return std::make_pair(false, cc.convertMV<Poly>(f, ci));
				}
			}
		}
		CARL_LOG_ERROR("carl.lazard", "No factor is zero in assignment.");
		assert(false);
		return std::make_pair(false, Poly());
	}
};

}

#endif
