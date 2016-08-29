/* 
 * File:   ThomEncoding.h
 * Author: tobias
 *
 * Created on 19. August 2016, 10:22
 */

#pragma once

#include "SignDetermination/SignDetermination.h"
#include "ThomRootFinder.h"

// some settings
#define INITIAL_OFFSET (Number(1)/10)


namespace carl {

template<typename Number>
class ThomEncoding {
        
        typedef MultivariatePolynomial<Number> Polynomial;
        
        mutable SignCondition mSc; // this can grow when comparing
        Polynomial mP;
        Variable mMainVar;
        
        std::shared_ptr<ThomEncoding<Number>> mPoint;
        mutable std::shared_ptr<SignDetermination<Number>> mSd;
        
        uint mRelevant;
        
public:
        
        ThomEncoding(
                const SignCondition& sc,
                const Polynomial& p,
                Variable::Arg mainVar,
                std::shared_ptr<ThomEncoding<Number>> point, 
                std::shared_ptr<SignDetermination<Number>> sd,
                uint mRelevant
        ) :
                mSc(sc),
                mP(p),
                mMainVar(mainVar),
                mPoint(point),
                mSd(sd),
                mRelevant(mRelevant)
        {}
                
        ThomEncoding(
                const Number& n,
                Variable::Arg mainVar,
                std::shared_ptr<ThomEncoding<Number>> point = nullptr
        ) {
                Polynomial p = Polynomial(mainVar) - n;
                std::list<ThomEncoding<Number>> roots = realRootsThom(p, mainVar, point);
                CARL_LOG_ASSERT("carl.thom", roots.size() == 1, "");
                mSc = roots.front().mSc;
                mP = roots.front().mP;
                mMainVar = mainVar;
                mPoint = point;
                mSd = roots.front().mSd;
                mRelevant = roots.front().mRelevant;      
        }
        
        ThomEncoding(
                const ThomEncoding<Number>& te,
                std::shared_ptr<ThomEncoding<Number>> point
        ) {
                CARL_LOG_ASSERT("carl.thom", te.mP.isUnivariate(), "");
                CARL_LOG_ASSERT("carl.thom", te.mPoint == nullptr, "");
                CARL_LOG_ASSERT("carl.thom", point != nullptr, "");
                std::list<ThomEncoding<Number>> roots = realRootsThom(te.mP, te.mMainVar, point);
                for(const auto r : roots) {
                        if(te.relevantSignCondition() == r.mSc) {
                                mSc = r.mSc;
                                mP = r.mP;
                                mMainVar = r.mMainVar;
                                mPoint = r.mPoint;
                                mSd = r.mSd;
                                mRelevant = r.mRelevant;
                                return;
                        }
                }
                CARL_LOG_ASSERT("carl.thom", false, "should never get here");
        }
              
        inline SignCondition signCondition() const { return mSc; }
        inline SignCondition relevantSignCondition() const { return mSc.trailingPart(mRelevant); }
        inline Variable::Arg mainVar() const { return mMainVar; }
        inline const Polynomial& polynomial() const { return mP; } 
        inline const ThomEncoding<Number>& point() const {assert(mPoint); return *mPoint; }
        inline SignDetermination<Number> sd() const {assert(mSd); return *mSd; }
        
        uint dimension() const {
                if(mPoint == nullptr) return 1;
                return 1 + point().dimension(); 
        }
        
        std::list<Polynomial> accumulatePolynomials() const {
                if(mPoint == nullptr) return {polynomial()};
                std::list<Polynomial> res = point().accumulatePolynomials();
                res.push_front(polynomial());
                return res;
        }
        
        SignCondition accumulateSigns() const {
                if(mPoint == nullptr) return signCondition();
                SignCondition res = point().accumulateSigns();
                res.insert(res.begin(), mSc.begin(), mSc.end());
                return res;
        }
        
        SignCondition accumulateRelevantSigns() const {
                if(mPoint == nullptr) return this->relevantSignCondition();
                SignCondition res = point().accumulateRelevantSigns();
                SignCondition mScRel = this->relevantSignCondition();
                res.insert(res.begin(), mScRel.begin(), mScRel.end());
                return res;
        }
        
