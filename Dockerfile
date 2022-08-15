FROM            ubuntu:22.04

ENV		DEBIAN_FRONTEND noninteractive

RUN		apt-get update ;\
		apt-get install -y apt-utils joe sudo ;\
		apt-get install -y bison clang cmake flex g++ git ;\
		apt-get install -y libboost libboost-all-dev libeigen3-dev libgmp-dev libreadline libreadline-dev libgmp ;\
		useradd -m -s /bin/bash user && echo "user ALL= NOPASSWD: ALL\n" >> /etc/sudoers

RUN             apt-get install -y libboost-thread-dev

RUN             apt-get install -y readline-common 

RUN             apt-get install -y libboost-filesystem-dev libboost-filesystem1.74.0 libboost-system-dev libboost-system1.74.0 libboost-thread1.74.0

RUN             git config --global user.email "rpgoldman@sift.net" &&\
                git config --global user.name "Robert P. GoldmanYour Name"

COPY	. carl/

RUN		cd carl/ &&\
		mkdir -p build/ &&\
		ls &&\
		cd build/ &&\
		cmake -DUSE_COCOA=ON ../ &&\
		make &&\
		make test
