#!/bin/bash
#      FindFile.sh
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
			echo -n "file $2 found in" $i
		else
			echo -n "file $2 not found"
			exit 1
		fi
	done
else
	echo "There is no input folder which is expected to be compressed"
	exit 2
fi
