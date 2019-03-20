#!/bin/sh

DIR=`pwd`

mkdir -p coverage/
mkdir -p coverage-html/
rm -f coverage/*
rm -rf coverage-html/*


make
make test LLVM_PROFILE_FILE=$DIR/coverage/%p.profraw

echo "Merging profile data..."
llvm-profdata merge -sparse coverage/*.profraw -o llvm.profdata

echo "Dumping coverage data..."
ARG1=`ls libcarl*.so | /usr/bin/grep -v carl-cad | sed 's/^/ -object /' | xargs`
ARG2=`find bin/ -type f | sed 's/^/ -object /' | xargs`
eval "llvm-cov show -instr-profile llvm.profdata libcarl.so $ARG1 $ARG2 > coverage.txt"
eval "llvm-cov show -Xdemangler c++filt -instr-profile llvm.profdata -format html -output-dir coverage-html/ libcarl.so $ARG1 $ARG2"

