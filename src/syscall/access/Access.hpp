//      Access.hpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Vahram Martirosyan <vmartirosyan@gmail.com>
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

#ifndef ACCESS_H
#define ACCESS_H

#include <sys/types.h>
#include "SyscallTest.hpp"

// Operations
/*enum AccessOps
{
	AccessNormal,
	AccessFileExists,
	AccessErrAccess,
	AccessErrLoop,
	AccessErrNameTooLong,	
	AccessErrNoEnt,	
	AccessErrNotDir	
};*/

class AccessTest : public SyscallTest
{			
public:	
	AccessTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "access")
	{			
				  
		
	}
	virtual ~AccessTest()
	{
	}
	
	Status FileExists();
	Status NormalFunc();
	/*
		The requested access would be denied to the file, or search permission is denied for one of the directories in the path prefix of
		pathname
	*/
	Status ErrAccess();	
	/*
		Too many symbolic links were encountered in resolving pathname.
	*/
	Status ErrLoop();
	/*
		The length of the path argument exceeds {PATH_MAX}
	*/
	Status ErrNameTooLong();
	/*
		 pathname is too long.
	*/	
	Status ErrNoEnt();
	/*
		A component used as a directory in pathname is not, in fact, a directory.
	*/	
	Status ErrNotDir();	
	   
protected:
	virtual int Main(vector<string> args);	
	
};
#endif


