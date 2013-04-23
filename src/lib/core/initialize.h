/*
 * File:   initialize.h
 * This file should be the first included header from the GiNaCRA library 
 * in order to make sure some initilization is done right.
 * @author Sebastian Junges
 * Created: 20/11/2012
 * Version: 23/11/2012
 */

#pragma once

#include <iomanip>

#include "config.h"
#include "logging.h"
#include "settings.h"

namespace arithmetic
{
    /**
     * The routine for initializing the arithmetic library.
     * Which is called automatically by including this header.
     * TODO prevent outside access.
     */
    inline int init()
    {
        // initializing the logging library.
        configureLogging();
        LOGMSG_DEBUG( getLog( "arithmetic" ), "Logging up and running" );
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
}
