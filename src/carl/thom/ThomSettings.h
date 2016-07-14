/* 
 * File:   ThomSettings.h
 * Author: tobias
 *
 * Created on 14. Juni 2016, 19:36
 */

#pragma once

namespace carl {
        
        
} // namespace carl;




struct ThomBasicSettings {
        
        
        /*
         * sign determination settings
         */
        static const bool SIGN_DETERMINATION_REDUCE = true;
        
        /*
         * tarski query settings
         */
        static const bool MULT_TABLE_USE_NF_ALG = false;
        
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
typedef ThomBasicSettings ThomDefaultSettings;
// typedef ThomExperimentalSettings ThomCurrentSettings;