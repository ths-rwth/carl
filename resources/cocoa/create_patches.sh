#!/bin/sh

BASEDIR=$1

echo "Using $BASEDIR as a reference."

for f in `find ./ -type f`
do
	if [ -a "$BASEDIR/$f" ]
	then
		echo "Creating patch for $f..."
		diff $BASEDIR/$f $f > $f.patch
	fi
done
