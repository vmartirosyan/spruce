//      Utime.hpp
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

#ifndef UTIME_H
#define UTIME_H

#include <utime.h>
#include <sys/types.h>
#include "SyscallTest.hpp"


// Operations
/*enum UtimeOps
{
	UtimeNormalNotNull,	
	UtimeNormalNull,
	UtimeErrAccess,
	UtimeErrLoop,
	UtimeErrNameTooLong1,
	UtimeErrNameTooLong2,
	UtimeErrNoEnt1,
	UtimeErrNoEnt2,
	UtimeErrNotDir,
	UtimeErrPerm,
};*/

class UtimeTest : public SyscallTest
{			
public:	
	UtimeTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "utime")
	{			
				  
		
	}
	virtual ~UtimeTest()
	{
	}	
	/*
		If  times  is  not  a null pointer, times shall be interpreted as a pointer to a utimbuf structure and the access and modification
		times shall be set to the values contained in the designated structure. Only a process with the effective user ID equal to the user
		ID of the file or a process with appropriate privileges may use  utime() this way. 
	*/
	Status NormallNotNull();	
	/*
		If times is a null pointer, the access and modification times of the file shall be set to the current time. The effective user ID of
		the process shall match  the  owner of the file, or the process has write permission to the file or has appropriate privileges, to
		use utime() in this manner.
	*/
	Status NormallNull();
	/*
		Search  permission  is  denied by a component of the path prefix, the times argument is a null pointer and the effective user ID of
		the process does not match the owner of the file, the process does not have write permission for the file, the process does not have
		appropriate privileges
	*/
	Status ErrAccess();
	
	/*
		A loop exists in symbolic links encountered during resolution of the path argument.
	*/
	Status ErrLoop();
	/*
		The length of the path argument exceeds {PATH_MAX}
	*/
	Status ErrNameTooLong1();
	/*
		 a pathname component is longer than {NAME_MAX}
	*/
	Status ErrNameTooLong2();
	/*
		A component of path does not name an existing file
	*/
	Status ErrNoEnt1();
	/*
		path is an empty string.
	*/
	Status ErrNoEnt2();
	/*
		A component of the path prefix is not a directory.
	*/
	Status ErrNotDir();
	/*
		The times argument is not a null pointer and the calling process' effective user ID does not match the owner of the file and the
		calling process  does  not  have the appropriate privileges.
	*/
	Status ErrPerm();
	   
protected:
	virtual int Main(vector<string> args);	
	
};
#endif


