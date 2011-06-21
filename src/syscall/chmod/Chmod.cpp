//      Close.cpp
//      
//      Copyright 2011 Gurgen Suren
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

#include "Chmod.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "File.hpp"



int Chmod::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case CHMOD_S_IRWXU:
				return PermissionsTest(CHMOD_S_IRWXU);
	        case CHMOD_S_IRUSR:
				return PermissionsTest(CHMOD_S_IRUSR);
			case CHMOD_S_IWUSR:
				return PermissionsTest(CHMOD_S_IWUSR);
			case CHMOD_S_IXUSR:
				return PermissionsTest(CHMOD_S_IXUSR);
				
			case CHMOD_S_IRWXG:
				return PermissionsTest(CHMOD_S_IRWXG);
			case CHMOD_S_IRGRP:
				return PermissionsTest(CHMOD_S_IRGRP);
			case CHMOD_S_IWGRP:
				return PermissionsTest(CHMOD_S_IWGRP);
			case CHMOD_S_IXGRP:
				return PermissionsTest(CHMOD_S_IXGRP);
				
			case CHMOD_S_IRWXO:
				return PermissionsTest(CHMOD_S_IRWXO);
			case CHMOD_S_IROTH:
				return PermissionsTest(CHMOD_S_IROTH);
			case CHMOD_S_IWOTH:
				return PermissionsTest(CHMOD_S_IWOTH);
			case CHMOD_S_IXOTH:
				return PermissionsTest(CHMOD_S_IXOTH);
			case CHMOD_S_ISUID:
				return PermissionsTest(CHMOD_S_ISUID);
			case ERR_EFAULT:
				return  ErrEfault();
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status Chmod::ErrEfault()
{/*
	const char *path="chmodTest.txt";
	int ret_chmod;
	struct stat mode;
		
	try
	{
		//File file(path, open_mode);


		ret_chmod = chmod("notafile", S_IWUSR);
		
		if(ret_chmod == -1)
		{
			cerr << "Chmod  failed: Aborting test "<<strerror(errno) <<errno;
			return Unres;
		}
		
		
	
	
	return Success;


	}
	
	
	
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Fail;
	}
	*/
	return Success;
}





Status Chmod::PermissionsTest(int open_mode)
{

	const char *path="chmodTest.txt";
	int ret_chmod;
	struct stat mode;
		
	try
	{
		File file(path, open_mode);


		ret_chmod = chmod(path, open_mode);
		
		if(ret_chmod == -1)
		{
			cerr << "Chmod  failed: Aborting test "<<strerror(errno);
			return Unres;
		}
		
		int ret = stat(path, &mode);
		
		if( ret == -1 )
		{
				cerr << "stat failed: Aborting test "<<strerror(errno);
			return Unres;
		}
		
		if((mode.st_mode & 4095) !=  open_mode)
		{
			cerr<<"Chmode failed ";
			return Fail;
		}
	
	
	return Success;


	}
	
	
	
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Fail;
	}
}

	

