#!/bin/sh

mkdir -p coverage/*
mkdir -p coverage-out/*
rm -f coverage/*
rm -f coverage-out/*

make
make test LLVM_PROFILE_FILE=coverage/%p.profraw

echo "Merging profile data..."
llvm-profdata merge -sparse coverage/*.profraw -o llvm.profdata

echo "Dumping coverage data..."
llvm-cov show -instr-profile llvm.profdata libcarl.so > coverage-out/coverage.txt
for f in `ls bin/` ; do 
	llvm-cov show -instr-profile llvm.profdata bin/$f > coverage-out/coverage-$f.txt
done
echo "Merging coverage data..."
cat coverage-out/*.txt > coverage.txt

