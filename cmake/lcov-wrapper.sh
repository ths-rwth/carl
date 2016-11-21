#!/bin/bash
echo "Calling llvm-cov with $@"
llvm-cov gcov "$@"
