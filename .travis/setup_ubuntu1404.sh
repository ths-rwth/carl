#!/usr/bin/env bash

source setup_common.sh

function install {
	sudo apt-get -qq install --force-yes $*
}

sudo apt-key adv --keyserver keyserver.ubuntu.com --recv 1397BC53640DB551
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt-get -qq update

install cmake doxygen libboost1.55-all-dev libcln-dev libeigen3-dev

cmake --version

if [[ ${USE} == "g++-4.8" ]]; then
	install gcc-4.8 g++-4.8
	defCXX gcc-4.8 g++-4.8
elif [[ ${USE} == "g++-4.9" ]]; then
	install gcc-4.9 g++-4.9
	defCXX gcc-4.9 g++-4.9
elif [[ ${USE} == "g++-5" ]]; then
	install gcc-5 g++-5
	defCXX gcc-5 g++-5
elif [[ ${USE} == "g++-6" ]]; then
	install gcc-6 g++-6
	defCXX gcc-6 g++-6
elif [[ ${USE} == "clang++-3.4" ]]; then
	install clang-3.4 libc++-dev
	defCXX clang clang++
	export CXXFLAGS="-stdlib=libc++"
elif [[ ${USE} == "clang++-3.5" ]]; then
	install clang-3.5 libc++-dev
	defCXX clang-3.5 clang++-3.5
	export CXXFLAGS="-stdlib=libc++"
elif [[ ${USE} == "clang++-3.6" ]]; then
	sudo add-apt-repository -y "deb http://llvm.org/apt/precise/ llvm-toolchain-precise-3.6 main"
	sudo apt-get -qq update
	install clang-3.6 libc++-dev
	defCXX clang-3.6 clang++-3.6
elif [[ ${USE} == "clang++-3.7" ]]; then
	sudo add-apt-repository -y "deb http://llvm.org/apt/precise/ llvm-toolchain-precise-3.7 main"
	sudo apt-get -qq update
	install clang-3.7 libc++-dev
	defCXX clang-3.7 clang++-3.7
elif [[ ${USE} == "clang++-3.8" ]]; then
	sudo add-apt-repository -y "deb http://llvm.org/apt/precise/ llvm-toolchain-precise-3.8 main"
	sudo apt-get -qq update
	install clang-3.8 libc++-dev
	defCXX clang-3.8 clang++-3.8
fi

sudo service postgresql stop
sudo service mysql stop
sudo service cron stop
