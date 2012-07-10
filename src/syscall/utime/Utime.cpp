//      Utime.cpp
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



#include "Utime.hpp"
#include "File.hpp"
#include "Directory.hpp"
#include <pwd.h>
#include <linux/limits.h>

int UtimeTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case UtimeNormalNotNull:
				return NormallNotNull();
			case UtimeNormalNull:
				return NormallNull();
			case UtimeErrAccess:
				return ErrAccess();			
			case UtimeErrLoop:
				return ErrLoop();
			case UtimeErrNameTooLong1:
				return ErrNameTooLong1();
			case UtimeErrNameTooLong2:
				return ErrNameTooLong2();
			case UtimeErrNoEnt1:
				return ErrNoEnt1();
			case UtimeErrNoEnt2:
				return ErrNoEnt2();
			case UtimeErrNotDir:
				return ErrNotDir();
			case UtimeErrPerm:
				return ErrPerm();
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status UtimeTest::NormallNotNull()
{
	try
	{	
		File f("some_file");
		struct stat stat_buf;
		
		time_t cur_time = time(0);
		if ( cur_time == (time_t)-1 )
		{
			cerr << "Cannot get current time. Error: " << strerror(errno);
			return Unres; 
		}
		
		cur_time -= 100;
		
		struct utimbuf buf;
		buf.actime = cur_time;
		
		if ( utime("some_file", &buf) == -1 )
		{
			cerr << "utime call failed. Error: " << strerror(errno);
			return Fail;
		}		
		
		if ( stat("some_file", &stat_buf) == -1 )
		{
			cerr << "Cannot stat the file. Error: " << strerror(errno);
			return Unres; 
		}
		
		if ( stat_buf.st_atime != cur_time )
		{
			cerr << "Wrong access time was set: " << cur_time << " != " << stat_buf.st_atime;
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
Status UtimeTest::NormallNull()
{
	try
	{	
		File f("some_file");
		struct stat stat_buf;
		
		time_t cur_time = time(0);
		
		if ( cur_time == (time_t)-1 )
		{
			cerr << "Cannot get current time. Error: " << strerror(errno);
			return Unres; 
		}
		
		if ( utime("some_file", NULL) == -1 )
		{
			cerr << "utime call failed. Error: " << strerror(errno);
			return Fail;
		}		
		
		if ( stat("some_file", &stat_buf) == -1 )
		{
			cerr << "Cannot stat the file. Error: " << strerror(errno);
			return Unres; 
		}
		
		if ( stat_buf.st_atime != cur_time )
		{
			cerr << "Wrong access time was set: " << cur_time << " != " << stat_buf.st_atime;
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

Status UtimeTest::ErrAccess()
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
				
		if ( utime("some_file", NULL) == 0 || errno != EACCES )
		{
			cerr << "utime should return EACCES error code but it did not. Error: " << strerror(errno);
			return Fail;
		}
		
		// Restore the user id to root
		if ( seteuid(0) == -1 )
		{
			cerr << "Cannot set the effective user ID to nobody. Error: " << strerror(errno);			
		}
		
		return Success;
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}	
}

Status UtimeTest::ErrLoop()
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
		int res = utime("old_file", NULL);
		
		unlink("new_file");
		
		if ( res == 0 || errno != ELOOP )
		{
			cerr << "utime should return ELOOP error code but it did not. Error: " << strerror(errno);
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
Status UtimeTest::ErrNameTooLong1()
{
	std::string filename = "asdf";
	for ( int i = 0; i < PATH_MAX; ++i )
		filename += "a";
		
	if ( utime(filename.c_str(), NULL) == 0 || errno != ENAMETOOLONG )
	{
		cerr << "utime should return ENAMETOOLONG error code but it did not. Error: " << strerror(errno);
		return Fail;
	}
	
	return Success;
}
Status UtimeTest::ErrNameTooLong2()
{
	std::string filename = "asdf";
	for ( int i = 0; i < NAME_MAX; ++i )
		filename += "a";
		
	if ( utime(filename.c_str(), NULL) == 0 || errno != ENAMETOOLONG )
	{
		cerr << "utime should return ENAMETOOLONG error code but it did not. Error: " << strerror(errno);
		return Fail;
	}
	
	return Success;
}
Status UtimeTest::ErrNoEnt1()
{
	if ( utime("non_existing_file", NULL) == 0 || errno != ENOENT )
	{
		cerr << "utime should return ENOENT error code but it did not. Error: " << strerror(errno);
		return Fail;
	}
	
	return Success;
}
Status UtimeTest::ErrNoEnt2()
{
	if ( utime("", NULL) == 0 || errno != ENOENT )
	{
		cerr << "utime should return ENOENT error code but it did not. Error: " << strerror(errno);
		return Fail;
	}
	
	return Success;
}
Status UtimeTest::ErrNotDir()
{
	try
	{
		File f("not_a_dir_name");
		
		if ( utime("not_a_dir_name/some_file", NULL) == 0 || errno != ENOTDIR )
		{
			cerr << "utime should return ENOTDIR error code but it did not. Error: " << errno << " > " << strerror(errno);
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
Status UtimeTest::ErrPerm()
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
		struct utimbuf buf;
		if ( utime("some_file", &buf) == 0 || errno != EPERM )
		{
			cerr << "utime should return EPERM error code but it did not. Error: " << strerror(errno);
			return Fail;
		}
		
		// Restore the user id to root
		if ( seteuid(0) == -1 )
		{
			cerr << "Cannot set the effective user ID to nobody. Error: " << strerror(errno);			
		}
		
		return Success;
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}	
}



