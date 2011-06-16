//      ReadWriteFile.cpp
//      
//      Copyright 2011 Eduard Bagrov <ebagrov@gmail.com>
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

#include <unistd.h>
#include <fcntl.h>
#include "ReadWriteFile.hpp"
#include "File.hpp"

int ReadWriteFileTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case ReadBadFileDescriptor:
				return ReadBadFileDescriptorTest1();
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status ReadWriteFileTest::ReadBadFileDescriptorTest1()
{
	try
	{
		File file("testfile.txt", S_IWUSR);
		
		char buf[1024];
		size_t count = 10;

		size_t fd = open("testfile.txt", O_WRONLY);

		ssize_t status = read(fd, buf, count);
		if (errno != EBADF)
		{
			cerr << "Expected to get bad file descriptor";
			return Fail;
		}
	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Fail;
	}
	
	return Success;
}

Status ReadWriteFileTest::ReadBadFileDescriptorTest2()
{
	char buf[1024];
	size_t count = 10;

	ssize_t status = read(-1, buf, count);
	if (errno != EBADF)
	{
		cerr << "Expected to get bad file descriptor";
		return Fail;
	}
	
	return Success;
}

