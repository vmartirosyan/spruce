#!/bin/bash

# searches file in the directory
# arguments 
# - directory name
# - file name to search

if [ -d $1 ]
then
	for i in `find . -name $2`
	do
		if [ -f $i ]
		then
			echo "file $2 found in" $i
		else
			echo "file $2 not found"
		fi
	done
else
	echo "There is no input folder which is expected to be compressed"
fi
