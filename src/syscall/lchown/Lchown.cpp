//      Lchown.cpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Gurgen Torozyan <gurgen.torozyan@gmail.com>
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

#include "Lchown.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "File.hpp"
#include <pwd.h>




int Lchown::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			
			case LCHOWN_CHANGE_OWNER:
			return LchownTest();
			
		    case LCHOWN_ERR_ENAMETOOLONG:
				return  LchownTooLongPath();
			case LCHOWN_ERR_ENOENT:
				return  LchownFileNotExist();
		    case LCHOWN_ERR_ENOTDIR:
				return LchownIsNotDirectory();
			case LCHOWN_SYMLINK:
				return LchownSymlinktest();
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status Lchown::LchownSymlinktest()
{
	const char *path="chownTest.txt";
	int ret_chown;
	struct passwd *nobody;
	uid_t user_uid;
	gid_t user_gid;	
	int ret_symlink;
	const char * newpath ="chownTestSym.txt";
	
	
	ret_symlink = symlink(path,newpath);
	
	if(ret_symlink == -1)
	{
		cerr<<"symlink failed "<<strerror(errno);
		return Unres;
	}
	
	try
	{
		File file(newpath, S_IWUSR);

	   //getting struct for nobody user	
		nobody= getpwnam("nobody");
		if(nobody == NULL)
		{
			cerr << "getpwnam failed: Aborting test "<<strerror(errno);
			return Unres;
		}
		
		//getting user  and group id for nobody user.
		user_uid = nobody->pw_uid;
		user_gid = nobody->pw_gid;

		//changing owner  from root to nobody 
		ret_chown = lchown(newpath, user_uid,user_gid);
		
		if(ret_chown == 0)
		{
			cerr<<"lchown should not dereference symbolic link , but it does. "<<strerror(errno);
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




Status Lchown::LchownTest()
{
	const char *path="chownTest.txt";
	int ret_chown;
	struct stat mode;
	struct passwd *nobody;
	int ret ;
	uid_t user_uid;
	gid_t user_gid;
		
	try
	{
		File file(path, S_IWUSR);

	   //getting struct for nobody user	
		nobody= getpwnam("nobody");
		if(nobody == NULL)
		{
			cerr << "getpwnam failed: Aborting test "<<strerror(errno);
			return Unres;
		}
		
		//getting user  and group id for nobody user.
		user_uid = nobody->pw_uid;
		user_gid = nobody->pw_gid;

		//changing owner  from root to nobody 
		ret_chown = lchown(path, user_uid,user_gid);
		
		if(ret_chown == -1)
		{
			cerr << "Chown failed: Aborting test "<<strerror(errno);
			return Unres;
		}
			
			
		ret = stat(path, &mode);
		if( ret == -1 )
		{
				cerr << "stat failed: Aborting test "<<strerror(errno);
			return Unres;
		}
		
		if( user_uid != mode.st_uid )
		{
			cerr << "Chown failed :chown should chenge file owner from root to nobody but it does not";
			return Fail;	
		}
		
		if( user_gid != mode.st_gid )
		{
			cerr << "Chown failed :chown should chenge group  from root to nobody but it does not";
			return Fail;	
		}

		
		
		
		//ret_chown = chown(tooLongfilename, mode.st_uid,mode.st_gid);
	
	return Success;


	}
	
		catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Fail;
	}
}



Status Lchown::LchownIsNotDirectory()
{

	
	const char *path="chownTest.txt";
	const char *pathNotDirectory = "chownTest.txt/somthingelse" ;
	int  ret_chown;
	struct stat mode;
	int ret ;
	try
	{
		File file(path, S_IWUSR);
		
		ret = stat(path, &mode);
		if( ret == -1 )
		{
				cerr << "stat failed: Aborting test "<<strerror(errno);
			return Unres;
		}
		
		ret_chown = lchown(pathNotDirectory, mode.st_uid,mode.st_gid);
		
				
		if(ret_chown == 0)
		{
			cerr << "Chown reruns 0 but it should return -1 when  component of the path prefix is not a directory  "<<strerror(errno);
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



Status Lchown::LchownTooLongPath()
{
	
    int ret_chown;
    struct stat mode;
    int ret;
	const char* filename = "chownTest.txt";
	const char* tooLongfilename = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";

	try
	{
		File file(filename, S_IWUSR);
		
		ret = stat(filename, &mode);
		if( ret == -1 )
		{
				cerr << "stat failed: Aborting test "<<strerror(errno);
			return Unres;
		}
		
		
		ret_chown = lchown(tooLongfilename, mode.st_uid,mode.st_gid);
		
		
		
		if(ret_chown == 0)
		{
			cerr << "Chown reruns 0 but it should return -1 when the path is too long  "<<strerror(errno);
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

Status Lchown::LchownFileNotExist()
{
	
	const char *path="chownTest.txt";
	
	int ret_chown ;
	
	ret_chown = lchown(path, 0,0);
	
	if(ret_chown == 0)
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






	

