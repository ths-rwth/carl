#!/bin/bash

# This script expects the (full) path of libgmp.a.
# It prints out the (full) path of the (hopefully) corresponding gmp.h.

if [ $# -ne 1 ]
then
  echo "ERROR: $0: requires 1 arg (path of GMP library)"
  exit 1
fi

GMP_LIB="$1"

PLATFORM=`basename "$GMP_LIB"`
if [ "$PLATFORM" = "lib" ]
then
  PLATFORM=
fi

GMP_LIB_DIR=`dirname "$GMP_LIB"`
GMP_LIB_DIR_DIR=`dirname "$GMP_LIB_DIR"`
# Special handling if libgmp.a is not fully installed...
if [ `basename "$GMP_LIB_DIR"` = ".libs" ]
then
  # GMP is not installed
  GMP_INC_DIR="$GMP_LIB_DIR_DIR"
else
  # GMP is installed -- have to check two possible locations for the header file
  GMP_INC_DIR1="$GMP_LIB_DIR_DIR"/include
  GMP_INC_DIR2=`dirname "$GMP_LIB_DIR_DIR"`/include
  GMP_INC_DIR3="$GMP_LIB_DIR_DIR/include/$PLATFORM"
  if [ -f "$GMP_INC_DIR1/gmp.h" ]
  then
    GMP_INC_DIR="$GMP_INC_DIR1"
  elif [ -f "$GMP_INC_DIR2/gmp.h" ]
  then
    GMP_INC_DIR="$GMP_INC_DIR2"
  elif [ -n "$PLATFORM" -a -f "$GMP_INC_DIR3/gmp.h" ]
  then
    GMP_INC_DIR="$GMP_INC_DIR3"
  else
    echo "ERROR: $0: Cannot find GMP header for $GMP_LIB; searched in $GMP_INC_DIR1 and $GMP_INC_DIR2 and $GMP_INC_DIR3"
    exit 3
  fi
fi
if [ -r "$GMP_INC_DIR/gmp.h" ]
then
  # We've found a plausible gmp.h.
  echo "$GMP_INC_DIR"
  exit 0
fi

# Get here probably only if there's a damaged GMP installation.
echo "ERROR: $0: Trouble reading GMP header file $GMP_INC_DIR/gmp.h"
exit 4
