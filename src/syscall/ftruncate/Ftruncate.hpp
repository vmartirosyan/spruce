//      Ftruncate.hpp
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

#ifndef FTRUNCATE_H
#define FTRUNCATE_H

#include <unistd.h>
#include <sys/types.h>
#include "File.hpp"
#include "Directory.hpp"
#include <pwd.h>
#include <linux/limits.h>
#include <limits.h>
#include <sys/stat.h>
#include "SyscallTest.hpp"


// Operations
/*enum FtruncateOps
{	
	FtruncateNormalIncreaseSize,
	FtruncateNormalDecreaseSize,
	FtruncateErrBadF,
	FtruncateErrInval1,
	FtruncateErrInval2	
};*/

class FtruncateTest : public SyscallTest
{			
public:	
	FtruncateTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "ftruncate")
	{			
				
	}
	
	virtual ~FtruncateTest()
	{
	}	
	
	Status NormalIncreaseSize();
	Status NormalDecreaseSize();
	Status ErrBadF();	
	Status ErrInval1();
	Status ErrInval2();

	   
protected:
	virtual int Main(vector<string> args);	
	
};
#endif



