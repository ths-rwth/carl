/* 
 * File:   ThomSettings.h
 * Author: tobias
 *
 * Created on 14. Juni 2016, 19:36
 */

#pragma once

namespace carl {
        
        
} // namespace carl;




struct ThomStandardSettings {
        
        
        /*
         * sign determination settings
         */
        static const bool SIGN_DETERMINATION_REDUCE = false;
        
        /*
         * tarski query settings
         */
        static const bool MULT_TABLE_USE_NF_ALG = true;
};

struct ThomExperimentalSettings {
        
        
        /*
         * sign determination settings
         */
        static const bool SIGN_DETERMINATION_REDUCE = true;
        
        /*
         * tarski query settings
         */
        static const bool MULT_TABLE_USE_NF_ALG = false;
        
};


/*
 * Typedef the current settings here!
 */
typedef ThomStandardSettings ThomCurrentSettings;
// typedef ThomExperimentalSettings ThomCurrentSettings;