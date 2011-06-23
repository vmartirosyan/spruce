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

#include <Chmod.hpp>
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
				return PermissionsTest(S_IRWXU);
	        case CHMOD_S_IRUSR:
				return PermissionsTest(S_IRUSR);
			case CHMOD_S_IWUSR:
				return PermissionsTest(S_IWUSR);
			case CHMOD_S_IXUSR:
				return PermissionsTest(S_IXUSR);
				
			case CHMOD_S_IRWXG:
				return PermissionsTest(S_IRWXG);
			case CHMOD_S_IRGRP:
				return PermissionsTest(S_IRGRP);
			case CHMOD_S_IWGRP:
				return PermissionsTest(S_IWGRP);
			case CHMOD_S_IXGRP:
				return PermissionsTest(S_IXGRP);
				
			case CHMOD_S_IRWXO:
				return PermissionsTest(S_IRWXO);
			case CHMOD_S_IROTH:
				return PermissionsTest(S_IROTH);
			case CHMOD_S_IWOTH:
				return PermissionsTest(S_IWOTH);
			case CHMOD_S_IXOTH:
				return PermissionsTest(S_IXOTH);
			case CHMOD_S_ISUID:
				return PermissionsTest(S_ISUID);
		    case CHMOD_ERR_ENAMETOOLONG:
				return  ChmodTooLongPath();
			case CHMOD_ERR_ENOENT:
				return  ChmodFileNotExist();
		    case CHMOD_ERR_ENOTDIR:
				return ChmodIsNotDirectory();
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}



Status Chmod::ChmodIsNotDirectory()
{

	
	const char *path="chmodTest.txt";
	const char *pathNotDirectory = "chmodTest.txt/somthingelse" ;
	int  ret_chmod;
	struct stat mode;
		
	try
	{
		File file(path, S_IWUSR);

		ret_chmod = chmod(pathNotDirectory, S_IWUSR);
		
				
		if(ret_chmod == 0)
		{
			cerr << "Chmod reruns 0 but it should return -1 when  component of the path prefix is not a directory  "<<strerror(errno);
			return Fail;
		}
		
		else 
		{
			
			if(errno != ENOTDIR)
			{
				
				cerr << "Incorrect error set in errno in case of component of the path prefix is not a directory "<<strerror(errno);
				return Fail;
			}
			
		}
		
	return Success;


	}
	
	
	
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Fail;
	}

	
}



Status Chmod::ChmodTooLongPath()
{
	
	
    int ret_chmod;
	const char* filename = "chmodTest.txt";
	const char* tooLongfilename = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	
	try
	{
		File file(filename, S_IWUSR);
	
		ret_chmod = chmod(tooLongfilename, S_IWUSR);
		
		if(ret_chmod == 0)
		{
			cerr << "Chmod reruns 0 but it should return -1 when the path is too long  "<<strerror(errno);
			return Fail;
		}
		

		else
		{
			if(errno != ENAMETOOLONG)
			{
				
				cerr << "Incorrect error set in errno in case of too long file name "<<strerror(errno);
				return Fail;
			}
		}
		return Success;

	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Fail;
	}
	
}


Status Chmod::ChmodFileNotExist()
{
	
	const char *path="chmodTest.txt";
	int ret_chmod;
	ret_chmod = chmod(path, S_IWUSR);
	
	if(ret_chmod == 0)
	{
		cerr << "Chmod reruns 0 but it should return -1 when the file is not exist  "<<strerror(errno);
		return Fail;
	}
	else
		{
			if(errno != ENOENT)
			{
				
				cerr << "Incorrect error set in errno in case of file does not exists "<<strerror(errno);
				return Fail;
			}
		}
	
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

	

