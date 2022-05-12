/**
 * @file:   initialize.h
 * @author Sebastian Junges
 
 * This file should be the first included header from the CArL library 
 * in order to make sure some initilization is done right.
 * Created: 20/11/2012
 * Version: 23/11/2012
 */

#pragma once

#include <carl/poly/umvpoly/MonomialPool.h>
#include <carl-common/config.h>
#include <carl-logging/carl-logging.h>

#ifdef USE_COCOA
#include <CoCoA/GlobalManager.H>
#endif

#include <iomanip>
#include <iostream>

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
        logging::configureLogging();
        // initialize anything else here!
		MonomialPool::getInstance();

#ifdef USE_COCOA
        static CoCoA::GlobalManager CoCoAFoundations;
#endif

        return 0;
    }

    /**
     * Method to ensure that upon inclusion, init() is called exactly once.
     * TODO prevent outside access.
     */
    inline int initialize()
    {
        static int initvariable = init();
        do { (void)(initvariable); } while (false);
        return 0;
    }

    /// Call to initialize.
    static int initvariable = initialize();
	// TODO Suppress unused variable.
}
