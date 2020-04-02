#!/usr/bin/env bash

function fold_start {
	travis_start_time=`date -u '+%s000000000'`
	echo -en "travis_fold:start:$1\r\033[0K"
	echo -en "travis_time:start:timer_$1\r\033[0K"
}
function fold_end {
	local travis_end_time=`date -u '+%s000000000'`
	local duration=$(($travis_end_time-$travis_start_time))
	echo -en "travis_time:end:timer_$1:start=$travis_start_time,finish=$travis_end_time,duration=$duration\r\033[0K"
	echo -en "travis_fold:end:$1\r\033[0K"
}
function fold {
	fold_start "$1"
	eval ${@:2}
	rc=$?
	fold_end "$1"
	return $rc
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
function start_keep_waiting() {
  keep_waiting &
  disown
  keep_waiting_id=$!
}
function stop_keep_waiting() {
  kill $keep_waiting_id
}

if [ -z "$MAKE_PARALLEL" ]; then
	MAKE_PARALLEL="-j2"
fi

if [[ ${TASK} == "dependencies" ]]; then
	
	start_keep_waiting
	fold "build-resources" /usr/bin/time make ${MAKE_PARALLEL} resources || return 1
	stop_keep_waiting
	
elif [[ ${TASK} == "coverity" ]]; then

	start_keep_waiting
	fold "build" /usr/bin/time make ${MAKE_PARALLEL} carl || return 1
	fold "build-tests" /usr/bin/time make ${MAKE_PARALLEL} || return 1
	stop_keep_waiting

elif [[ ${TASK} == "sonarcloud-build" ]]; then
	
	fold "reconfigure" cmake -D COVERAGE=ON ../ || return 1
	
	WRAPPER="build-wrapper-linux-x86-64 --out-dir ../bw-output"
	fold "build" $WRAPPER make ${MAKE_PARALLEL} || return 1
	fold "collect-coverage" make coverage-collect
	
elif [[ ${TASK} == "sonarcloud-scan" ]]; then

	cd ../ && sonar-scanner -X -Dproject.settings=build/sonarcloud.properties && cd build/

elif [[ ${TASK} == "clang-ubsan" ]]; then

	fold "reconfigure" cmake -D CLANG_SANITIZER=undefined ../

	fold "build" /usr/bin/time make ${MAKE_PARALLEL} || return 1
	fold "tests" /usr/bin/time make -j1 CTEST_OUTPUT_ON_FAILURE=1 test || return 1

elif [[ ${TASK} == "documentation" ]]; then
	
	# To allow convert for doc/pictures/
	sudo rm -f /etc/ImageMagick-6/policy.xml
	
	start_keep_waiting
	fold "build-doc" make doc || return 1
	stop_keep_waiting
	
	fold_start "commit"
	git config --global user.email "gereon.kremer@cs.rwth-aachen.de"
	git config --global user.name "Travis documentation daemon"
	
	git clone https://${GH_TOKEN}@github.com/smtrat/smtrat.github.io.git
	cd smtrat.github.io/ || return 1
	git branch -m master old_master
	git checkout --orphan master
	
	# Update cloned copy
	cp -r ../doc/apidoc-html/* carl/ || return 1
	cp ../doc/doc_carl-*.pdf . || return 1
	cp ../doc/doc_carl-*.pdf doc_carl-latest.pdf || return 1
	cp ../doc/manual_carl-*.pdf . || return 1
	cp ../doc/manual_carl-*.pdf manual_carl-latest.pdf || return 1
	
	git add . || return 1
	
	# Commit and push
	git commit -q -m "Updated documentation for carl" || return 1
	git push -f origin master || return 1
	fold_end "commit"

elif [[ ${TASK} == "pycarl" ]]; then
	
	# Create a python virtual environment for local installation
	virtualenv -p python3 pycarl-env
	source pycarl-env/bin/activate
	
	/usr/bin/time make ${MAKE_PARALLEL} carl || return 1
	
	# Clone pycarl
	git clone https://github.com/moves-rwth/pycarl.git
	cd pycarl/ || return 1
	# Build bindings
	python setup.py build_ext -j 1 develop || return 1
	# Run tests
	python setup.py test || return 1
	
elif [[ ${TASK} == "tidy" ]]; then

	fold "build-resources" $WRAPPER make ${MAKE_PARALLEL} resources || return 1
	fold "reconfigure" cmake -D DEVELOPER=ON -D THREAD_SAFE=ON -D USE_BLISS=ON -D USE_CLN_NUMBERS=OFF -D USE_COCOA=ON -D USE_GINAC=OFF ../ || return 1
	
	fold "tidy" /usr/bin/time make tidy

elif [[ ${TASK} == "addons" ]]; then
	
	fold "reconfigure" cmake -D BUILD_ADDONS=ON -D BUILD_ADDON_PARSER=ON -D BUILD_ADDON_PYCARL=ON -D DEVELOPER=ON -D USE_CLN_NUMBERS=ON -D USE_GINAC=ON -D USE_COCOA=ON ../ || return 1
	
	fold "build" /usr/bin/time make ${MAKE_PARALLEL} || return 1
	fold "tests" /usr/bin/time make -j1 CTEST_OUTPUT_ON_FAILURE=1 test || return 1
	
else
	fold "build" /usr/bin/time make ${MAKE_PARALLEL} || return 1
	fold "tests" /usr/bin/time make -j1 CTEST_OUTPUT_ON_FAILURE=1 test || return 1
fi

cd ../
