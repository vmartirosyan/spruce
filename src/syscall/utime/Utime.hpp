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

#include <sys/types.h>
#include "SyscallTest.hpp"


// Operations
enum UtimeOps
{
	UtimeNormalNotNull,
	UtimeNormalNull,
	UtimeErrAccess1,
	UtimeErrAccess2,
	UtimeErrAccess3,
	UtimeErrAccess4,
	UtimeErrLoop,
	UtimeErrNameTooLong1,
	UtimeErrNameTooLong2,
	UtimeErrNoEnt1,
	UtimeErrNoEnt2,
	UtimeErrNotDir,
	UtimeErrPerm,
};

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
	Status NormallNotNull();
	Status NormallNull();
	Status ErrAccess1();
	Status ErrAccess2();
	Status ErrAccess3();
	Status ErrAccess4();
	Status ErrLoop();
	Status ErrNameTooLong1();
	Status ErrNameTooLong2();
	Status ErrNoEnt1();
	Status ErrNoEnt2();
	Status ErrNotDir();
	Status ErrPerm();
	   
protected:
	virtual int Main(vector<string> args);	
	
};
#endif


