/* 
 * File:   ThomUtil.cpp
 * Author: tobias
 *
 * Created on 17. Juli 2016, 10:17
 */

#include "ThomUtil.h"

namespace carl {
        

// maybe make a class sign condition some day

// states that tau extends sigma (see Algorithms for RAG, p.387)
bool extends(const SignCondition& tau, const SignCondition& sigma) {
        //assert(tau.size() == sigma.size() +1);
        assert(tau.size() > sigma.size());
        for(uint i = tau.size() - sigma.size(); i < tau.size(); i++) {
                if(tau[i] != sigma[i - (tau.size() - sigma.size())]) return false;
        }
        return true;
}

bool isPrefix(const SignCondition& lhs, const SignCondition& rhs) {
        if(lhs.size() > rhs.size()) {
                return false;
        }
        for(uint i = 0; i < lhs.size(); i++) {
                if(lhs[i] != rhs[i]) {
                        return false;
                }
        }
        return true;
}

// compares two sign conditions (associated to the same list of derivatives)
// optimize this so that is returns a comparison result
/*bool operator<(const SignCondition& lhs, const SignCondition& rhs) {
        assert(lhs.size() == rhs.size());
        assert(lhs.back() == rhs.back());
        if(lhs.size() == 1) return false; // because then they are actually equal
        assert(lhs.size() >= 2);
        for(int i = (int)lhs.size() - 2; i >= 0; i--) {
                if(lhs[i] != rhs[i]) {
                        assert(lhs[i+1] == rhs[i+1]);
                        if(lhs[i+1] == Sign::POSITIVE) {
                                return lhs[i] < rhs[i];
                        }
                        else {
                                return lhs[i] > rhs[i];
                        }
                }
        }
        return false;
}*/

// compares two sign conditions (associated to the same list of derivatives)
ThomComparisonResult operator<(const SignCondition& lhs, const SignCondition& rhs) {
        assert(lhs.size() == rhs.size());
        assert(lhs.back() == rhs.back());
        if(lhs.size() == 1) return EQUAL;
        assert(lhs.size() >= 2);
        for(int i = (int)lhs.size() - 2; i >= 0; i--) {
                if(lhs[i] != rhs[i]) {
                        assert(lhs[i+1] == rhs[i+1]);
                        if(lhs[i+1] == Sign::POSITIVE) {
                                return (lhs[i] < rhs[i]) ? ThomComparisonResult::LESS : ThomComparisonResult::GREATER;
                        }
                        else {
                                return (lhs[i] < rhs[i]) ? ThomComparisonResult::GREATER : ThomComparisonResult::LESS;
                        }
                }
        }
        return ThomComparisonResult::EQUAL;
}

std::ostream& operator<<(std::ostream& os, const SignCondition& s) {
    os << "[";
    for(int i = 0; i < (int)s.size(); i++) {
        switch(s[i]) {
            case Sign::NEGATIVE:
                os << "-"; break;
            case Sign::ZERO:
                os << "0"; break;
            case Sign::POSITIVE:
                os << "+"; break;
        }
        if(i != (int)s.size() - 1) os << ","; 
    }
    os << "]";
    return os;
}
        
} // namespace carl