        Sign signOnPolynomial(const Polynomial& p) const {
                CARL_LOG_ASSERT("carl.thom", p.gatherVariables().size() <= this->dimension(), "");
                std::list<SignCondition> signs = mSd->getSigns(p);
                SignCondition relevant = accumulateRelevantSigns();
                for(const auto& sigma : signs) {
                        if(relevant.isSuffixOf(sigma)) return sigma.front();
                }
                CARL_LOG_ASSERT("carl.thom", false, "we should never get here");
        }
        
        
        void extendSignCondition() const {
                CARL_LOG_ASSERT("carl.thom", signCondition().size() < polynomial().degree(mMainVar) + 1, "cannot extend sign condition any more");
                if(mSc.size() == mP.degree(mMainVar)) {
                        mSc.push_front(Sign::ZERO);
                        return;
                }
                Polynomial derivative = der(mP, mMainVar, 1, mP.degree(mMainVar) - mSc.size()).back();
                mSc.push_front(this->signOnPolynomial(derivative));
        }
        
        Sign sgnReprNum() const {
                return signOnPolynomial(Polynomial(mMainVar));
        }
        
        bool isZero() const {
                return sgnReprNum() == Sign::ZERO;
        }
        
        //*******************//
        //    COMPARISON     //
        //*******************//
        
private:      
        static ThomComparisonResult compare(const ThomEncoding<Number>& lhs, const ThomEncoding<Number>& rhs) {
                CARL_LOG_INFO("carl.thom.compare", "lhs = " << lhs << ", rhs = " << rhs);
                
                // 1. Encodings from different levels
                if(lhs.mainVar() != rhs.mainVar()) {
                        CARL_LOG_TRACE("carl.thom.compare", "encodings are from different levels");
                        // in this case it is not necessary to compare them by the numbers they actually represent
                        return compareDifferentLevels(lhs, rhs);
                }
                CARL_LOG_ASSERT("carl.thom.compare", lhs.mainVar() == rhs.mainVar(), "");
                
                // 2. same underlying polynomial
                if(lhs.polynomial() == rhs.polynomial()) {
                        CARL_LOG_TRACE("carl.thom.compare", "comparing encodings with same underlying polynomials");
                        return SignCondition::compare(lhs.relevantSignCondition(), rhs.relevantSignCondition());
                }
                
                // 3. different underlying polynomial
                if( !(lhs.mPoint == nullptr && rhs.mPoint != nullptr) && !(lhs.mPoint != nullptr && rhs.mPoint == nullptr)) {
                        CARL_LOG_TRACE("carl.thom.compare", "comparing encodings with different underlying polynomials, !!hopefully with same defining point!!");
                        return compareDifferentPoly(lhs, rhs);
                }

                // 4. one of the encodings is "new" on this level and therefore has no underlying point
                CARL_LOG_ASSERT("carl.thom.compare", lhs.mPoint == nullptr || rhs.mPoint == nullptr, "");
                CARL_LOG_ASSERT("carl.thom.compare", lhs.mPoint != nullptr || rhs.mPoint != nullptr, "");
                if(lhs.mPoint == nullptr) {
                        return compare(ThomEncoding<Number>(lhs, rhs.mPoint), rhs);
                }
                if(rhs.mPoint == nullptr) {
                        return compare(lhs, ThomEncoding<Number>(rhs, lhs.mPoint));
                }
                        
                CARL_LOG_ASSERT("carl.thom.compare", false, "we should never get here ... inputs where lhs = " << lhs << ", rhs = " << rhs);
                return EQUAL;
        }
        
        static ThomComparisonResult compareDifferentLevels(const ThomEncoding<Number>& lhs, const ThomEncoding<Number>& rhs) {
                // TODO implement sinnvolle ordnung
                //CARL_LOG_WARN("carl.thom.compare", "returning LESS as comparison result for encodings on different levels");
                //return LESS;
                if(lhs.polynomial() < rhs.polynomial()) return LESS;
                else if(lhs.polynomial() > rhs.polynomial()) return GREATER;
                else return EQUAL;
        }
        
