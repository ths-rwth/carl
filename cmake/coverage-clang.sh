#!/bin/sh

DIR=`pwd`

mkdir -p coverage/
mkdir -p coverage-out/
rm -f coverage/*
rm -f coverage-out/*

make
make test LLVM_PROFILE_FILE=$DIR/coverage/%p.profraw

echo "Merging profile data..."
llvm-profdata merge -sparse coverage/*.profraw -o llvm.profdata

echo "Dumping coverage data..."
llvm-cov show -instr-profile llvm.profdata libcarl.so > coverage-out/coverage.txt
llvm-cov show -instr-profile llvm.profdata libcarl-cad.so > coverage-out/coverage-cad.txt
llvm-cov show -instr-profile llvm.profdata libcarl-settings.so > coverage-out/coverage-settings.txt
for f in `ls bin/` ; do 
	llvm-cov show -instr-profile llvm.profdata bin/$f > coverage-out/coverage-$f.txt
done
echo "Merging coverage data..."
cat coverage-out/*.txt > coverage.txt

