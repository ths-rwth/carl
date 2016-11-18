#!/bin/bash

# Auxiliary script for CoCoALib configuration process.

# Script to see whether compiler is clang, and then link with
# If no warning is produced, the script prints -fPIC; otherwise it prints nothing.

if [ $# -ne 1 ]
then
  echo "***ERROR*** $0 needs 1 arg (name of C++ compiler)"
  exit 1
fi

CXX="$1"

umask 22
TMP_DIR=fpic-check-$HOSTNAME-$UID-$$
/bin/rm -rf "$TMP_DIR" && mkdir "$TMP_DIR"
if [ $? -ne 0 ]
then
  echo "***ERROR*** $0: unable to create temp directory $TMP_DIR"  
  exit 2
fi
cd "$TMP_DIR"

# test if it is clang:  .... a bit harsh, maybe...
cat > TestProg.C <<EOF
int main()
{
#ifdef __clang__
  exit(1);
#endif
}
EOF


COMPILER_MESG2=`"$CXX" $FPIC_FLAG -c -o TestProg.o TestProg.C 2>& 1`
if [ $? -ne 0 ]
then
  FPIC_LDFLAG="-Wl,-no_pie";
fi

cd ..
/bin/rm -rf "$TMP_DIR"
echo $FPIC_LDFLAG
