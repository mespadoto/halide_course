#!/bin/sh -x

find . -type d -name "*_*" |\
while read d
do
	cd $d
	cmake . -DCMAKE_BUILD_TYPE=Release
	make VERBOSE=1
	cd -
done
