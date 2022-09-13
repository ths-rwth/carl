#pragma once

#include <carl-common/config.h>

#if defined(USE_COCOA) && defined(USE_LIBPOLY)

#include <carl-arith/core/Variable.h>
#include <carl-arith/core/Variables.h>

#include "LPPolynomial.h"

#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <map>

//#include "CoCoA/library.H"
#include <CoCoA/BigInt.H>
#include <CoCoA/BigRat.H>
#include <CoCoA/RingQQ.H>
#include <CoCoA/SparsePolyIter.H>
#include <CoCoA/SparsePolyOps-RingElem.H>
#include <CoCoA/SparsePolyRing.H>
#include <CoCoA/factorization.H>
#include <CoCoA/ring.H>

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
} // namespace cocoawrapper

class CoCoAAdaptorLP {
private:
    std::map<Variable, CoCoA::RingElem> mSymbolThere;
    std::vector<Variable> mSymbolBack;
    CoCoA::ring mQ = CoCoA::RingQQ();
    CoCoA::SparsePolyRing mRing;

    const LPContext& mContext ; 

public:
    static CoCoA::BigInt convert(const mpz_class& n) {
        return CoCoA::BigIntFromMPZ(n.get_mpz_t());
    }
    mpz_class convert(const CoCoA::BigInt& n) const {
        return mpz_class(CoCoA::mpzref(n));
    }
    static CoCoA::BigRat convert(const mpq_class& n) {
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

    /**
     * Helper function to collect Terms of a libpoly polynomial to CoCoA polynomial
     */
    static void collectTerms(const lp_polynomial_context_t* /*ctx*/,
                             lp_monomial_t* m,
                             void* d) {
        using DataLP = std::pair<CoCoA::RingElem*, std::pair<std::map<Variable, CoCoA::RingElem>, std::pair<std::vector<Variable>, std::pair<CoCoA::ring, CoCoA::SparsePolyRing>>>>;

        DataLP* res = static_cast<DataLP*>(d);
        CoCoA::RingElem* resPoly = res->first;
        std::map<Variable, CoCoA::RingElem> mSymbolThere = res->second.first;
        std::vector<Variable> mSymbolBack = res->second.second.first;
        CoCoA::ring mQ = res->second.second.second.first;
        CoCoA::SparsePolyRing mRing = res->second.second.second.second;

        std::vector<long> exponents(mSymbolBack.size());

        for (size_t i = 0; i < m->n; i++) {
            carl::Variable var = VariableMapper::getInstance().getCarlVariable(m->p[i].x);
            auto it = mSymbolThere.find(var);
            assert(it != mSymbolThere.end());
            long indetIndex;
            if (CoCoA::IsIndet(indetIndex, it->second)) {
                exponents[std::size_t(indetIndex)] = long(m->p[i].d);
            } else {
                assert(false && "The symbol is not an inderminant.");
            }
        }

        auto coefPol = poly::Integer(&(m->a));
        mpz_class* coef = poly::detail::cast_to_gmp(&coefPol);
        *resPoly += CoCoA::monomial(mRing, convert(*(coef)), exponents);
    }

    CoCoA::RingElem convert(const LPPolynomial& p) const {
        CoCoA::RingElem res(mRing);
        using DataLP = std::pair<CoCoA::RingElem*, std::pair<std::map<Variable, CoCoA::RingElem>, std::pair<std::vector<Variable>, std::pair<CoCoA::ring, CoCoA::SparsePolyRing>>>>;
        DataLP data{&res, {mSymbolThere, {mSymbolBack, {mQ, mRing}}}};
        lp_polynomial_traverse(p.get_internal(), collectTerms, &data);
        return res;
    }

    LPPolynomial convert(const CoCoA::RingElem& p, const lp_polynomial_context_t* context) const {
        poly::Polynomial temPoly(context);

        for (CoCoA::SparsePolyIter i = CoCoA::BeginIter(p); !CoCoA::IsEnded(i); ++i) {
            mpq_class coeff;
            convert(coeff, CoCoA::coeff(i));
            std::vector<long> exponents;
            CoCoA::exponents(exponents, CoCoA::PP(i));
            poly::Polynomial termPoly(context);
            termPoly += (poly::Integer)carl::get_num(coeff);

            for (std::size_t i = 0; i < exponents.size(); ++i) {
                if (exponents[i] == 0) continue;
                poly::Variable polyVar = VariableMapper::getInstance().getLibpolyVariable(mSymbolBack[i]);
                termPoly *= poly::pow(polyVar, (unsigned int)exponents[i]);
            }
            temPoly += termPoly;
        }

        LPPolynomial res(temPoly);
        return res;
    }

    std::vector<CoCoA::RingElem> convert(const std::vector<LPPolynomial>& p) const {
        std::vector<CoCoA::RingElem> res;
        for (const auto& poly : p)
            res.emplace_back(convert(poly));
        return res;
    }
    std::vector<LPPolynomial> convert(const std::vector<CoCoA::RingElem>& p) const {
        std::vector<LPPolynomial> res;
        for (const CoCoA::RingElem& poly : p)
            res.emplace_back(convert(poly, mContext.context()));
        return res;
    }

    const auto& variables() const {
        return mSymbolBack;
    }


    auto construct_ring(const std::vector<Variable>& vars) const {
        std::vector<CoCoA::symbol> indets;
        for (auto s : vars) {
            indets.emplace_back(s.safe_name());
        }
        return CoCoA::NewPolyRing(mQ, indets) ; 
    }

public:
    
	CoCoAAdaptorLP() = delete ; 

    CoCoAAdaptorLP(const LPContext& ctx) : mSymbolBack(ctx.variable_ordering()), mRing(construct_ring(mSymbolBack)), mContext(ctx) {
		std::vector<Variable> vars = ctx.variable_ordering() ; 

		auto indets = CoCoA::indets(mRing);
		for (std::size_t i = 0; i < mSymbolBack.size(); ++i) {
			mSymbolThere.emplace(mSymbolBack[i], indets[i]);
		}
	}

    // Poly gcd(const Poly& p1, const Poly& p2) const {
    // 	auto start = CARL_TIME_START();
    // 	auto res = convert(cocoawrapper::gcd(convert(p1), convert(p2)));
    // 	CARL_TIME_FINISH(cocoa::statistics().gcd, start);
    // 	return res;
    // }

    // Poly makeCoprimeWith(const Poly& p1, const Poly& p2) const {
    // 	CoCoA::RingElem res = convert(p1);
    // 	return convert(res / cocoawrapper::gcd(res, convert(p2)));
    // }

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
    Factors<LPPolynomial> factorize(const LPPolynomial& p, bool includeConstant = true) const {
        auto finfo = cocoawrapper::factor(convert(p));
        Factors<LPPolynomial> res;
        if (includeConstant && !CoCoA::IsOne(finfo.myRemainingFactor())) {
            res.emplace(convert(finfo.myRemainingFactor(), mContext.context()), 1);
        }
        for (std::size_t i = 0; i < finfo.myFactors().size(); i++) {
            res[convert(finfo.myFactors()[i], mContext.context())] = finfo.myMultiplicities()[i];
        }
        return res;
    }

    /**
     * Break down a polynomial into its unique, irreducible factors
     * without their exponents/multiplicities.
     * E.g. "3*x^3 + 12*x^2 + 15*x + 6" has the unique, non-constant, irreducible
     * factors "(x+1)", "(x+2)", and a constant factor "3" that is included if includeConstant is true.
     */
    std::vector<LPPolynomial> irreducible_factors(const LPPolynomial& p, bool includeConstant = true) const {
        std::vector<LPPolynomial> res;
        for (auto& f : factorize(p, includeConstant)) {
            res.emplace_back(std::move(f.first));
        }
        return res;
    }

    // Poly squareFreePart(const Poly& p) const {
    // 	auto finfo = cocoawrapper::SqFreeFactor(convert(p));
    // 	Poly res(1);
    // 	for (const auto& f: finfo.myFactors()) {
    // 		res *= convert(f);
    // 	}
    // 	return res;
    // }

    // auto GBasis(const std::vector<Coeff>& p) const {
    // 	auto start = CARL_TIME_START();
    // 	auto res = convert(cocoawrapper::ReducedGBasis(convert(p)));
    // 	CARL_TIME_FINISH(cocoa::statistics().gbasis, start);
    // 	return res;
    // }
};

} // namespace carl

#endif
