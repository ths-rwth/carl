/* 
 * File:   ThomEncoding.h
 * Author: tobias
 *
 * Created on 19. August 2016, 10:22
 */

#pragma once

#include "SignDetermination/SignDetermination.h"


namespace carl {

template<typename Number>
class ThomEncoding {
        
        typedef MultivariatePolynomial<Number> Polynomial;
        
        SignCondition mSc;
        Polynomial mP;
        Variable mMainVar;
        
        std::shared_ptr<ThomEncoding<Number>> mPoint;
        std::shared_ptr<SignDetermination<Number>> mSd;
        
        uint mDim;
        
public:
        
        ThomEncoding(
                const SignCondition& sc,
                const Polynomial& p,
                Variable::Arg mainVar,
                std::shared_ptr<ThomEncoding<Number>> point, 
                std::shared_ptr<SignDetermination<Number>> sd,
                uint dim
        ) :
                mSc(sc),
                mP(p),
                mMainVar(mainVar),
                mPoint(point),
                mSd(sd),
                mDim(dim)
        {}
              
        inline SignCondition signCondition() const { return mSc; }
        inline Variable::Arg mainVar() const { return mMainVar; }
        inline uint dimension() const { return mDim; }
        inline const Polynomial& polynomial() const { return mP; } 
        inline const ThomEncoding<Number>& point() const {assert(mPoint); return *mPoint; }
        inline SignDetermination<Number> sd() const {assert(mSd); return *mSd; }
        
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
        
        Sign signOnPolynomial(const Polynomial& p) const {
                std::list<SignCondition> signs = sd->getSigns(p);
                for(const auto& sigma : signs) {
                        if(mSc.isSuffixOf(sigma)) return sigma.front();
                }
                CARL_LOG_ASSERT("carl.thom.sign", false, "we should never get here");
        }
        
        template<typename N>
        friend ThomComparisonResult compare(const ThomEncoding<N>& lhs, const ThomEncoding<N>& rhs);
                

        
        template<typename N>
        friend std::ostream& operator<<(std::ostream& os, const ThomEncoding<N>& rhs) {
                os << "\n---------------------------------------------------" << std::endl;
                os << "Thom encoding" << std::endl;
                os << "---------------------------------------------------" << std::endl;
                os << "sign condition:\t\t\t" << rhs.mSc << std::endl;
                os << "polynomial:\t\t\t" << rhs.mP << std::endl;
                os << "main variable: \t\t\t" << rhs.mMainVar  << std::endl;
                os << "point below:\t\t\t" << rhs.mPoint << std::endl;
                os << "sign determination obejct:\t" << rhs.mSd << std::endl;
                os << "dimension:\t\t\t" << rhs.dimension() << std::endl;
                os << "---------------------------------------------------" << std::endl;
                return os;
        }
        
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
                
        
        
}; // class ThomEncoding


} // namespace carl

#include "ThomEncoding.tpp"
