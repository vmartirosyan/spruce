#!/bin/bash

# 'number' ($1) - number of files to create
# 'size' ($2) - sizeof files to create
# 'size' can be followed by the following multiplicative suffixes:
# c = 1, w = 2,  b = 512, kB = 1000, K = 1024, MB = 1000*1000, M =1024*1024, 
# xM = M GB = 1000*1000*1000, G = 1024*1024*1024, and so on for T, P, E, Z, Y.

# creating "number" of files
number=$1

parent_folder="results"
mkdir -p $parent_folder
cd $parent_folder
for ((i = 0; i < $number; i++))
do
    touch $i
done

# filling the existing files with the help of random number generator

size=$2
for ((i = 0; i < $number; i++))
do
	dd if=/dev/urandom of=$i bs=$size count=1
done
