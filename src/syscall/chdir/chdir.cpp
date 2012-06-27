//      chdir.cpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Giorgi Gulabyan <gulabyang@gmail.com>
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

#include <chdir.hpp>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "File.hpp"



int Chdir::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			/*case CHDIR_S_IRWXU:
				return PermissionsTest(S_IRWXU);
	        case CHDIR_S_IRUSR:
				return PermissionsTest(S_IRUSR);
			case CHDIR_S_IWUSR:
				return PermissionsTest(S_IWUSR);
			case CHDIR_S_IXUSR:
				return PermissionsTest(S_IXUSR);
				
			case CHDIR_S_IRWXG:
				return PermissionsTest(S_IRWXG);
			case CHDIR_S_IRGRP:
				return PermissionsTest(S_IRGRP);
			case CHDIR_S_IWGRP:
				return PermissionsTest(S_IWGRP);
			case CHDIR_S_IXGRP:
				return PermissionsTest(S_IXGRP);
				
			case CHDIR_S_IRWXO:
				return PermissionsTest(S_IRWXO);
			case CHDIR_S_IROTH:
				return PermissionsTest(S_IROTH);
			case CHDIR_S_IWOTH:
				return PermissionsTest(S_IWOTH);
			case CHDIR_S_IXOTH:
				return PermissionsTest(S_IXOTH);
			case CHDIR_S_ISUID:
				return PermissionsTest(S_ISUID);*/
				
		    case CHDIR_ERR_ENAMETOOLONG:
				return ChdirTooLongPath();
			case CHDIR_ERR_ENOENT:
				return ChdirFileNotExist();
		    case CHDIR_ERR_ENOTDIR:
				return ChdirIsNotDirectory();
				
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}



Status Chdir::ChdirIsNotDirectory()
{
	const char *pathNotDirectory = "chdirTest.txt/somthingelse" ;
	int  ret_chdir;
		
	try
	{
		ret_chdir = chdir(pathNotDirectory);
		
				
		if(ret_chdir == 0)
		{
			cerr << "Chdir reruns 0 but it should return -1 when  component of the path prefix is not a directory  "<<strerror(errno);
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



Status Chdir::ChdirTooLongPath()
{
	
	
    int ret_chdir;
	const char* tooLongPath = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	
	try
	{
	
		ret_chdir = chdir(tooLongPath);
		
		if(ret_chdir == 0)
		{
			cerr << "Chdir reruns 0 but it should return -1 when the path is too long  "<<strerror(errno);
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


Status Chdir::ChdirFileNotExist()
{
	
	const char *path="/notExistingPath198/2/7/1/htap";
	int ret_chdir;
	ret_chdir = chdir(path);
	
	if(ret_chdir == 0)
	{
		cerr << "Chdir reruns 0 but it should return -1 when the file is not exist  "<<strerror(errno);
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


/*
Status Chdir::PermissionsTest(int open_mode)
{

	const char *path="chdirTest.txt";
	int ret_chdir;
	struct stat mode;
		
	try
	{
		File file(path, open_mode);


		ret_chdir = chdir(path, open_mode);
		
		if(ret_chdir == -1)
		{
			cerr << "Chdir  failed: Aborting test "<<strerror(errno);
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
			cerr<<"Chdire failed ";
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
*/
	

