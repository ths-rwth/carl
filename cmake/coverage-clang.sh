#!/bin/sh

CMAKE_BINARY_DIR=$1

mkdir -p ${CMAKE_BINARY_DIR}/coverage
mkdir -p ${CMAKE_BINARY_DIR}/coverage-out
rm -f ${CMAKE_BINARY_DIR}/coverage/*
rm -f ${CMAKE_BINARY_DIR}/coverage-out/*

make
make test LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage/%p.profraw

llvm-profdata merge -sparse ${CMAKE_BINARY_DIR}/coverage/*.profraw -o ${CMAKE_BINARY_DIR}/llvm.profdata

llvm-cov show -instr-profile llvm.profdata libcarl.so > coverage-out/coverage.txt
for f in `ls bin/` ; do 
	llvm-cov show -instr-profile llvm.profdata bin/$f > coverage-out/coverage-$f.txt
done
cat coverage-out/*.txt > coverage.txt
