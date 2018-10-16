#pragma once

#include "../../../../converter/Z3Converter.h"

#include <boost/optional.hpp>

namespace carl { // TODO template ...

    template<typename Number>
    class Z3Ran {
        private:
            algebraic_numbers::anum mContent;

            mutable Number mLower;
            mutable Number mUpper;
            mutable Interval<Number> mInterval;
            mutable boost::optional<UnivariatePolynomial<Number>> mPolynomial;

        public:
            Z3Ran() = default;

            Z3Ran(algebraic_numbers::anum content) : mContent(content) {}

            const algebraic_numbers::anum& content() const {
                return mContent;
            }

            Z3Ran(const Number& r) {
                z3().anumMan().set(mContent, z3().toZ3MPQ(r));
            }

            bool isZero() const {
                z3().anumMan().is_zero(mContent);
            }

            bool isIntegral() const {
                z3().anumMan().is_int(mContent);
            }
           
            const Number& lower() const {
                mpq res;
                z3().anumMan().get_lower(mContent, res);
                mLower = z3().toNumber<Number>(res);
                return mLower;
            } 

            const Number& upper() const {
                mpq res;
                z3().anumMan().get_upper(mContent, res);
                mUpper = z3().toNumber<Number>(res);
                return mUpper;
            } 

            const Interval<Number>& getInterval() const {
                const Number& lo = lower();
                const Number& up = upper();
                mInterval = Interval<Number>(lo, BoundType::STRICT, up, BoundType::STRICT);
                return mInterval;
            }

            Number branchingPoint() const
            {
                const Number& low = lower();
                const Number& up = upper();
                const Number& mid = (up-low)/2;
                
                const Number& midf = carl::floor(mid);
                if (low <= midf)
                    return midf;
                const Number& midc = carl::ceil(mid);
                if (up >= midc)
                    return midc;
                return mid;
            }

            const UnivariatePolynomial<Number>& getPolynomial() const {
                svector<mpz> res;
                z3().anumMan().get_polynomial(mContent, res);
                mPolynomial = z3().toUnivPoly<Number>(res);
                return *mPolynomial;
                // TODO use IntervalContent<Number>::auxVariable
            }

            Sign sgn() const {
                if (z3().anumMan().is_zero(mContent))
                    return Sign::ZERO;
                else if (z3().anumMan().is_pos(mContent))
                    return Sign::POSITIVE;
                else if (z3().anumMan().is_neg(mContent))
                    return Sign::NEGATIVE;
                else
                    assert(false);
            }

            Sign sgn(const UnivariatePolynomial<Number>& p) const {
                polynomial::polynomial_ref poly = z3().toZ3(p);
                nlsat::assignment map(z3().anumMan());
                polynomial::var var = z3().toZ3(p.mainVar());
                map.set(var, content());
                int rs = z3().anumMan().eval_sign_at(poly, map);
            }

            bool containedIn(const Interval<Number>& i) const {
                if(i.lowerBoundType() != BoundType::INFTY) {
                    if(i.lowerBoundType() == BoundType::STRICT &&
                        z3().anumMan().le(content(), z3().toZ3MPQ(i.lower()))) return false;
                    if(i.lowerBoundType() == BoundType::WEAK &&
                        z3().anumMan().lt(content(), z3().toZ3MPQ(i.lower()))) return false;
                }
                if(i.upperBoundType() != BoundType::INFTY) {
                    if(i.upperBoundType() == BoundType::STRICT &&
                        z3().anumMan().ge(content(), z3().toZ3MPQ(i.upper()))) return false;
                    if(i.upperBoundType() == BoundType::WEAK &&
                        z3().anumMan().gt(content(), z3().toZ3MPQ(i.upper()))) return false;
                }
                return true;
            }

            Z3Ran abs() const {
                if (z3().anumMan().is_pos(mContent)) {
                    return *this;
                }
                else if (z3().anumMan().is_zero(mContent)) {
                    return *this;
                }
                else if (z3().anumMan().is_neg(mContent)) {
                    algebraic_numbers::anum res;
                    z3().anumMan().set(res, mContent);
                    z3().anumMan().neg(res);
                    return res;
                }
            }

            bool equal(const Z3Ran<Number>& n) const {
                return z3().anumMan().eq(mContent,n.mContent);
            }

	        bool less(const Z3Ran<Number>& n) const {
                return z3().anumMan().lt(mContent,n.mContent);
            }
    };

}