#!/usr/bin/env bash

git fetch --unshallow

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then

	mkdir -p ~/usr
	PREFIX=`cd ~/usr; pwd`

	if [ ! -f $PREFIX/bin/cmake ]; then
		wget -nv https://cmake.org/files/v3.7/cmake-3.7.2-Linux-x86_64.sh
		chmod +x cmake-3.7.2-Linux-x86_64.sh
		./cmake-3.7.2-Linux-x86_64.sh --prefix=$PREFIX --exclude-subdir --skip-license
	fi
	if [ ! -f $PREFIX/bin/doxygen ]; then
		wget -nv http://ftp.stack.nl/pub/users/dimitri/doxygen-1.8.13.linux.bin.tar.gz
		tar -xzf doxygen-1.8.13.linux.bin.tar.gz -C $PREFIX/bin/
		cp $PREFIX/bin/doxygen-*/bin/* $PREFIX/bin/
	fi

	export PATH="$PREFIX/bin:$PATH"

elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then

	brew update --quiet
	brew install doxygen llvm

	if [[ ${CXX} == "g++-5" ]]; then
		brew install gcc@5
		brew link --overwrite gcc@5
	elif [[ ${CXX} == "g++-6" ]]; then
		brew install gcc@6
		brew link --overwrite gcc@6
	fi

fi

export CC=$CC
export CXX=$CXX
