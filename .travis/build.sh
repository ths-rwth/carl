#!/usr/bin/env bash

mkdir build || return 1
cd build/ || return 1
cmake -D DEVELOPER=ON -D USE_CLN_NUMBERS=ON ../ || return 1

if [[ ${TASK} == "doxygen" ]]; then
	make doc || return 1
	
	git config --global user.email "gereon.kremer@cs.rwth-aachen.de"
	git config --global user.name "Travis doxygen daemon"
	
	git clone https://${GH_TOKEN}@github.com/smtrat/smtrat.github.io.git
	cd smtrat.github.io/ || return 1
	
	# Update cloned copy
	cp ../doc/html/* carl/ || return 1
	# Check if something has changed
	git diff --summary --exit-code && return 0
	git add carl/ || return 1
	# Commit and push
	git commit -m "Updated documentation for carl" || return 1
	git push origin master || return 1

else
	/usr/bin/time make resources -j1 || return 1
	/usr/bin/time make -j1 lib_carl || return 1
	/usr/bin/time make -j1 || return 1
	/usr/bin/time make -j1 CTEST_OUTPUT_ON_FAILURE=1 test || return 1
fi

cd ../
