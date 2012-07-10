//      Ftruncate.cpp
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



#include "Ftruncate.hpp"


int FtruncateTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case FtruncateNormalIncreaseSize:
				return NormalIncreaseSize();
			case FtruncateNormalDecreaseSize:
				return NormalDecreaseSize();
			case FtruncateErrBadF:
				return ErrBadF();
			case FtruncateErrInval1:
				return ErrInval1();
			case FtruncateErrInval2:
				return ErrInval2();
			default:
				cerr << "Unsupported operation.";
				return Unsupported;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status FtruncateTest::NormalIncreaseSize()
{
	try
	{	
		File f("some_file");
		int fd = f.GetFileDescriptor();
		
		if ( fd == -1 )
		{
			cerr << "Cannot obtain descriptor. Error: " << strerror(errno);
			return Unres;
		}	
		
		char * buf = (char*)"1234";
		const int length = strlen(buf);
		int length_after_truncate = 5;
		
		if ( write(fd, buf, length) != length )
		{
			cerr << "Cannot write " << length << " bytes to the file. Error: " << strerror(errno);
			return Unres;
		}
		
		if ( ftruncate(fd, length_after_truncate) != 0 )
		{
			cerr << "ftruncate failed. Error: " << strerror(errno);
			return Fail;
		}
		
		// Ensure the file size.
		struct stat stat_buf;
		if ( stat("some_file", &stat_buf) == -1 )
		{
			cerr << "Cannot stat file. Error : " << strerror(errno);
			return Unres;
		}
		
		if ( stat_buf.st_size != length_after_truncate )
		{
			cerr << "Different file size after truncate.";			
			return Fail;
		}
		
		
		// Ensure the trailing NULLs are there.
		char buf_read;		
		if ( read(fd, &buf_read, 1) != 1 )
		{
			cerr << "Cannot read 1 byte from truncated file." << endl;			
			return Fail;
		}
		
		if ( buf_read != '\0' )
		{
			cerr << "After truncation file was not filled with NULLs.";
			return Fail;
		}
		
		return Success;
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}	
}
	
Status FtruncateTest::NormalDecreaseSize()
{
	try
	{	
		File f("some_file");
		int fd = f.GetFileDescriptor();
		
		if ( fd == -1 )
		{
			cerr << "Cannot obtain descriptor. Error: " << strerror(errno);
			return Unres;
		}	
		
		char * buf = (char*)"1234";
		const int length = strlen(buf);
		int length_after_truncate = 1;
		
		if ( write(fd, buf, length) != length )
		{
			cerr << "Cannot write " << length << " bytes to the file. Error: " << strerror(errno);
			return Unres;
		}
		
		if ( ftruncate(fd, length_after_truncate) != 0 )
		{
			cerr << "ftruncate failed. Error: " << strerror(errno);
			return Fail;
		}
		
		struct stat stat_buf;
		if ( stat("some_file", &stat_buf) == -1 )
		{
			cerr << "Cannot stat file. Error : " << strerror(errno);
			return Unres;
		}
		
		if ( stat_buf.st_size != length_after_truncate )
		{
			cerr << "Different file size after truncate.";			
			return Fail;
		}
		
		return Success;
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}	
}

Status FtruncateTest::ErrBadF()
{
	if ( ftruncate(-1, 1) == 0 || errno != EBADF )
	{
		cerr << "ftruncate should return EBADF but it did not. Error: " << strerror(errno);
		return Fail;
	}
	return Success;
}

Status FtruncateTest::ErrInval1()
{
	try
	{	
		File f("some_file", S_IRUSR, O_CREAT | O_RDONLY);
		int fd = f.GetFileDescriptor();
		
		if ( fd == -1 )
		{
			cerr << "Cannot obtain descriptor. Error: " << strerror(errno);
			return Unres;
		}
		
		if ( ftruncate(fd, 1) == 0 || ( ( errno != EBADF ) && ( errno != EINVAL ) ) )
		{
			cerr << "ftruncate should return EBADF or EINVAL but it did not. Error: " << strerror(errno);
			return Fail;
		}
		return Success;
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}	
}

Status FtruncateTest::ErrInval2()
{
	try
	{	
		File f("/dev/stdin", S_IRUSR, O_RDONLY);
		int fd = f.GetFileDescriptor();
		
		if ( fd == -1 )
		{
			cerr << "Cannot obtain descriptor. Error: " << strerror(errno);
			return Unres;
		}
		
		if (ftruncate(fd, 1) == 0 || errno != EINVAL )
		{
			cerr << "ftruncate should return EINVAL but it did not. Error: " << strerror(errno);
			return Fail;
		}
		return Success;
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}	
}
