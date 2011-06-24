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

#include <fcntlFD.hpp>

int fcntlFD::Main(vector<string> args)
{
	if ( _mode == Normal )
	{
		switch (_operation)
		{
			case fcntlFDGetSetFileDescriptorFlags:
				return get_setFileDescriptorFlags();
			case fcntlFDGetSetFileStatusFlags:
				return get_setFileStatusFlags();

			default:
				cerr << "Unsupported operation.";
				return Unres;
		}
	}

	cerr << "Test was successful";

	return Success;
}
Status fcntlFD::get_setFileStatusFlags()
{
	int fd;

	return Success;
}

Status fcntlFD::get_setFileDescriptorFlags()
{
	int fd;
	int set_flags = FD_CLOEXEC;
	int get_flags;

	if(fd = open("test", O_CREAT | O_WRONLY) < 0)
		cerr << "Can't open file\n errno: " << strerror(errno) << endl;

	// set File Descriptor Flags
	fcntl(fd, F_SETFD, set_flags);

	// get File Descriptor Flags
	get_flags = fcntl(fd, F_GETFD);

	close(fd);

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
