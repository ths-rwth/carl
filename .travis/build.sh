#!/usr/bin/env bash

mkdir build || return 1
cd build/ || return 1
cmake -D DEVELOPER=ON ../ || return 1

if [[ ${USE} == "doxygen" ]]; then
	make doc
	
	git config --global user.email "gereon.kremer@cs.rwth-aachen.de"
	git config --global user.name "Travis doxygen daemon"
	
	git clone 219fc41efb80a7a8f102f5ca9147baf58514d734@github.com:smtrat/smtrat.github.io.git
	cd smtrat.github.io.git/
	
	git rm -rf carl/
	mkdir -p carl/
	cp doc/html/* carl/
	git add carl/
	git commit -m "Updated documentation for carl"
	git push origin master

else
	make resources || return 1
	make -j4 lib_carl || return 1
	make -j4 || return 1
	make CTEST_OUTPUT_ON_FAILURE=1 test || return 1
fi

cd ../
