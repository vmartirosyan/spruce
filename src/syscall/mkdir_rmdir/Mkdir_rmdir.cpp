//      Mkdir_rmdir.cpp
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

#include "Mkdir_rmdir.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <pwd.h>



int Mkdir_rmdir::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{			
		    /*case CHDIR_ERR_ENAMETOOLONG:
				return Mkdir_rmdirTooLongPath();
			case CHDIR_ERR_ENOENT:
				return Mkdir_rmdirFileNotExist();
		    case CHDIR_ERR_ENOTDIR:
				return Mkdir_rmdirIsNotDirectory();
		    case CHDIR_NORMAL_FUNC:
				return Mkdir_rmdirNormalFunc();	
			case CHDIR_ERR_ELOOP:
				return Mkdir_rmdirLoopInSymLink();	
			case CHDIR_ERR_EACCES:
				return Mkdir_rmdirNoAcces();*/
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

/*

Status Mkdir_rmdir::Mkdir_rmdirIsNotDirectory()
{
	const char *path="chdirTest.txt";
	const char *pathNotDirectory = "chdirTest.txt/somthingelse" ;
	int  ret_chdir;
		
	try
	{
		File file(path, S_IWUSR);
		ret_chdir = chdir(pathNotDirectory);
				
		if(ret_chdir == 0)
		{
			cerr << "Mkdir_rmdir reruns 0 but it should return -1 when  component of the path prefix is not a directory  "<<strerror(errno);
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
		return Unres;
	}

	
}


Status Mkdir_rmdir::Mkdir_rmdirTooLongPath()
{
    int ret_chdir;
	const char* tooLongPath = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	
	ret_chdir = chdir(tooLongPath);
	
	if(ret_chdir == 0)
	{
		cerr << "Mkdir_rmdir reruns 0 but it should return -1 when the path is too long  "<<strerror(errno);
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


Status Mkdir_rmdir::Mkdir_rmdirFileNotExist()
{
	
	const char *path="/notExistPath198/2/7/1/htap";
	int ret_chdir;
	
	ret_chdir = chdir(path);
	if(ret_chdir == 0)
	{
		cerr << "Mkdir_rmdir return 0 but it should return -1 when the file is not exist  "<<strerror(errno);
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

Status Mkdir_rmdir::Mkdir_rmdirNormalFunc()
{
	string dirPath = (string)_cwd + "/chdirTestDirectory";
	char * cwd;
	long size;
	int  ret_chdir;
		
	try
	{
		Directory dir(dirPath, 0777);		
		
		ret_chdir = chdir(dirPath.c_str());
		if(ret_chdir != 0)
		{
			cerr << "Mkdir_rmdir does not change the working directory. "<<strerror(errno);
			return Fail;
		} 
		
		
		cwd = NULL;
		size = pathconf(".", _PC_PATH_MAX);
		if ((cwd = (char *)malloc((size_t)size)) == NULL)
		{
			cerr << "Can not allocate memmory. "<<strerror(errno);
			return Unres;			
		}
		cwd = getcwd(cwd, (size_t)size);
		if(cwd == NULL)
		{
			cerr << "Can not read changed working directory. "<<strerror(errno);
			free(cwd);
			return Unres;
		}
		
		if(strcmp(cwd, dirPath.c_str()) != 0)
		{
			cerr << "Directory change error ";
			free(cwd);
			return Fail;
		}
		free(cwd);
		
		ret_chdir = chdir(_cwd);
		if(ret_chdir != 0)
		{
			cerr << "Mkdir_rmdir does not change the working directory. "<<strerror(errno);
			return Fail;
		} 
		return Success;

	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	
}


Status Mkdir_rmdir::Mkdir_rmdirLoopInSymLink()
{
	
	int ret_chdir = 0;	
    string dirPath = (string)_cwd + "/chdirTestDirectory";
    string link1 = dirPath + "/link1";
    string link2 = dirPath + "/link2";
    
    
	try
	{
		Directory dir(dirPath, 0777);		
		
		if (symlink (dirPath.c_str(), link1.c_str()) != 0) {
			cerr<<"symlink() can't create symlink.";
			return Unres;
		}
		if (symlink (link1.c_str(), link2.c_str()) != 0) {
			cerr<<"symlink() can't create symlink.";
			return Unres;
		}
		if (unlink (link1.c_str()) != 0) {
			cerr<<"remove() can't remove symlink.";
			return Unres;
		}
		if (symlink (link2.c_str(), link1.c_str()) != 0) {
			cerr<<"symlink() can't create symlink.";
			return Unres;
		}    
	   
		string path = link1 + "/chdir_file";

		//passing pathname with looping symbolic links .. expected to get ELOOP
		ret_chdir = chdir(path.c_str());
	   
		if (ret_chdir != -1) 
		{
			cerr<<"Creat should return -1 when we are passing pathname with looping symbolic links.";
			return Fail;
		}    
		if (errno != ELOOP) 
		{
			cerr << "Incorrect error set in errno in case of looping symbolic links "<<strerror(errno);
			return Fail;
		}
		 
		return Success;
	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	
}

Status Mkdir_rmdir::Mkdir_rmdirNoAcces ()
{
	
    string dirPath = (string)_cwd + "/chdir_noaccess_dir";
    struct passwd * noBody;
    int ret_chdir;
    struct stat stat_buf;
    const int FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
 
 
	try
	{
		Directory dir(dirPath, 0777);	
		  
		if (chmod (dirPath.c_str(), FILE_MODE) != 0) {
			cerr<<"can not chmod directory";
			return Unres;
		}
		
		// Change root to nobody
		if((noBody = getpwnam("nobody")) == NULL) {
			cerr<< "Can not set user to nobody";
			return Unres;
		}
		if (setuid(noBody->pw_uid) != 0) {
			cerr<<"Can not set uid";
			return Unres;
		}
		
		ret_chdir = chdir(dirPath.c_str());	 
		if (ret_chdir != -1) {
			cerr<<"Mkdir_rmdir should return -1 when search permission was denied .";
			return Fail;
		}
		
		if (errno != EACCES) {
			cerr << "Incorrect error set in errno in case of search permission assces denied "<<strerror(errno);
			return Fail;
		}
		return Success;
	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}	
}*/