        static ThomComparisonResult compareRational(const ThomEncoding<Number>& lhs, const Number& rhs) {
                ThomEncoding<Number> rhs_te(rhs, lhs.mainVar(), lhs.mPoint);
                return compare(lhs, rhs_te);
        }
        
        static ThomComparisonResult compareDifferentPoly(const ThomEncoding<Number>& lhs, const ThomEncoding<Number>& rhs);
                

public:       
        
        // compare thom encodings with thom encodings
        template<typename N>
        friend bool operator<(const ThomEncoding<N>& lhs, const ThomEncoding<N>& rhs) { return compare(lhs, rhs) == LESS; }
        template<typename N>
        friend bool operator<=(const ThomEncoding<N>& lhs, const ThomEncoding<N>& rhs) { return compare(lhs, rhs) != GREATER; } 
        template<typename N>
        friend bool operator>(const ThomEncoding<N>& lhs, const ThomEncoding<N>& rhs) { return compare(lhs, rhs) == GREATER; }    
        template<typename N>
        friend bool operator>=(const ThomEncoding<N>& lhs, const ThomEncoding<N>& rhs) { return compare(lhs, rhs) != LESS; } 
        template<typename N>
        friend bool operator==(const ThomEncoding<N>& lhs, const ThomEncoding<N>& rhs) { return compare(lhs, rhs) == EQUAL; }
        template<typename N>
        friend bool operator!=(const ThomEncoding<N>& lhs, const ThomEncoding<N>& rhs) { return compare(lhs, rhs) != EQUAL; }
        
        // comparing with rational numbers
        template<typename N>
        friend bool operator<(const ThomEncoding<N>& lhs, const N& rhs) { return compareRational(lhs, rhs) == LESS; }   
        template<typename N>
        friend bool operator<=(const ThomEncoding<N>& lhs, const N& rhs) { return compareRational(lhs, rhs) != GREATER; }
        template<typename N>
        friend bool operator>(const ThomEncoding<N>& lhs, const N& rhs) { return compareRational(lhs, rhs) == GREATER; }
        template<typename N>
        friend bool operator>=(const ThomEncoding<N>& lhs, const N& rhs) { return compareRational(lhs, rhs) != LESS; }    
        template<typename N>
        friend bool operator==(const ThomEncoding<N>& lhs, const N& rhs) { return compareRational(lhs, rhs) == EQUAL; }  
        template<typename N>
        friend bool operator!=(const ThomEncoding<N>& lhs, const N& rhs) { return compareRational(lhs, rhs) != EQUAL; }
              
        template<typename N>
        friend bool operator<(const N& lhs, const ThomEncoding<N>& rhs) { return rhs > lhs; }      
        template<typename N>
        friend bool operator<=(const N& lhs, const ThomEncoding<N>& rhs) { return rhs >= lhs; }    
        template<typename N>
        friend bool operator>(const N& lhs, const ThomEncoding<N>& rhs) { return rhs < lhs; }   
        template<typename N>
        friend bool operator>=(const N& lhs, const ThomEncoding<N>& rhs) { return rhs <= lhs; }
        template<typename N>
        friend bool operator==(const N& lhs, const ThomEncoding<N>& rhs) { return rhs == lhs; } 
        template<typename N>
        friend bool operator!=(const N& lhs, const ThomEncoding<N>& rhs) { return rhs != lhs; }
        
        
        /* Addition */
        
        template<typename N>
        friend ThomEncoding<N> operator+(const ThomEncoding<N>& lhs, const N& rhs) {
                Polynomial subs = Polynomial(lhs.mMainVar) - rhs;
                Polynomial p = lhs.mP.substitute(lhs.mMainVar, subs);
                std::list<ThomEncoding<N>> roots = realRootsThom(p, lhs.mMainVar, lhs.mPoint);
                for(const auto r : roots) {
                        if(lhs.relevantSignCondition() == r.mSc) return r;
                }
                CARL_LOG_ASSERT("carl.thom.samples", false, "we should never get here");
        }
        
        
        template<typename N>
        friend ThomEncoding<N> operator+(const N& lhs, const ThomEncoding<N>& rhs) { return rhs + lhs; }
        
