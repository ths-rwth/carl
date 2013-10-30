/*
 * @file:   initialize.h
 * @author Sebastian Junges
 
 * This file should be the first included header from the GiNaCRA library 
 * in order to make sure some initilization is done right.
 * Created: 20/11/2012
 * Version: 23/11/2012
 */

#pragma once

#include <iomanip>
#include <iostream>
#include "config.h"
#include "logging.h"

namespace carl
{
    /**
     * The routine for initializing the carl library.
     * Which is called automatically by including this header.
     * TODO prevent outside access.
     */
    inline int init()
    {
        // initializing the logging library.
        configureLogging();
        // initialize anything else here!
		
        return 0;
    }

    /**
     * Method to ensure that upon inclusion, init() is called exactly once.
     * TODO prevent outside access.
     */
    inline int initialize( void )
    {
        static int initvariable = init();
        do { (void)(initvariable); } while( 0 );
        return 0;
    }

    /// Call to initialize.
    static int initvariable = initialize();
	// TODO Suppress unused variable.
}
