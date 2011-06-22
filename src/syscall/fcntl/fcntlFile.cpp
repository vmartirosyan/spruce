//      fnctl.cpp
//
//      Copyright 2011 Shahzadyan Khachik <qwerity@gmail.com>
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

#include "fcntlFile.hpp"
#include <fcntl.h>

int fcntlTest::Main(vector<string> args)
{
	if ( _mode == Normal )
	{
		switch (_operation)
		{
			case fcntlTestGetFileDescriptorFlags:
				return getFileDescriptorFlags();
			default:
				cerr << "Unsupported operation.";
				return Unres;
		}
	}

	cerr << "Test was successful";

	return Success;
}

Status fcntlTest::getFileDescriptorFlags()
{
	int fd;
	int flags;
	int get_flags;

	try
	{
		flags = O_CREAT | O_WRONLY;
		fd = open("./test", flags);
		
		get_flags = fcntl(fd, F_GETFD);
		
		if(flags == get_flags)
		{
			return Success;
		}
		else
		{
			cerr << "Cannot get file descriptor mode: " << strerror(errno);
			return Fail;
		}
	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Fail;
	}
}