#!/usr/bin/env bash

mkdir build || return 1
cd build/ || return 1
cmake -D DEVELOPER=ON ../ || return 1

if [[ ${USE} == "doxygen" ]]; then
	make doc
	
	git config --global user.email "gereon.kremer@cs.rwth-aachen.de"
	git config --global user.name "Travis doxygen daemon"
	
	git clone git@github.com:smtrat/smtrat.github.io.git
elif
	make resources || return 1
	make -j4 lib_carl || return 1
	make -j4 || return 1
	make CTEST_OUTPUT_ON_FAILURE=1 test || return 1
fi

cd ../
