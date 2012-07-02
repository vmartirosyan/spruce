//      Truncate.cpp
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



#include <Truncate.hpp>


int TruncateTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case TruncateNormalIncreaseSize:
				return NormalIncreaseSize();
			case TruncateNormalDecreaseSize:
				return NormalDecreaseSize();
			case TruncateErrAccess:
				return ErrAccess();
			case TruncateErrFault:
				return ErrFault();
			case TruncateErrFBig:
				return ErrFBig();
			case TruncateErrInval:
				return ErrInval();
			case TruncateErrIsDir:
				return ErrIsDir();
			case TruncateErrLoop:
				return ErrLoop();
			case TruncateErrNameTooLong:
				return ErrNameTooLong();			
			case TruncateErrNoEnt:
				return ErrNoEnt();
			case TruncateErrNotDir:
				return ErrNotDir();			
			case TruncateErrTxtBsy:
				return ErrTxtBsy();
			default:
				cerr << "Unsupported operation.";
				return Unsupported;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status TruncateTest::NormalIncreaseSize()
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
		
		if ( truncate("some_file", length_after_truncate) != 0 )
		{
			cerr << "truncate failed. Error: " << strerror(errno);
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
		
		
		// Ensuze the trailing NULLs are there.
		char * buf_read = new char[length_after_truncate];
		if ( read(fd, buf_read, length_after_truncate) != length_after_truncate )
		{
			cerr << "Cannot read " << length_after_truncate << " bytes from truncated file";
			return Fail;
		}
		
		if ( buf_read[length_after_truncate - 1] != '\0' )
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
	
Status TruncateTest::NormalDecreaseSize()
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
		
		if ( truncate("some_file", length_after_truncate) != 0 )
		{
			cerr << "truncate failed. Error: " << strerror(errno);
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

Status TruncateTest::ErrAccess()
{
	try
	{	
		File f("some_file");
		
		struct passwd * nobody = getpwnam("nobody");
		
		if (nobody == NULL)
		{
			cerr << "Cannot obtain nobody user information. Error: " << strerror(errno);
			return Unres;
		}
		
		if ( seteuid(nobody->pw_uid) == -1 )
		{
			cerr << "Cannot set the effective user ID to nobody. Error: " << strerror(errno);
			return Unres;
		}
				
		if ( truncate("some_file", 1) == 0 || errno != EACCES )
		{
			cerr << "truncate should return EACCES error code but it did not. Error: " << strerror(errno);
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

Status TruncateTest::ErrFault()
{
	if ( truncate((const char*)0, 1) == 0 || errno != EPERM )
	{
		cerr << "truncate should return EPERM error code but it did not. Error: " << strerror(errno);
		return Fail;
	}
	return Success;
}

Status TruncateTest::ErrFBig()
{
	try
	{	
		File f("some_file");
		off_t length = UINT_MAX;
	
		if ( truncate("some_file", length) == 0 || errno != EFBIG )
		{
			cerr << "truncate should return EFBIG error code but it did not. Error: " << strerror(errno);
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

Status TruncateTest::ErrInval()
{
	try
	{	
		File f("some_file");		
	
		if ( truncate("some_file", -1) == 0 || errno != EINVAL )
		{
			cerr << "truncate should return EINVAL error code but it did not. Error: " << strerror(errno);
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

Status TruncateTest::ErrIsDir()
{
	try
	{	
		Directory dir("some_dir");		
	
		if ( truncate("some_dir", 1) == 0 || errno != EISDIR )
		{
			cerr << "truncate should return EISDIR error code but it did not. Error: " << strerror(errno);
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

Status TruncateTest::ErrLoop()
{
	try
	{	
		File f("old_file");
		
		if ( symlink("old_file", "new_file") == -1)
		{
			cerr << "Cannot create symlink on old_file. Error: " << strerror(errno);
			return Unres;
		}
		
		if ( unlink("old_file") == -1)
		{
			cerr << "Cannot remove old_file. Error: " << strerror(errno);
			return Unres;
		}
		
		if ( symlink("new_file", "old_file") == -1)
		{
			cerr << "Cannot create symlink on new_file. Error: " << strerror(errno);
			return Unres;
		}
		int res = truncate("old_file", 1);
		
		unlink("new_file");
		
		if ( res == 0 || errno != ELOOP )
		{
			cerr << "truncate should return ELOOP error code but it did not. Error: " << strerror(errno);
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
Status TruncateTest::ErrNameTooLong()
{
	std::string filename = "asdf";
	for ( int i = 0; i < PATH_MAX; ++i )
		filename += "a";
		
	if ( truncate(filename.c_str(), 1) == 0 || errno != ENAMETOOLONG )
	{
		cerr << "truncate should return ENAMETOOLONG error code but it did not. Error: " << strerror(errno);
		return Fail;
	}
	
	filename = "asdf";
	for ( int i = 0; i < NAME_MAX; ++i )
		filename += "a";
		
	if ( truncate(filename.c_str(), 1) == 0 || errno != ENAMETOOLONG )
	{
		cerr << "truncate should return ENAMETOOLONG error code but it did not. Error: " << strerror(errno);
		return Fail;
	}
	
	return Success;
}

Status TruncateTest::ErrNoEnt()
{
	if ( truncate("non_existing_file", 1) == 0 || errno != ENOENT )
	{
		cerr << "truncate should return ENOENT error code but it did not. Error: " << strerror(errno);
		return Fail;
	}
	
	return Success;
}

Status TruncateTest::ErrNotDir()
{
	try
	{
		File f("not_a_dir_name");
		
		if ( truncate("not_a_dir_name/some_file", 1) == 0 || errno != ENOTDIR )
		{
			cerr << "truncate should return ENOTDIR error code but it did not. Error: " << errno << " > " << strerror(errno);
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

Status TruncateTest::ErrTxtBsy()
{
	cerr << "truncating ${CMAKE_INSTALL_PREFIX}/bin/spruce" << endl;
	if ( truncate("${CMAKE_INSTALL_PREFIX}/bin/spruce", 1) == 0 || errno != ETXTBSY )
	{
		cerr << "truncate should return ETXTBSY error code but it did not. Error: " << errno << " > " << strerror(errno);
		return Fail;
	}
		
	return Success;	
}
