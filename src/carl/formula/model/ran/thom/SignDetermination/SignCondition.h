/* 
 * File:   SignCondition.h
 * Author: tobias
 *
 * Created on 16. August 2016, 10:54
 */

#pragma once

#include <carl/core/Sign.h>
#include <list>

namespace carl {
        
enum ThomComparisonResult {LESS, EQUAL, GREATER};

class SignCondition : public std::list<Sign> {
        
        // to inherit constructors
        using std::list<Sign>::list;
        
public:
        bool isPrefixOf(const SignCondition& other);
        
        bool isSuffixOf(const SignCondition& other) const {
                if(this->size() > other.size()) return false;
                auto it_this = this->crbegin();
                auto it_other = other.crbegin();
                while(it_this != this->crend()) {
                        if(*it_this != *it_other) return false;
                        it_this++;
                        it_other++;
                }
                return true;
        }
        
        SignCondition trailingPart(uint count) const {
                SignCondition result;
                auto it = this->rbegin();
                for(uint i = 0; i < count; i++) {
                        result.push_front(*it);
                        it++;
                }
                assert(result.size() == count);
                return result;
        }
        
        static ThomComparisonResult compare(const SignCondition& lhs, const SignCondition& rhs) {
                assert(lhs.size() == rhs.size());
                assert(lhs.back() == rhs.back());
                if(lhs.size() == 1) return EQUAL;
                assert(lhs.size() >= 2);
                auto it_lhs = lhs.rbegin();
                auto it_rhs = rhs.rbegin();
                it_lhs++; it_rhs++; // let the iterators point to the second last element
                while(it_lhs != lhs.rend() && it_rhs != rhs.rend()) {
                        if(*it_lhs != *it_rhs) {
                                it_lhs--; it_rhs--;
                                assert(*it_lhs == *it_rhs);
                                if(*it_lhs == Sign::POSITIVE) {
                                        it_rhs++; it_lhs++;
                                        return (*it_lhs < *it_rhs) ? ThomComparisonResult::LESS : ThomComparisonResult::GREATER;
                                }
                                else {
                                        it_rhs++; it_lhs++;
                                        return (*it_lhs < *it_rhs) ? ThomComparisonResult::GREATER : ThomComparisonResult::LESS;
                                }
                        }
                        it_rhs++; it_lhs++;
                }
                return ThomComparisonResult::EQUAL;
        }
        
        friend std::ostream& operator<<(std::ostream& os, const SignCondition& rhs) {
                os << "[" << rhs.size() << ": ";
                for(const auto& sign : rhs) {
                        switch(sign) {
                                case Sign::ZERO:
                                        os << "0";
                                        break;
                                case Sign::POSITIVE:
                                        os << "+";
                                        break;
                                case Sign::NEGATIVE:
                                        os << "-";
                                        break;           
                        }
                        os << ", ";
                }
                os << "]";
                return os;
        }
};


} // namespace carl 