        /*
         * returns a thom encoding representing a number in the interval (lhs, rhs)
         */ 
        static ThomEncoding<Number> intermediatePoint(const ThomEncoding<Number>& lhs, const ThomEncoding<Number>& rhs) {
                CARL_LOG_INFO("carl.thom.samples", lhs << ", " << rhs);
                CARL_LOG_ASSERT("carl.thom.samples", lhs.mMainVar == rhs.mMainVar, "no intermediate point between encodings on different levels!");
                CARL_LOG_ASSERT("carl.thom.samples", lhs < rhs, "lhs >= rhs !!!");
                Number epsilon(INITIAL_OFFSET);
                std::cout << "epsilon = " << epsilon << std::endl;
                // pick the polynomial with smaller degree here (or lhs, if equal degree)
                if(lhs.mP.degree(lhs.mMainVar) <= rhs.mP.degree(rhs.mMainVar)) {
                        ThomEncoding<Number> res = lhs + epsilon;
                        while(res >= rhs) {
                                epsilon /= 2;
                                res = lhs + epsilon;
                        }
                        CARL_LOG_TRACE("carl.thom.samples", "result: " << res);
                        return res;
                }
                else {
                        epsilon = -epsilon;
                        ThomEncoding<Number> res = rhs + epsilon;
                        std::cout << "res = " << res << std::endl;
                        while(lhs >= res) {
                                epsilon /= 2;
                                res = rhs + epsilon;
                        }
                        CARL_LOG_TRACE("carl.thom.samples", "result: " << res);
                        return res;
                }
                
        }
        
        /*
         * finds an intermediate point between the thom encoding and the rational
         * always returns a rational
         */
        static Number intermediatePoint(const ThomEncoding<Number>& lhs, const Number& rhs) {
                CARL_LOG_INFO("carl.thom.samples", lhs << ", " << rhs);
                CARL_LOG_ASSERT("carl.thom.samples", lhs < rhs, "intermediatePoint with wrong order or equal arguments called");
                Number res;
                Number epsilon(INITIAL_OFFSET);
                res = rhs - epsilon;
                while(lhs >= res) {
                    epsilon /= 2;
                    res = rhs - epsilon;
                }
                CARL_LOG_TRACE("carl.thom.samples", "result: " << res);
                return res;
        }
        
        static Number intermediatePoint(const Number& lhs, const ThomEncoding<Number>& rhs) {
                CARL_LOG_INFO("carl.thom.samples", lhs << ", " << rhs);
                CARL_LOG_ASSERT("carl.thom.samples", lhs < rhs, "intermediatePoint with wrong order order in arguments called");
                Number res;
                Number epsilon(INITIAL_OFFSET);
                res = lhs + epsilon;
                while(res >= rhs) {
                    epsilon /= 2;
                    res = lhs + epsilon;
                }
                CARL_LOG_TRACE("carl.thom.samples", "result: " << res);
                return res;
        }
                
        
        /* Output */
        
        void print(std::ostream& os) const {
                os << "\n---------------------------------------------------" << std::endl;
                os << "Thom encoding" << std::endl;
                os << "---------------------------------------------------" << std::endl;
                os << "sign condition:\t\t\t" << mSc << std::endl;
                os << "relevant:\t\t\t" << mRelevant << std::endl;
                os << "polynomial:\t\t\t" << mP << std::endl;
                os << "main variable: \t\t\t" << mMainVar  << std::endl;
                os << "point below:\t\t\t" << mPoint << std::endl;
                os << "sign determination obejct:\t" << mSd << std::endl;
                os << "dimension:\t\t\t" << dimension() << std::endl;
                os << "---------------------------------------------------" << std::endl;
        }
        
        template<typename N>
        friend std::ostream& operator<<(std::ostream& os, const ThomEncoding<N>& rhs) {
                os << rhs.mP << " in " << rhs.mMainVar << ", " << rhs.mSc << "(" << rhs.mRelevant << ")";
                if(rhs.dimension() > 1) {
                        os << " OVER " << rhs.point();
                }
                return os;
        }
        
}; // class ThomEncoding


} // namespace carl

#include "ThomEncoding.tpp"
