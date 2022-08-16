FROM            ubuntu:22.04

ENV		DEBIAN_FRONTEND noninteractive

RUN		apt-get update

RUN             apt-get install -y apt-utils joe sudo \
                                   bison clang cmake flex g++ git \
                                   libboost-all-dev libeigen3-dev libgmp-dev libreadline8 libreadline-dev libgmp10 \
                                   libboost-thread-dev readline-common \
                                   libboost-filesystem-dev libboost-filesystem1.74.0 libboost-system-dev libboost-system1.74.0 libboost-thread1.74.0 \
                                   libboost-program-options-dev libboost-timer-dev libboost-test-dev libboost-program-options1.74 libboost-timer1.74 libboost-test1.74

RUN		useradd -m -s /bin/bash user && echo "user ALL= NOPASSWD: ALL\n" >> /etc/sudoers

RUN             apt list --installed

RUN             git config --global user.email "rpgoldman@sift.net" &&\
                git config --global user.name "Robert P. GoldmanYour Name"

COPY	. carl/

# These are necessary because the CoCoA configure script is broken on Ubuntu
ENV             COCOA_EXTLIB_DIR=/carl/build/resources/src/boost

ENV             BOOST_LIB_DIR=/carl/build/resources/src/boost/lib

RUN		cd carl/ &&\
                mkdir -p src/boost &&\
                cd src/boost && ln -s /usr/lib/aarch64-linux-gnu/ lib &&\
                ln -s /usr/include/boost/ include &&\
                cd /carl &&\
		mkdir -p build/ &&\
		ls &&\
		cd build/ &&\
		cmake -DUSE_COCOA=ON ../ # &&\
		make #&&\
		# make test
