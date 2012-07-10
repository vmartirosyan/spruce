//      Linkat.cpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Suren Gishyan <gsuren@gmail.com>
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

#include "Linkat.hpp"
#include <unistd.h>
#include <fcntl.h>
#include "File.hpp"

int LinkatTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			
			case LinkatTestIsNotDirectory:
				return LinkatIsNotDirectory();
			case LinkatTestInvalidFileDescriptor:
				return LinkatInvalidDescriptor();
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}




Status LinkatTest::LinkatInvalidDescriptor()
{

	int ret_val;
	const char* filename2="filename2.txt";
	const char* filename="filename.txt";
	
	
	
	try
	{
		File file(filename, S_IWUSR);
		File file2(filename2, S_IWUSR);
		
		ret_val=linkat(-1,filename,-1,filename2,AT_SYMLINK_FOLLOW);
					
				
		if(ret_val==0)
		{
				cerr << "Linkat returned 0 in case of invalid descriptor.";
				return Fail;
		}
		else
		{
			if(errno!=EBADF)
			{
					
					cerr << "Incorrect error set in errno in case of invalid descriptor."<<strerror(errno);
					return Fail;
			}
		}
		
		return Success;
		
	}	
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Fail;
	}

	
}

Status LinkatTest::LinkatIsNotDirectory()
{

	int ret_val;
	const char* notDirectoryPath="filename2.txt/linkname";
	

	
		
		ret_val=linkat(0,notDirectoryPath,0,notDirectoryPath,AT_SYMLINK_FOLLOW);
					
				
		if(ret_val==0)
		{
				cerr << "Linkat returned 0 in case of not correct irectory.";
				return Fail;
		}
		else
		{
			if(errno!=ENOTDIR)
			{
					
					cerr << "Incorrect error set in errno in case of invalid descriptor."<<strerror(errno) ;
					return Fail;
			}
		}
		
		return Success;

	
	
	
	
}

