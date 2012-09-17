//      Umask.cpp
//      
// 		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author: 
// 			Tigran Piloyan <tigran.piloyan@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#include "Umask.hpp"
#include "File.hpp"

int Umask::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case UmaskBasicOp:
				return UmaskBasicFunc();
			default:
				cerr << "Unsupported operation";
				return Unres;
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status Umask::UmaskBasicFunc()
{
	struct stat statbuf;
	mode_t setMask = 0077;
	unsigned mode;
	mode_t retMask;
	bool firstCallSkiped = false;
	
	for(umask(setMask); setMask < 01000; retMask = umask(++setMask))
	{
		if(firstCallSkiped)
		{
			if(retMask != setMask - 1)
			{
				cerr << "Returned by the function mask should be " 
					 << setMask - 1 << ", but it is " << retMask; 
				return Fail;
			}
			
		}
		
		firstCallSkiped = true;
		
		int fd = creat("testfile", 0777);
		if(fd == -1)
		{
			cerr << "Unable to create file\n";
			return Unres;
		}
		
		if(fstat(fd, &statbuf) != 0) 
		{
			cerr << "Cannot fstat file";
			unlink("testfile");
			return Unres;
		}
		
		mode = statbuf.st_mode & 0777;
		if (mode != (~setMask & 0777))
		{
			cerr << "Set by the function mask should be " 
				 << (~setMask & 0777) << ", but it is " << mode;
			unlink("testfile");
			return Fail;
		}
		unlink("testfile");
	}
	
	return Success;
}
