//      Access.cpp
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



#include <Access.hpp>
#include "File.hpp"
#include "Directory.hpp"
#include <pwd.h>
#include <linux/limits.h>

int AccessTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case AccessNormal:
				return NormalFunc();
			case AccessFileExists:
				return FileExists();
			case AccessErrAccess:
				return ErrAccess();			
			case AccessErrLoop:
				return ErrLoop();
			case AccessErrNameTooLong:
				return ErrNameTooLong();
			case AccessErrNoEnt:
				return ErrNoEnt();
			case AccessErrNotDir:
				return ErrNotDir();			
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status AccessTest::FileExists()
{
	try
	{	
		File f("some_file");
		
		if ( access("some_file", F_OK) != 0 )
		{
			cerr << "The file exists but access returns error: " << strerror(errno);
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

Status AccessTest::NormalFunc()
{
	try
	{
		File f("some_file", S_IROTH | S_IWOTH | S_IXOTH );
		
		struct stat buf;
		stat("some_file", &buf);
		
		cerr << "Others may read: " << (buf.st_mode & S_IROTH) << endl;
		cerr << "Others may write: " << (buf.st_mode & S_IWOTH) << endl;
		cerr << "Others may execute: " << (buf.st_mode & S_IXOTH) << endl;
		
		struct passwd * nobody = getpwnam("nobody");
		
		if ( nobody == NULL )
		{
			cerr << "Cannot obtain nobody user information. Error: " << strerror(errno);
			return Unres;
		}
		
		if ( setuid(nobody->pw_uid) == -1 )
		{
			cerr << "Cannot set the effective user ID to nobody. Error: " << strerror(errno);
			return Unres;
		}
		
		if ( access("some_file", R_OK) == -1 )
		{
			cerr << "Read permission is granted but access returns error: " << strerror(errno);
			return Fail;
		}
		
		if ( access("some_file", W_OK) == -1 )
		{
			cerr << "Write permission is granted but access returns error: " << strerror(errno);
			return Fail;
		}
		
		if ( access("some_file", X_OK) != 0 )
		{
			cerr << "Execution permission is granted but access returns error: " << strerror(errno);
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

Status AccessTest::ErrAccess()
{
	try
	{	
		File f("some_file", 0600);
		
		struct passwd * nobody = getpwnam("nobody");
		
		if (nobody == NULL)
		{
			cerr << "Cannot obtain nobody user information. Error: " << strerror(errno);
			return Unres;
		}
		
		if ( setuid(nobody->pw_uid) == -1 )
		{
			cerr << "Cannot set the effective user ID to nobody. Error: " << strerror(errno);
			return Unres;
		}
				
		if ( access("some_file", W_OK) == 0 || errno != EACCES )
		{
			cerr << "access should return EACCES error code but it did not. Error: " << strerror(errno);
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

Status AccessTest::ErrLoop()
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
		int res = access("old_file", W_OK);
		
		unlink("new_file");
		
		if ( res == 0 || errno != ELOOP )
		{
			cerr << "access should return ELOOP error code but it did not. Error: " << strerror(errno);
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

Status AccessTest::ErrNameTooLong()
{
	std::string filename = "asdf";
	for ( int i = 0; i < PATH_MAX; ++i )
		filename += "a";
		
	if ( access(filename.c_str(), W_OK) == 0 || errno != ENAMETOOLONG )
	{
		cerr << "access should return ENAMETOOLONG error code but it did not. Error: " << strerror(errno);
		return Fail;
	}
	
	return Success;
}

Status AccessTest::ErrNoEnt()
{
	if ( access("non_existing_file", W_OK) == 0 || errno != ENOENT )
	{
		cerr << "access should return ENOENT error code but it did not. Error: " << strerror(errno);
		return Fail;
	}
	
	return Success;
}

Status AccessTest::ErrNotDir()
{
	try
	{
		File f("not_a_dir_name");
		
		if ( access("not_a_dir_name/some_file", W_OK) == 0 || errno != ENOTDIR )
		{
			cerr << "access should return ENOTDIR error code but it did not. Error: " << errno << " > " << strerror(errno);
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

