#!/bin/bash

# multiple file compressing in the given directory
# arguments 
# - directory name

if [ -d $1 ]
then
	cd $1
	for i in `ls`
	do
		tar czf $i.tar $i
	done
else
	echo "There is no input folder, in which the files should be compressed"
fi

