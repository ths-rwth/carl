#!/bin/bash

# This script gets the version number of GMP.
# Assumes that env variable CXX is set, and that a link to GMP header file is in $COCOA_EXTLIB_DIR/include
# if there is no system default GMP.

# If an error occurs, exit code is non-zero (& mesg is printed on stderr).
# Otherwise exit code is zero, and output is version number (e.g. 6.0.1)

if [ "$#" != 0 ]
then
  echo "$0: ERROR: expects no args"
  exit 1
fi

if [ -z "$CXX" ]
then
  echo "$0: ERROR: shell variable CXX must be set to a C++ compiler compatible with GMP"
  exit 1
fi

if [ -z "$COCOA_EXTLIB_DIR" ]
then
    echo "ERROR: $0: environment variable COCOA_EXTLIB_DIR not set."
    exit 1
fi

if [ \! "$COCOA_EXTLIB_DIR"  -ef "/$COCOA_EXTLIB_DIR" ]
then
  echo "ERROR: $0: environment variable COCOA_EXTLIB_DIR is not absolute: $COCOA_EXTLIB_DIR."
  exit 1
fi

if [ \! -d "$COCOA_EXTLIB_DIR" -o \! -d "$COCOA_EXTLIB_DIR/include" -o \! -d "$COCOA_EXTLIB_DIR/lib" ]
then
  echo "ERROR: $0: environment variable COCOA_EXTLIB_DIR is implausible: $COCOA_EXTLIB_DIR."
  exit 1
fi


# Get version number from the header file; we (ab)use the compiler.
umask 22
TODAY=`date "+%Y-%m-%d"`
TIME=`date "+%H:%M:%S"`
TMP_DIR=/tmp/CoCoALib-config-$USER-$TODAY/gmp-version-$TIME
/bin/rm -rf $TMP_DIR  &&  mkdir -p $TMP_DIR
if [ $? -ne 0 ]; then
  echo "ERROR: $0 failed to create temporary directory \"$TMP_DIR\""
  exit 1
fi

pushd $TMP_DIR  >/dev/null

cat > TestProg.C <<EOF
#include "gmp.h"
#include <iostream>

int main()
{
  std::cout << __GNU_MP_VERSION << "." << __GNU_MP_VERSION_MINOR << "." << __GNU_MP_VERSION_PATCHLEVEL << std::endl;
}
EOF

# Use c++ compiler specified in CXX; no need to specify libgmp as all info is in header file!!
$CXX -I "$COCOA_EXTLIB_DIR/include"  TestProg.C -o TestProg 2> /dev/null

# Check whether compilation failed; if so, complain.
if [ $? -ne 0 ]
then
  echo "ERROR: $0: unable to determine version of GMP library"   > /dev/stderr
  echo "ERROR: $0: (compilation failed in gmp-version.sh)"       > /dev/stderr
  exit 1
fi

# Compilation succeeded, so run $PROG which will print out the version.
GMP_LIB_VERSION=`./TestProg`

popd > /dev/null
/bin/rm -rf $TMP_DIR

# If we get here, all tests have passed, so print version number and exit with code 0.
echo $GMP_LIB_VERSION
