#!/usr/bin/env bash

mkdir -p build || return 1
cd build/ || return 1
cmake -D DEVELOPER=ON -D USE_BLISS=ON -D USE_COCOA=ON  ../ || return 1

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

if [[ ${TASK} == "dependencies" ]]; then
	
	start_keep_waiting
	/usr/bin/time make ${MAKE_PARALLEL} resources || return 1
	stop_keep_waiting
	
elif [[ ${TASK} == "documentation" ]]; then
	
	# To allow convert for doc/pictures/
	if ! command -v sudo &> /dev/null
	then
		rm -f /etc/ImageMagick-6/policy.xml
	else
		sudo rm -f /etc/ImageMagick-6/policy.xml
	fi
	
	start_keep_waiting
	make doc || return 1
	stop_keep_waiting
	
	git config --global user.email "admin@ths.rwth-aachen.de"
	git config --global user.name "Documentation daemon"
	
	git clone https://${GH_TOKEN}@github.com/ths-rwth/ths-rwth.github.io.git
	cd ths-rwth.github.io/ || return 1
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

elif [[ ${TASK} == "tidy" ]]; then

	/usr/bin/time make ${MAKE_PARALLEL} tidy || return 1

elif [[ ${TASK} == "parallel" ]]; then
	/usr/bin/time make ${MAKE_PARALLEL} || return 1
else
	/usr/bin/time make || return 1
fi