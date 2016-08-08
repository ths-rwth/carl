/* 
 * File:   ThomSettings.h
 * Author: tobias
 *
 * Created on 14. Juni 2016, 19:36
 */

#pragma once

namespace carl {
        
enum class IntermediatePointAlgorithm : unsigned {ROLLE, BOUND, ITERATIVE};
enum class SeperationBound : unsigned            {RUMP, OTHER};
        
struct ThomSettings1 {
               
        static const bool                       SIGN_DETERMINATION_REDUCE =     true;       
        static const bool                       MULT_TABLE_USE_NF_ALG =         true;
        static const IntermediatePointAlgorithm INTERMEDIATE_POINT_ALGORITHM =  IntermediatePointAlgorithm::BOUND;
        static const SeperationBound            SEPERATION_BOUND =              SeperationBound::RUMP;
        static constexpr double                 INITIAL_OFFSET =                0.5;
};

struct ThomSettings2 : ThomSettings1 {
        
        static const bool                       SIGN_DETERMINATION_REDUCE =     false;       
        static const bool                       MULT_TABLE_USE_NF_ALG =         true;
};
        
  
/*
 * Typedef the current settings here!
 */
typedef ThomSettings1 ThomDefaultSettings;
// typedef ThomSettings2 ThomDefaultSettings;


} // namespace carl;