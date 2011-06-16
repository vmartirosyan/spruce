#      LargeFileCopyTest.sh
#      
#      Copyright 2011 Tigran Piloyan <tigran.piloyan@gmail.com>
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

#!/bin/bash

# Copying the large file to the given directory given number of times

if [ -d $1 ]
then
	cd $1
	for i in `seq 1 $3` 
	do 
		cp $2 $i
	done
else
	echo -n "The directory you specified does not exist."
	exit 2
fi
