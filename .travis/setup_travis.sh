#!/usr/bin/env bash

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then

	source setup_ubuntu1404.sh

elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then

	source setup_osx.sh

fi
