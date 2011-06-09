#!/bin/bash

# Deleting the specified directory

if [ -d $1 ]
then 
	rm -rf $1
else
	echo "The directory you specified does not exist."
fi
