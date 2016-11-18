#! /bin/sh

# Script assumes that CXX and CXXFLAGS are set.
# This script tests whether CXX can compile with gmp (and gmpxx)
# by just adding -lgmp (and -lgmpxx) as flags.
# The check entails compiling very simple source files.

# Exit code is non-zero if "-lgmp" does not work (or an error occurs).
# Exit code is 0 if "-lgmp" works; output is "GMPXX" if -lgmpxx works,
# otherwise "GMP" if just -lgmp works.

if [ -z "$CXX" ]
then
  echo "$0: ERROR: environment variable CXX not set."
  exit 1
fi


# Now create a temp dir in which to work:
# I use a temp dir so all junk created by the compiler can be removed
# simply by removing the directory -- also I do not need to worry about
# stomping on existing files.

umask 22
TODAY=`date "+%Y-%m-%d"`
TIME=`date "+%H:%M:%S"`
TMP_DIR=/tmp/CoCoALib-config-$USER-$TODAY/gmp-try-default-$TIME-$$
/bin/rm -rf $TMP_DIR  &&  mkdir -p $TMP_DIR
if [ $? -ne 0 ]; then
  echo "ERROR: $0 failed to create temporary directory \"$TMP_DIR\""
  exit 1
fi

cd $TMP_DIR

# Here is the simple source code we shall use to test for gmp:
cat > TestGMP.C <<EOF
#include "gmp.h"
#include <iostream>

int main()
{
  mpz_t i;
  mpz_init(i);
  mpz_set_si(i, 12345);
  std::cout << __GNU_MP_VERSION << "." << __GNU_MP_VERSION_MINOR << "." << __GNU_MP_VERSION_PATCHLEVEL << std::endl;
  mpz_clear(i);
}
EOF

# Here is the simple source code we shall use to test for gmpxx:
cat > TestGMPXX.C <<EOF
#include "gmpxx.h"
#include <iostream>

int main()
{
  mpz_class i(12345);
  std::cout << __GNU_MP_VERSION << "." << __GNU_MP_VERSION_MINOR << "." << __GNU_MP_VERSION_PATCHLEVEL << std::endl;
}
EOF

# Try different architectures (in case we're on a 32/64 bit platform)
DYN_LINK=ok
for ARCHFLAG in "" "-m64" "-m32"
do
  # Try plain GMP
  $CXX $ARCHFLAG $CXXFLAGS TestGMP.C -o TestGMP -lgmp > /dev/null 2>&1
  if [ $? -ne 0 -o \! -f TestGMP -o \! -x TestGMP ]
  then
    # compilation failed so try next ARCHFLAG
    continue;
  fi
  ./TestGMP > /dev/null 2>&1
  if [ $? -ne 0 ]
  then
    DYN_LINK="arch=$ARCHFLAG"
    continue;
  fi  

  # Default GMP worked, now try GMPXX
  $CXX $ARCHFLAG $CXXFLAGS TestGMPXX.C -o TestGMPXX -lgmpxx -lgmp > /dev/null 2>&1
  if [ $? -ne 0 ]
  then
    # GMPXX compilation failed, so we have only GMP
    echo GMP
  fi
  
  # GMPXX compilation passed, so check it runs.
  ./TestGMPXX > /dev/null 2>&1
  if [ $? -ne 0 ]
  then
    # Test prog did not run: accept just GMP (or should it give error???)
    echo GMP
  else
    # We have both GMP and GMPXX
    echo GMPXX
  fi
  # Clean-up TMP_DIR
  cd # leave TMP_DIR
  /bin/rm -rf $TMP_DIR
  exit 0
done

if [ DYN_LINK -ne "ok" ]
then
  echo "Problem with GMP dynamic library ($DYN_LINK); perhaps run ldconfig?"
  exit 2
fi

# No default GMP installation found
echo "No default GMP installation (with CXXFLAGS=\"$CXXFLAGS\")"
exit 1
