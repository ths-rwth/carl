#pragma once

#include "../../../../core/Sign.h"
#include "../../../../core/UnivariatePolynomial.h"
#include "../../../../core/MultivariatePolynomial.h"
#include "../../../../interval/Interval.h"

#include <boost/optional.hpp>

namespace carl {

    class Z3RanContent;


    template<typename Number>
    class Z3Ran {
        private:
            std::shared_ptr<Z3RanContent> mContent;

            mutable Number mLower;
            mutable Number mUpper;
            mutable Interval<Number> mInterval;
            mutable boost::optional<UnivariatePolynomial<Number>> mPolynomial;

        public:
            explicit Z3Ran();

            explicit Z3Ran(const Z3RanContent& content);

            explicit Z3Ran(Z3RanContent&& content);

            explicit Z3Ran(const Number& r);

            Z3RanContent& content() const {
                return *mContent;
            }

            bool isZero() const;

            bool isIntegral() const;

            // Note: this already tries to simplify to a polynomial!
            // TODO can this result be cached ?!?
            bool isNumeric() const;

            Number getNumber() const;
           
            const Number& lower() const;

            const Number& upper() const;

            const Interval<Number>& getInterval() const;

            Number branchingPoint() const;

            const UnivariatePolynomial<Number>& getPolynomial() const;

            Sign sgn() const;

            Sign sgn(const UnivariatePolynomial<Number>& p) const;

            bool containedIn(const Interval<Number>& i) const;

            Z3Ran abs() const;

            bool equal(const Z3Ran<Number>& n) const;

	        bool less(const Z3Ran<Number>& n) const;

            bool equal(const Number& n) const;

            bool less(const Number& n) const;

            bool greater(const Number& n) const;
    };

    template<typename Number>
    std::ostream& operator<<(std::ostream& os, const Z3Ran<Number>& zr);    
}