#!/bin/bash
#      MultipleFilesDecompression.sh
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


# multiple file decompressing in the given directory
# arguments 
# - directory name

if [ -d $1 ]
then
	cd $1
	for i in `ls | grep .tar`
	do
		#echo $i
		tar xf $i > /dev/null 2>&1
	done
else
	echo -n "There is no input directory"
	exit 2
fi
