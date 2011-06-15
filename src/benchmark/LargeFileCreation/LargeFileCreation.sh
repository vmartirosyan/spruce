#!/bin/bash
#      LargeFileCreation.sh
#      
#      Copyright 2011 Eduard Bagrov <ebagrov@gmail.com>
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


# creates large file with fixed size filled with zeros
# arguments
# - name of file to create
# - size of file to create
# 'size' can be followed by the following multiplicative suffixes:
# c = 1, w = 2,  b = 512, kB = 1000, K = 1024, MB = 1000*1000, M =1024*1024, 
# xM = M GB = 1000*1000*1000, G = 1024*1024*1024, and so on for T, P, E, Z, Y.

parent_folder="results"
mkdir -p $parent_folder
cd $parent_folder
touch $1

# filling the existing file with zeros
./../createfile $2

