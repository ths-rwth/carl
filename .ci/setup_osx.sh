#!/usr/bin/env bash

source setup_common.sh


function install {
	brew install $*
}

brew update --quiet
brew install cln doxygen eigen llvm

if [[ ${CXX} == "g++-5" ]]; then
	install gcc@5
	brew link --overwrite gcc@5
elif [[ ${CXX} == "g++-6" ]]; then
	install gcc@6
	brew link --overwrite gcc@6
fi

export CC=$CC
export CXX=$CXX
