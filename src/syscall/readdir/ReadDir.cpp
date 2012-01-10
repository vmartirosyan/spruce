//     ReadDir.cpp
//
// 		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//
//      Author:  Ani Tumanyan <ani.tumanyan92@gmail.com>
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

#include <ReadDir.hpp>

#include <File.hpp>
#include <sys/types.h>
#include <sys/param.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int ReadDirTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case ReadDirBadFileDesc1: 
				return ReadDirTestBadFileDesc1Func();
			case ReadDirBadFileDesc2:
				return ReadDirTestBadFileDesc2Func();
			case ReadDirIsNotDirect:
				return ReadDirTestIsNotDirectFunc();
			case ReadDirBadAddress:
				return ReadDirTestBadAddressFunc();
			
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

struct linux_direct {
	 long           d_ino;
     off_t          d_off;
     unsigned short d_reclen;
     char           d_name[];
};

Status ReadDirTest:: ReadDirTestBadFileDesc1Func()
{
	char buf[1024];

	if ( syscall( SYS_readdir, -1, buf, 0 ) != -1 )
	{
		cerr << "returns 0 in case of bad file descriptor: "<<strerror(errno);
		return Fail;
	}
	if ( errno != EBADF )
	{
		cerr << "Incorrect error set in errno in case of bad file descriptor: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

Status ReadDirTest:: ReadDirTestBadFileDesc2Func()
{
	char buf[1024];
	const char *dirname = "directory";
	int fd;
	
	if ( mkdir( dirname, 0777 ) == -1 )
	{
		cerr << "Error in mkdir: "<<strerror(errno);
		return Unres;
	}
	
	if ( (fd = open( dirname, O_RDONLY | O_DIRECTORY )) == -1 )
	{
		cerr << "Error in opening and creating directory: "<<strerror(errno);
		return Unres;
	}
	
	if ( close( fd ) == -1 )
	{
		cerr << "Error in closing directory: "<<strerror(errno);
		return Unres;
	}
	
	if ( syscall( SYS_readdir, fd, buf, 0 ) != -1 )
	{
		cerr << "returns 0 in case of bad file descriptor: "<<strerror(errno);
		return Fail;
	}
	
	if ( rmdir( dirname ) == -1 )
	{
		cerr << "Error in removing directory: "<<strerror(errno);
		return Unres;
	}
	
	return Success;
}

Status ReadDirTest:: ReadDirTestBadAddressFunc()
{
	const char* dirname = "directory";
	int fd;
	
	if ( mkdir( dirname, 0777 ) == -1 )
	{
		cerr << "Error in creating directory: "<<strerror(errno);
		return Unres;
	}
	if ( (fd = open( dirname, O_RDONLY | O_DIRECTORY )) == -1 )
	{
		cerr << "Error in opening directory: "<<strerror(errno);
		return Unres;
	}
	
	if ( syscall( SYS_readdir, fd, -1, 0 ) != -1 )
	{
		cerr << "returns 0 in case of bad address. ";
		return Fail;
	}
	
	if ( errno != EFAULT )
	{
		cerr << "Incorrect error set in errno in case of bad address "<<strerror(errno);
		return Fail;
	}
	
	if ( rmdir ( dirname ) == -1 )
	{
		cerr << "Error in removong directory: "<<strerror(errno);
		return Unres;
	}
	return Success;
}

Status ReadDirTest:: ReadDirTestIsNotDirectFunc()
{
	int fd;
	const char *filename = "filename";
	char buf[1024];
	try
	{
		File file( filename );
		fd = file.GetFileDescriptor();
		if ( syscall( SYS_readdir, fd, buf, 0 ) != -1 )
		{
			cerr << "returns 0 in case of is not directory ";
			return Fail;
		}
		if ( errno != ENOTDIR )
		{
			cerr << "Incorrect error set in errno in case of "<<strerror(errno);
			return Fail;
		}
	}
	catch( Exception ex )
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	return Success;
}

