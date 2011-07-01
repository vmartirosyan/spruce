//      fnctl.cpp
//
// 		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//
//      Author: Shahzadyan Khachik <qwerity@gmail.com>
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

#include <fcntlFD.hpp>
#include <string.h>
#include <stdlib.h>

int fcntlFD::Main(vector<string> args)
{
	if ( _mode == Normal )
	{
		switch (_operation)
		{
			case fcntlFDGetSetFileDescriptorFlags:
				return get_setFileDescriptorFlags();

			case fcntlFDGetSetFileStatusFlagsIgnore:
				return get_setFileStatusFlagsIgnore();

			case fcntlFDGetSetFileStatusFlagsIgnoreRDONLY:
				return get_setFileStatusFlagsIgnoreRDONLY();

			case fcntlFDGetSetFileStatusFlags:
				return get_setFileStatusFlags();

			case fcntlFDDupFileDescriptor:
				return dupFileDescriptor();

			default:
				cerr << "Unsupported operation.";
				return Unres;
		}
	}

	cerr << "Test was successful";

	return Success;
}

Status fcntlFD::dupFileDescriptor()
{
	int fd;
	int new_fd;
	const char buff[6] = "apero";
	char* nbuff = new char[6];
	long _arg = rand()%99;

	if((fd = open("test", O_CREAT | O_RDWR)) < 0)
		cerr << "Can't open file\n errno: " << strerror(errno) << endl;

	new_fd = fcntl(fd, F_DUPFD, _arg);

	write(fd, &buff, 5);
	lseek(fd, 0, SEEK_SET);
	read(new_fd, nbuff, 5);

	close(fd);
	close(new_fd);
	unlink("test");

	if(new_fd == EINVAL)
	{
		cerr << "arg is negative or is greater than the maximum allowable value." << endl;
		return Fail;
	}

	if(new_fd == EMFILE)
	{
		cerr << "the process already has the maximum number of file descriptors open" << endl;
		return Fail;
	}

	if((strcmp(buff, nbuff) != 0) || (new_fd < _arg))
	{
		delete [] nbuff;
		cerr << "Return wrong file descriptor";
		return Fail;
	}

	return Success;
}

Status fcntlFD::get_setFileStatusFlagsIgnore()
{
	int fd;
	long set_flags;
	long get_flags;

	if((fd = open("test", O_CREAT)) < 0)
		cerr << "Can't open file\n errno: " << strerror(errno) << endl;

	// set File Descriptor Flags
	set_flags = O_WRONLY | O_RDWR | O_CREAT | O_EXCL | O_NOCTTY | O_TRUNC;
	fcntl(fd, F_SETFL, set_flags);

	// get File Descriptor Flags
	get_flags = fcntl(fd, F_GETFL);

	close(fd);
	unlink("test");

	if(set_flags & get_flags != 0)
	{
		cerr << "Must Ignore setted flags, but does not!: " << strerror(errno);
		return Fail;
	}

	return Success;
}

Status fcntlFD::get_setFileStatusFlagsIgnoreRDONLY()
{
	int fd;
	long set_flags;
	long get_flags;

	if((fd = open("test", O_CREAT)) < 0)
		cerr << "Can't open file\n errno: " << strerror(errno) << endl;

	// set File Descriptor Flags
	set_flags = O_RDONLY;
	fcntl(fd, F_SETFL, set_flags);

	// get File Descriptor Flags
	get_flags = fcntl(fd, F_GETFL);

	close(fd);
	unlink("test");

	if(set_flags & get_flags != 0)
	{
		cerr << "Must Ignore setted flags, but does not!: " << strerror(errno);
		return Fail;
	}

	return Success;
}

Status fcntlFD::get_setFileStatusFlags()
{
	int fd;
	long set_flags;
	long get_flags;

	if((fd = open("test", O_CREAT)) < 0)
	{
		cerr << "Can't open file\n errno: " << strerror(errno) << endl;
	}

	// set File Descriptor Flags
	set_flags = O_APPEND | O_DIRECT | O_NOATIME | O_NONBLOCK;
	fcntl(fd, F_SETFL, set_flags);

	// get File Descriptor Flags
	get_flags = fcntl(fd, F_GETFL);

	close(fd);
	unlink("test");

	if(set_flags != get_flags)
	{
		cerr << "Can't get right file descriptor flags: " << strerror(errno);
		return Fail;
	}

	return Success;
}

Status fcntlFD::get_setFileDescriptorFlags()
{
	int fd;
	long set_flags = FD_CLOEXEC;
	long get_flags;

	if(fd = open("test", O_CREAT | O_WRONLY) < 0)
		cerr << "Can't open file\n errno: " << strerror(errno) << endl;

	// set File Descriptor Flags
	fcntl(fd, F_SETFD, set_flags);

	// get File Descriptor Flags
	get_flags = fcntl(fd, F_GETFD);

	close(fd);
	unlink("test");

	if(FD_CLOEXEC == get_flags)
	{
		return Success;
	}
	else
	{
		cerr << "Cannot get right file descriptor flags: " << strerror(errno);
		return Fail;
	}
}
