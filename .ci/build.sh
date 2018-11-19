#!/usr/bin/env bash

function fold_start {
	echo -en "travis_fold:start:$1\r"
}
function fold_end {
	echo -en "travis_fold:end:$1\r"
}
function fold {
	fold_start "$1"
	${@:2}
	fold_end "$1"
}

mkdir -p build || return 1
cd build/ || return 1

fold "configure" cmake -D DEVELOPER=ON -D THREAD_SAFE=ON -D USE_BLISS=ON -D USE_CLN_NUMBERS=ON -D USE_COCOA=ON -D USE_GINAC=ON ../ || return 1

function keep_waiting() {
  while true; do
    echo -e "."
    sleep 60
  done
}

if [ -z "$MAKE_PARALLEL" ]; then
	MAKE_PARALLEL="-j2"
fi

if [[ ${TASK} == "dependencies" ]]; then
	
	keep_waiting &
	fold "build-resources" /usr/bin/time make ${MAKE_PARALLEL} resources || return 1
	kill $!
	
elif [[ ${TASK} == "coverity" ]]; then

	keep_waiting &
	fold "build" /usr/bin/time make ${MAKE_PARALLEL} lib_carl || return 1
	fold "build-tests" /usr/bin/time make ${MAKE_PARALLEL} || return 1
	kill $!

elif [[ ${TASK} == "sonarcloud" ]]; then
	
	fold "reconfigure" cmake -D COVERAGE=ON ../ || return 1
	
	WRAPPER="build-wrapper-linux-x86-64 --out-dir ../bw-output"
	fold "build" $WRAPPER make ${MAKE_PARALLEL} lib_carl || return 1
	fold "build-tests" $WRAPPER make ${MAKE_PARALLEL} || return 1
	fold "collect-coverage" make coverage-collect
	
	cd ../ && sonar-scanner -X -Dproject.settings=build/sonarcloud.properties && cd build/
	
elif [[ ${TASK} == "doxygen" ]]; then
	
	fold "reconfigure" cmake -D DOCUMENTATION_CREATE_PDF=ON -D BUILD_DOXYGEN=ON ../
	
	fold "build-doc" make doc || return 1
	
	fold_start "commit"
	git config --global user.email "gereon.kremer@cs.rwth-aachen.de"
	git config --global user.name "Travis doxygen daemon"
	
	git clone https://${GH_TOKEN}@github.com/smtrat/smtrat.github.io.git
	cd smtrat.github.io/ || return 1
	git branch -m master old_master
	git checkout --orphan master
	
	# Update cloned copy
	cp -r ../doc/html/* carl/ || return 1
	cp ../doc/*.pdf . || return 1
	
	git add . || return 1
	
	# Commit and push
	git commit -q -m "Updated documentation for carl" || return 1
	git push -f origin master || return 1
	fold_end "commit"

elif [[ ${TASK} == "pycarl" ]]; then
	
	# Create a python virtual environment for local installation
	virtualenv -p python3 pycarl-env
	source pycarl-env/bin/activate
	
	/usr/bin/time make ${MAKE_PARALLEL} lib_carl || return 1
	
	# Clone pycarl
	git clone https://github.com/moves-rwth/pycarl.git
	cd pycarl/ || return 1
	# Build bindings
	python setup.py build_ext -j 1 develop || return 1
	# Run tests
	python setup.py test || return 1
	
elif [[ ${TASK} == "tidy" ]]; then

	fold "reconfigure" cmake -D DEVELOPER=ON -D THREAD_SAFE=ON -D USE_BLISS=ON -D USE_CLN_NUMBERS=OFF -D USE_COCOA=ON -D USE_GINAC=OFF ../ || return 1
	
	/usr/bin/time make tidy || return 1

elif [[ ${TASK} == "addons" ]]; then
	
	fold "reconfigure" cmake -D BUILD_ADDONS=ON -D BUILD_ADDON_PARSER=ON -D BUILD_ADDON_PYCARL=ON -D DEVELOPER=ON -D USE_CLN_NUMBERS=ON -D USE_GINAC=ON -D USE_COCOA=ON ../ || return 1
	
	/usr/bin/time make ${MAKE_PARALLEL} lib_carl || return 1
	/usr/bin/time make ${MAKE_PARALLEL} || return 1
	/usr/bin/time make -j1 CTEST_OUTPUT_ON_FAILURE=1 test || return 1
	
else
	/usr/bin/time make ${MAKE_PARALLEL} lib_carl || return 1
	/usr/bin/time make ${MAKE_PARALLEL} || return 1
	/usr/bin/time make -j1 CTEST_OUTPUT_ON_FAILURE=1 test || return 1
fi

cd ../
