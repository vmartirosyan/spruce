	//      Statfs.cpp
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

#include <Statfs.hpp>
#include "File.hpp"
#include "Directory.hpp"
#include <map>


int StatfsTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case StatfsNormalFunc:
				return NormalFunc();
			case StatfsErrAccess:
				return ErrAccess();
			case StatfsErrFault:
				return ErrFault();
			case StatfsErrLoop:
				return ErrLoop();
			case StatfsErrNameTooLong:
				return ErrNameTooLong();
			case StatfsErrNoEnt:
				return ErrNoEnt();
			case StatfsErrNotDir:
				return ErrNotDir();
			default:
				cerr << "Unsupported operation.";
				return Unsupported;
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status StatfsTest::NormalFunc()
{
	if ( MountAt == "" || FileSystem == "" )
	{
		cerr << "Either MountAt or FileSyste env variables are not defined";
		return Unres;
	}
	
	if ( FileSystemTypesMap[FileSystem] == -1 )
	{
		cerr << "statfs is not supported by " << FileSystem;
		return Unsupported;
	}
	
	struct statfs buf;
	
	if ( statfs(MountAt.c_str(), &buf) != 0 )
	{
		cerr << "statfs failed. Error: " << strerror(errno);
		return Fail;
	}
	
	if ( buf.f_type != FileSystemTypesMap[FileSystem] )
	{
		cerr << "statfs returned wrong FS type: " << buf.f_type;
		return Fail;
	}
	return Success;
}
Status StatfsTest::ErrAccess()
{
	try
	{	
		Directory dir("some_dir");
		File f("some_dir/some_file");
		
		if ( chmod ("some_dir", 0700) == -1 )
		{
			cerr << "Cannot change directory permission mode. Error: " << strerror(errno);
			return Unres; 
		}
		
		struct statfs buf;
		if ( statfs("some_dir/some_file", &buf) == 0 || errno != EACCES )
		{
			cerr << "statfs should return EACCES error code but it did not. Error: " << strerror(errno);
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
Status StatfsTest::ErrFault()
{
	try
	{
		File f("some_file");
		
		struct statfs buf;
		if ( statfs("some_file", &buf) == 0 || errno != EFAULT )
		{
			cerr << "statfs should return EFAULT error code but it did not. Error: " << errno << " > " << strerror(errno);
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
Status StatfsTest::ErrLoop()
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
		struct statfs buf;
		int res = statfs("old_file", &buf);
		
		unlink("new_file");
		
		if ( res == 0 || errno != ELOOP )
		{
			cerr << "statfs should return ELOOP error code but it did not. Error: " << strerror(errno);
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
Status StatfsTest::ErrNameTooLong()
{
	std::string filename = "asdf";
	for ( int i = 0; i < PATH_MAX; ++i )
		filename += "a";
		
	struct statfs buf;
	if ( statfs(filename.c_str(), &buf) == 0 || errno != ENAMETOOLONG )
	{
		cerr << "statfs should return ENAMETOOLONG error code but it did not. Error: " << strerror(errno);
		return Fail;
	}
	
	return Success;
}
Status StatfsTest::ErrNoEnt()
{
	struct statfs buf;
	if ( statfs("non_existing_file", &buf) == 0 || errno != ENOENT )
	{
		cerr << "statfs should return ENOENT error code but it did not. Error: " << strerror(errno);
		return Fail;
	}
	
	return Success;
}
Status StatfsTest::ErrNotDir()
{
	try
	{
		File f("not_a_dir_name");
		
		struct statfs buf;
		if ( statfs("not_a_dir_name/some_file", &buf) == 0 || errno != ENOTDIR )
		{
			cerr << "statfs should return ENOTDIR error code but it did not. Error: " << errno << " > " << strerror(errno);
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
