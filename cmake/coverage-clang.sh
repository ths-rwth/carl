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

OBJS=""
for obj in `ls libcarl*.so` `find bin/ -type f`
do
	if llvm-cov show -instr-profile llvm.profdata -object $obj > /dev/null 2> /dev/null
	then
		OBJS="$OBJS -object $obj"
	else
		echo "No coverage data for $obj"
	fi
done

eval "llvm-cov show -instr-profile llvm.profdata libcarl.so $OBJS > coverage.txt"
eval "llvm-cov show -Xdemangler c++filt -instr-profile llvm.profdata -format html -output-dir coverage-html/ libcarl.so $ARG1 $ARG2"

