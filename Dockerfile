FROM            rpgoldman/cocoalib:99800

ENV		DEBIAN_FRONTEND noninteractive

RUN		apt-get update && apt-get install -y clang-tidy clang-format

RUN		useradd -m -s /bin/bash user && echo "user ALL= NOPASSWD: ALL\n" >> /etc/sudoers

RUN             apt list --installed

RUN             git config --global user.email "rpgoldman@sift.net" &&\
                git config --global user.name "Robert P. Goldman"

COPY	. carl/

# These are necessary because the CoCoA configure script is broken on Ubuntu
# ENV             COCOA_EXTLIB_DIR=/carl/build/resources/src/boost

ENV             BOOST_LIB_DIR=/carl/build/resources/src/boost/lib

RUN		cd carl/ &&\
                mkdir -p src/boost &&\
                cd src/boost && ln -s /usr/lib/aarch64-linux-gnu/ lib &&\
                ln -s /usr/include/boost/ include

RUN             cd /carl &&\
		mkdir -p build/ &&\
		cd build/ &&\
		cmake -DUSE_COCOA=ON -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON .. &&\
		make
		# # make test
