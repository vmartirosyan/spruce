//      DupFileDescriptor.cpp
//      
//		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//      	Narek Saribekyan <narek.saribekyan@gmail.com>
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
#include "DupFileDescriptor.hpp"
#include "File.hpp"
#include <unistd.h>
#include <errno.h>

int DupFileDescriptorTest::Main(vector<string> args)
{
	// TODO: Write test for dup/dup2/dup3 system call	
	if (_mode == Normal)
	{		
		switch (_operation)
		{		
			case Dup:
				return DupTest(args);
			case Dup2:
				return Dup2Test(args);
			case Dup3:			
				return Dup3Test(args);
			default:
				return Unknown; 
		}
	}
	else
	{		
	}
	return 0;
} 

int DupFileDescriptorTest::DupTest(vector<string> args)
{
	try 
	{		
		File file("newfile");
		int fd = file.GetFileDescriptor();		
		int newfd = dup(fd);
				
		if (newfd == -1)
		{				
			cerr << "System call dup failed: "<< strerror(errno);
			return Fail; 
		}								
	}
	catch (Exception ex) 
	{
		cerr << ex.GetMessage();
		return Unres;
	}	
	return Success;
}

int DupFileDescriptorTest::Dup2Test(vector<string> args)
{
	return 0;
}

int DupFileDescriptorTest::Dup3Test(vector<string> args)
{
	return 0;
}
