//      Fstatfs.cpp
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

#include <Fstatfs.hpp>
#include "File.hpp"
#include "Directory.hpp"

int FstatfsTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case FstatfsNormalFunc:
				return NormalFunc();
			case FstatfsErrBadf:
				return ErrBadf();
			default:
				cerr << "Unsupported operation.";
				return Unsupported;
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status FstatfsTest::NormalFunc()
{
	try
	{	
		if ( FileSystem == "" )
		{
			cerr << "FileSyste env variables is not defined";
			return Unres;
		}
		
		if ( FileSystemTypesMap[FileSystem] == -1 )
		{
			cerr << "fstatfs is not supported by " << FileSystem;
			return Unsupported;
		}
		
		File f("some_file");
		int fd = f.GetFileDescriptor();
		
		if ( fd == -1 )
		{
			cerr << "Cannot obtain file descriptor. Error: " << strerror(errno);
			return Unres;
		}
		
		struct statfs buf;
		
		if ( fstatfs(fd, &buf) != 0 )
		{
			cerr << "fstatfs failed. Error: " << strerror(errno);
			return Fail;
		}
		
		if ( buf.f_type != FileSystemTypesMap[FileSystem] )
		{
			cerr << "fstatfs returned wrong FS type: " << buf.f_type;
			return Fail;
		}
		return Success;
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}	
	return Unsupported;
}
Status FstatfsTest::ErrBadf()
{
	if ( fstatfs(-1, NULL) != -1 && errno != EBADF )
	{
		cerr << "fstatfs should return EBADF but did not. Error : " << strerror(errno);
		return Fail;
	}
	return Success;
}

