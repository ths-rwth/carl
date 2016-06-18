/* 
 * File:   TriangularThomEncoding.h
 * Author: tobias
 *
 * Created on 17. Juni 2016, 17:49
 */

#pragma once

#include "ThomEncoding.h"

template<typename Coeff>
using TriangularSystem = std::vector<MultivariatePolynomial<Coeff>>;

template<typename Coeff>
class TriangularThomEncoding {

        /*
         * The underlying triangular system
         */
        TriangularSystem triangularSystem;
        
        /*
         * The list of thom encodings specifying a root of triangularSystem
         */
        std::vector<ThomEncoding> thomEncodings;
};

