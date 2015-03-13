#!/bin/bash

projectFile="build/sgi.pro"

if [ $1 = 'all' ]
then
	qmake $projectFile
	make
elif [ $1 = 'clean' ]
then
	make clean
	rm -f SGI.app
fi