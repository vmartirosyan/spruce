#!/bin/bash
#      MultipleFilesCreation.sh
#      
# 		Copyright (C) 2011, Institute for System Programming
#                          of the Russian Academy of Sciences (ISPRAS)
#
#      Author: Eduard Bagrov <ebagrov@gmail.com>
#      
#      This program is free software; you can redistribute it and/or modify
#      it under the terms of the GNU General Public License as published by
#      the Free Software Foundation; either version 2 of the License, or
#      (at your option) any later version.
#      
#      This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#      GNU General Public License for more details.
#      
#      You should have received a copy of the GNU General Public License
#      along with this program; if not, write to the Free Software
#      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#      MA 02110-1301, USA.


# creates files with fixed size filled with zeros and place them into folder
# arguments
# - number of files to create
# - size of files to create
# - folder to store the files in
# 'size' can be followed by the following multiplicative suffixes:
# c = 1, w = 2,  b = 512, kB = 1000, K = 1024, MB = 1000*1000, M = 1024*1024, 
# xM = M GB = 1000*1000*1000, G = 1024*1024*1024, and so on for T, P, E, Z, Y.

# creating 'number' of files
number=$1

parent_folder=$3
mkdir -p $parent_folder
cd $parent_folder
for ((i = 0; i < $number; i++))
do
    touch $i
done

# filling the existing files with zeros
size=$2
for ((i = 0; i < $number; i++))
do
	$INSTALL_PREFIX/bin/createfile $i $size
done
