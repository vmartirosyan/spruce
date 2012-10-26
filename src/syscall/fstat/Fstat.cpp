//      Fstat.cpp
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

#include "Fstat.hpp"
#include "File.hpp"
#include "Directory.hpp"

int FstatTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case FstatNormalFunc:
				return NormalFunc();
			case FstatErrBadf:
				return ErrBadf();
			default:
				cerr << "Unsupported operation.";
				return Unsupported;
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status FstatTest::NormalFunc()
{
	try
	{	
		File f("some_file", S_IRUSR, O_CREAT | O_RDONLY);
		
		int fd = f.GetFileDescriptor();
		if ( fd == -1 )
		{
			cerr << "Cannot obtain file descriptor. Error: " << strerror(errno);
			return Unres;
		}
		
		struct stat buf;
		
		if ( fstat(fd, &buf) != 0 )
		{
			cerr << "fstat failed. Error : " << strerror(errno);
			return Fail;
		}
		
		if ( ( buf.st_mode & 0777 ) != S_IRUSR )
		{
			cerr << "fstat returned another file mode.";
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
Status FstatTest::ErrBadf()
{
	struct stat buf;
	if ( fstat(-1, &buf) != -1 && errno != EBADF )
	{
		cerr << "fstat should return EBADF but did not. Error : " << strerror(errno);
		return Fail;
	}
	return Success;
}

