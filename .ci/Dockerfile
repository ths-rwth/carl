FROM    ubuntu:rolling

ENV     DEBIAN_FRONTEND noninteractive

RUN     apt-get update ;\
        apt-get install -y apt-utils joe sudo ;\
        apt-get install -y bison clang cmake flex g++ git ;\
		apt-get install -y libboost-all-dev libeigen3-dev libgmp-dev ;\
        useradd -m -s /bin/bash user && echo "user ALL= NOPASSWD: ALL\n" >> /etc/sudoers

USER    user
WORKDIR /home/user

RUN     git clone https://github.com/smtrat/carl.git &&\
        mkdir -p carl/build &&\
        cd carl/build/ &&\
        cmake ../ &&\
        make &&\
        make test
