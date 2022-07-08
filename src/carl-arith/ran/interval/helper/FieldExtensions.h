#pragma once

#include "../Evaluation.h"

#include <carl-arith/poly/umvpoly/functions/Representation.h>

#ifdef USE_COCOA
#include <CoCoA/library.H>
#endif 

namespace carl::ran::interval {
namespace detail_field_extensions {
	#ifdef USE_COCOA
	
	struct CoCoAConverter {

		std::map<Variable, CoCoA::RingElem> mSymbolThere;
		std::map<std::pair<long,std::size_t>, Variable> mSymbolBack;

		auto buildPolyRing(CoCoA::ring coeff_ring, Variable v) {
			CoCoA::SparsePolyRing ring = CoCoA::NewPolyRing(coeff_ring, {CoCoA::NewSymbol()});
			mSymbolThere.emplace(v, CoCoA::indets(ring)[0]);
			mSymbolBack.emplace(std::make_pair(CoCoA::RingID(ring), 0), v);
			return ring;
		}
		
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
				CARL_LOG_ERROR("carl.ran.interval", "Unsupported number type.");
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
		Poly convertMV(const CoCoA::RingElem& p) const {
			Poly res;
			for (CoCoA::SparsePolyIter i = CoCoA::BeginIter(p); !CoCoA::IsEnded(i); ++i) {
				Poly coeff;
				CoCoA::BigRat numcoeff;
				if (CoCoA::IsRational(numcoeff, CoCoA::coeff(i))) {
					coeff = Poly(convert<typename Poly::CoeffType>(numcoeff));
				} else {
					coeff = convertMV<Poly>(CoCoA::CanonicalRepr(CoCoA::coeff(i)));
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
						monContent.emplace_back(mSymbolBack.at(std::make_pair(CoCoA::RingID(ring),i)), exponents[i]);
						tdeg += std::size_t(exponents[i]);
					}
					res += coeff * createMonomial(std::move(monContent), tdeg);
				}
			}
			return res;
		}

		template<typename Poly>
		CoCoA::RingElem convertMV(const Poly& p, const CoCoA::ring& ring) const {
			CoCoA::RingElem res(ring);
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
				res += CoCoA::monomial(ring, convert(t.coeff()), exponents);
			}
			return res;
		}
		
		template<typename Poly>
		CoCoA::RingElem convertUV(const Poly& p, const CoCoA::SparsePolyRing& ring) const {
			CoCoA::RingElem res(ring);
			CoCoA::RingElem exp(ring, 1);
			CoCoA::RingElem var = mSymbolThere.at(p.main_var());
			for (std::size_t deg = 0; deg <= p.degree(); ++deg) {
				res += convert(p.coefficients()[deg]) * exp;
				exp *= var;
			}
			return res;
		}
	};
	#endif
}

/**
 * This class can be used to construct iterated field extensions from a sequence of real algebraic numbers.
 * In particular it makes sure that the minimal polynomials are "reduced", i.e. making sure that they are minimal polynomial w.r.t. the current extension field.
 */
template<typename Rational, typename Poly>
class FieldExtensions {
private:
	std::map<Variable,IntRepRealAlgebraicNumber<Rational>> mModel;

	#ifdef USE_COCOA
	CoCoA::ring mQ = CoCoA::RingQQ();
	detail_field_extensions::CoCoAConverter cc;
		
	auto buildPolyRing(Variable v) {
		return cc.buildPolyRing(mQ, v);
	}

	bool evaluatesToZero(const CoCoA::RingElem& p) const {
		auto mp = cc.convertMV<Poly>(p);
		auto res = carl::evaluate(BasicConstraint<Poly>(mp, Relation::EQ), mModel);
		CARL_LOG_DEBUG("carl.ran.interval", "Evaluated " << p << " -> " << mp << " == 0 -> " << res);
		assert(!indeterminate(res));
		return (bool)res;
	}
	
	void extendRing(const CoCoA::ring& ring, const CoCoA::RingElem& p) {
		mQ = CoCoA::NewQuotientRing(ring, CoCoA::ideal(p));
	}
	#endif
	
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
	std::pair<bool,Poly> extend(Variable v, const IntRepRealAlgebraicNumber<Rational>& r) {
		#ifdef USE_COCOA
		mModel.emplace(v, r);
		if (r.is_numeric()) {
			CARL_LOG_DEBUG("carl.ran.interval", "Is numeric: " << v << " -> " << r);
			return std::make_pair(true, Poly(r.value()));
		}
		auto ci = buildPolyRing(v);
		CoCoA::RingElem p = cc.convertUV(replace_main_variable(r.polynomial(), v), ci);
		CARL_LOG_DEBUG("carl.ran.interval", "Factorization of " << p << " on " << ci);
		auto factorization = CoCoA::factor(p);
		CARL_LOG_DEBUG("carl.ran.interval", "-> " << factorization);
		for (const auto& f: factorization.myFactors()) {
			if (evaluatesToZero(f)) {
				CARL_LOG_DEBUG("carl.ran.interval", "Factor " << f << " is zero in assignment.");
				if (CoCoA::deg(f) == 1) {
					auto cf =-(f -CoCoA::LF(f)) / CoCoA::CoeffEmbeddingHom(CoCoA::owner(f))(CoCoA::LC(f));
					return std::make_pair(true, cc.convertMV<Poly>(cf));
				} else {
					extendRing(ci, f);
					return std::make_pair(false, cc.convertMV<Poly>(f));
				}
			}
		}
		CARL_LOG_ERROR("carl.ran.interval", "No factor is zero in assignment.");
		assert(false);
		return std::make_pair(false, Poly());
		#else
		CARL_LOG_ERROR("carl.ran.interval", "CoCoALib is not enabled");
		assert(false);
		return std::make_pair(false, Poly());
		#endif
	}

	Poly embed(const Poly& poly) { // TODO not functional yet
		#ifdef USE_COCOA
		CARL_LOG_DEBUG("carl.ran.interval", "Embed " << poly << " into " << mQ);
		auto f = cc.convertMV(poly, mQ);
		CARL_LOG_DEBUG("carl.ran.interval", "Embedding is " << f);
		return cc.convertMV<Poly>(f);
		#else
		CARL_LOG_ERROR("carl.ran.interval", "CoCoALib is not enabled");
		assert(false);
		return Poly();
		#endif
	}
};

}
