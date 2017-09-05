#!/usr/bin/env bash

source setup_common.sh

mkdir -p ~/usr
PREFIX=`cd ~/usr; pwd`

if [ ! -f $PREFIX/bin/cmake ]; then
	wget -nv https://cmake.org/files/v3.7/cmake-3.7.2-Linux-x86_64.sh
	chmod +x cmake-3.7.2-Linux-x86_64.sh
	./cmake-3.7.2-Linux-x86_64.sh --prefix=$PREFIX --exclude-subdir --skip-license
fi

export PATH="$PREFIX/bin:$PATH"

cmake --version

export CC=$CC
export CXX=$CXX
