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
#include <Directory.hpp>
#include <sys/types.h>
#include <sys/param.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int ReadDirTest::Main(vector<string>)
{
#if __WORDSIZE==64		
	cerr << "readdir is not supported on x86-64 architecture";		
	return Unsupported;		
#else
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
			case ReadDirBadAddress1:
				return ReadDirTestBadAddress1Func();
			case ReadDirBadAddress2:
				return ReadDirTestBadAddress2Func();
			case ReadDirNoSuchDir:
				return ReadDirTestNoSuchDirFunc();
			case ReadDirNormalCase:
				return ReadDirTestNormalCaseFunc();
			
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
#endif
}

#if __WORDSIZE!=64
struct old_linux_dirent {
	long           d_ino;
	off_t          d_off;
	unsigned short d_reclen;
	char           d_name[];
};

//EBADF
//case 1: setting negative directory descriptor
Status ReadDirTest:: ReadDirTestBadFileDesc1Func()
{
	old_linux_dirent direct;

	if ( syscall( SYS_readdir, -1, &direct, 0 ) != -1 )
	{
		cerr << "returns success in case of bad file descriptor: "<<strerror(errno);
		return Fail;
	}
	if ( errno != EBADF )
	{
		cerr << "Incorrect error set in errno in case of bad file descriptor: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

//EBADF
//case 2: setting opened - closed directory descriptor
Status ReadDirTest:: ReadDirTestBadFileDesc2Func()
{
	old_linux_dirent direct;
	const char *dirname = "directory";
	int fd;
	
	if ( mkdir ( dirname, 0777 ) == -1 )
	{
		cerr << "Error in creating directory: "<<strerror(errno);
		return Unres;
	}
	if ( (fd = open( dirname, O_RDONLY | O_DIRECTORY )) == -1 )
	{
		cerr << "Error in opening directory: "<<strerror(errno);
		return Unres;
	}
	if ( close( fd ) == -1 )
	{
		cerr << "Error in closing directory: "<<strerror(errno);
		return Unres;
	}
	
	if ( syscall( SYS_readdir, fd, &direct, 0 ) != -1 )
	{
		cerr << "returns success in case of bad file descriptor: "<<strerror(errno);
		return Fail;
	}
	
	if ( rmdir( dirname ) == -1 )
	{
		cerr << "Error in removing directory: "<<strerror(errno);
		return Unres;
	}
	

	
	return Success;
}

//EFAULT
//case 1: setting negative value to the pointer
Status ReadDirTest:: ReadDirTestBadAddress1Func()
{
	const char *dirname = "dirname";
	try
	{
		Directory direct( dirname );
		int fd = direct.GetdirectoryDescriptor();
		
		//setting invalid(negative) value to the pointer 
		if ( syscall( SYS_readdir, fd, -1, 0 ) != -1 )
		{
			cerr << "returns success in case of bad address. ";
			return Fail;
		}
	
		if ( errno != EFAULT )
		{
			cerr << "Incorrect error set in errno in case of bad address "<<strerror(errno);
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

//EFAULT
//case 2: setting NULL-pointer
Status  ReadDirTest:: ReadDirTestBadAddress2Func()
{
	const char *dirname = "directory";
	try
	{
		Directory direct( dirname );
		int fd = direct.GetdirectoryDescriptor();
		
		if ( syscall( SYS_readdir, fd, NULL, 0 ) != -1 )
		{
			cerr << "returns success in case of bad address. ";
			return Fail;
		}
		if ( errno != EFAULT )
		{
			cerr << "Incorrect error set in errno in case of bad address: "<<strerror(errno);
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


//ENOTDIR
//setting ordinary file descriptor
Status ReadDirTest:: ReadDirTestIsNotDirectFunc()
{
	int fd;
	const char *filename = "filename";
	old_linux_dirent direct;
	
	try
	{
		File file( filename );
		fd = file.GetFileDescriptor();
		if ( syscall( SYS_readdir, fd, &direct, 0 ) != -1 )
		{
			cerr << "returns success in case of is not directory ";
			return Fail;
		}
		if ( errno != ENOTDIR )
		{
			cerr << "Incorrect error set in errno in case of is not directory: "<<strerror(errno);
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

//ENOENT
//setting removed directory descriptor
Status ReadDirTest:: ReadDirTestNoSuchDirFunc()
{
	old_linux_dirent direct;
	int fd;
	const char *dirname = "dirname";
	
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
	
	if ( rmdir( dirname ) == -1 )
	{
		cerr << "Error in removing directory: "<<strerror(errno);
		return Unres;
	}
	
	if ( syscall( SYS_readdir, fd, &direct, 0 ) != -1 )
	{
		cerr << "returns successs in case of no such  directroy. ";
		return Fail;
	}
	if ( errno != ENOENT )
	{
		cerr << "Incorrect error set in errno on case of no such directory: "<<strerror(errno);
		return Fail;
	}
	
	return Success;
} 

Status ReadDirTest:: ReadDirTestNormalCaseFunc()
{
	old_linux_dirent dir;
	int fd;
	const char *dirname = "dirname";
	
	try
	{
		Directory direct( dirname );
		fd = direct.GetdirectoryDescriptor();
		
		if ( syscall( SYS_readdir, fd, &dir, 0 ) == -1 )
		{
			cerr << " Readdir failed. ";
			return Fail;
		}
	
		if ( strcmp(dir.d_name, ".") != 0 || dir.d_reclen != 1 )
		{
			cerr << "Readdir failed. ";
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
#endif
