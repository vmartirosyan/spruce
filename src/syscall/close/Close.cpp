//      Close.cpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Gurgen Torozyan <gurgen.torozyan@gmail.com>
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

#include "Close.hpp"
#include <unistd.h>
#include <fcntl.h>
#include "File.hpp"

int Close::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case BadFileDescriptor:
				return CloseBadFileDescriptorTest();
			case CorrectDescriptor:
			   return CloseCorrectDescriptorTest();
			case CorrectDescriptorUnlink:
			   return CloseCorrectDescriptorUnlinkTest();
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status Close::CloseBadFileDescriptorTest()
{

	int fd = -1;
    int ret_val ;
	
	ret_val = close(fd);
	
	if(ret_val == -1 )
	{
	   if(errno	== EBADF)
			return Success;
			
	   cerr << "Closeing bad file descriptor : Close returned -1 but errno is set to  "<<strerror(errno);
	   return Fail; 
	}
	else 
	{
    	cerr << "Closeing bad file descriptor : Close did not return -1";
		return Fail;
	}
	
}



Status Close::CloseCorrectDescriptorTest()
{
	
	
	char buf[1024];
	size_t count = 10;
	int fd ;
	int ret_val;
	
	try
	{
		File file("testfile.txt", S_IWUSR);
		fd = file.GetFileDescriptor();
		
		ret_val = close(fd);
		if(ret_val==0)
		{
			return Success;
		}
		else
		{
			cerr << "Cannot close file: "<<strerror(errno);
			return Fail;
		}

	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Fail;
	}
	
}


Status Close::CloseCorrectDescriptorUnlinkTest()
{
	char buf[1024];
	const char *pathname="testfile.txt";
	int ret_val ;
	int fd = open("testfile.txt", O_CREAT, S_IRUSR);
	
	if( fd == -1)
	{
		cerr << "Cannot create file: Aborting test "<<strerror(errno);
		return Unres;
	}
	
	ret_val=unlink("testfile.txt");
	if(ret_val!=0)
	{
		cerr << "Cannot unlink file: Aborting test "<<strerror(errno);
		return Unres;
	}
	
	ret_val = close(fd);	
	if(ret_val!=0)
	{
		cerr << "Cannot close file: Aborting test "<<strerror(errno);
		return Unres;
	}

	int fd_new = open("testfile2.txt", O_CREAT, S_IRUSR);
	if( fd_new== -1)
	{
		cerr << "Cannot create file: Aborting test "<<strerror(errno);
		return Unres;
	}
	
	
	if(fd_new!=fd)
	{
		cerr << "After closing and unlinking the previous file descriptor ,the new one must refer to the same value(as it is minimal value)";
		return Fail;
	}
	unlink("testfile2.txt");
	return Success;
	
	
	
	
	
	
	
	

}
