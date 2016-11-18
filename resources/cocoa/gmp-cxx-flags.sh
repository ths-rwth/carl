#! /bin/bash

# This script prints out the -m*** flags used for compiling GMP
# (or exits with code 1 if an error occurred).
# Assumes that env variables CXX and COCOA_EXTLIB_DIR are set.

if [ $# != 0 ]
then
  echo "ERROR: $0 expects no arguments"
  exit 1
fi

# Check environment variables CXX and COCOA_EXTLIB_DIR
if [ -z "$CXX" ]
then
  echo "ERROR: $0: environment variable CXX not set."
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


# Below we create a small C++ program for printing out the GMP compilation flags.
TODAY=`date "+%Y-%m-%d"`
TIME=`date "+%H:%M:%S"`
TMP_DIR=/tmp/CoCoALib-config-$USER-$TODAY/gmp-cxx-flags-$TIME-$$
/bin/rm -rf $TMP_DIR  &&  mkdir -p $TMP_DIR
if [ $? -ne 0 ]; then
  echo "ERROR: $0 failed to create temporary directory \"$TMP_DIR\""
  exit 1
fi

cd $TMP_DIR
cat > prog.C <<EOF
#include "gmp.h"
#include <iostream>

int main()
{
  std::cout << __GMP_CFLAGS << std::endl;
}
EOF

$CXX -I "$COCOA_EXTLIB_DIR/include" prog.C -o prog 2> /dev/null
GMP_CXXFLAGS=`./prog`
if [ $? -ne 0 ]
then
  # Deliberately leave $TMP_DIR to assist debugging.
  echo "ERROR: $0: test program crashed!"
  exit 1
fi

# GMP_CXXFLAGS contains all the compilation flags used when building GMP.
# We pick out just the compilation options which begin with -m
COCOALIB_CXXFLAGS=
for opt in $GMP_CXXFLAGS
do
  case $opt in
  ( -m* )
    COCOALIB_CXXFLAGS="$COCOALIB_CXXFLAGS $opt";;
  esac
done

#######################################################
# Check that these GMP CXXFLAGS actually work (they may not if the gmp.h and libgmp.a
# have been copied from another machine, e.g. installing via "brew" on MacOSX)
# See redmine 975.

$CXX $COCOALIB_CXXFLAGS -I "$COCOA_EXTLIB_DIR/include" prog.C -o prog  2> /dev/null
if [ $? -ne 0 ]
then
  COCOALIB_CXXFLAGS=""
fi

# Clean up TMP_DIR
cd # Leave TMP_DIR
/bin/rm -rf $TMP_DIR

echo $COCOALIB_CXXFLAGS
