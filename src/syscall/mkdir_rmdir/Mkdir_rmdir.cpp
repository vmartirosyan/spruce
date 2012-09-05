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
#include "File.hpp"


int Mkdir_rmdir::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{		
			case MKDIR_RMDIR_NORMAL:
				return mkdirRmdirNormal();
				
				
			case MKDIR_ENAMETOOLONG:
				return mkdirNameTooLong();
			case MKDIR_ENOENT:	
				return mkdirPathNotEnt();
			case MKDIR_ENOTDIR:
				return mkdirNotDir();
			case MKDIR_EEXIST:
				return mkdirExists();
			case MKDIR_EFAULT:
				return mkdirFault();
			case MKDIR_EACCES:
				return mkdirNoAccess();
				
			case RMDIR_ENAMETOOLONG:	
				return rmdirNameTooLong();
			case RMDIR_ENOENT:
				return rmdirPathNotEnt();
			case RMDIR_ENOTDIR:
				return rmdirNotDir();
			case RMDIR_EINVAL:
				return rmdirInval();
			case RMDIR_ENOTEMPTY:
				return rmdirNotEmpty();
			case RMDIR_EFAULT:
				return rmdirFault();
			case RMDIR_EACCES:
				return rmdirNoAccess();

			default:
				cerr << "Unsupported operation.";
				return Unsupported;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status Mkdir_rmdir::mkdirRmdirNormal()
{
	const char * dir = "mkdirrmdirtestdir";
	if(mkdir(dir, 0777) != 0)
	{
		cerr<<"Mkdir can not create directory. "<<strerror(errno);
		return Fail;
	}
	if(rmdir(dir) != 0)
	{
		cerr<<"Rmdir can not delete directory. "<<strerror(errno);
		return Fail;
	}	
	return Success;	
}



Status Mkdir_rmdir::mkdirNoAccess()
{
	const char * dir = "mkdirnoaccessdir";
	struct passwd * noBody;
	const int FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
		 	
	// Change root to nobody
	if((noBody = getpwnam("nobody")) == NULL) {
		cerr<< "Can not set user to nobody";
		return Unres;
	}
	if (setuid(noBody->pw_uid) != 0) {
		cerr<<"Can not set uid";
		return Unres;
	}
	
	if(mkdir(dir, FILE_MODE) == 0)
	{
		cerr<<"Mkdir create directory. ";
		return Fail;		
	}
	
	if(errno != EACCES)
	{
		cerr << "Incorrect error set in errno. "<<strerror(errno);
		return Fail;
	}	
	return Success;
}
Status Mkdir_rmdir::mkdirFault()
{
	if(mkdir((char *)-1, 0777) == 0)
	{
		cerr<<"Mkdir return 0, but pathname points outside your accessible address space. "<<strerror(errno);
		return Fail;
	}
	if(errno != EFAULT)
	{
		
		cerr << "Incorrect error set in errno. "<<strerror(errno);
		return Fail;
	}
	return Success;
}
Status Mkdir_rmdir::mkdirExists()
{
	const char * dir = "mkdirtestdirex";
	if(mkdir(dir, 0777) != 0)
	{
		cerr << "Mkdir can not create directory. "<<strerror(errno);
		return Unres;
	} 
	if(mkdir(dir, 0777) == 0)
	{
		cerr << "Mkdir reruns 0 but it should return -1 when pathname already exists. "<<strerror(errno);
		rmdir(dir);
		return Fail;
	}
	if(errno != EEXIST)
	{
		cerr << "Incorrect error set in errno in case of pathname already exists. "<<strerror(errno);
		return Fail;
	}
		
	rmdir(dir);
	return Success;
	
}
Status Mkdir_rmdir::mkdirNotDir()
{
	const char *filePath="mkdirTest.txt";
	const char *pathNotDirectory = "mkdirTest.txt/somthingelse" ;
		
	try
	{
		File file(filePath, S_IWUSR);
				
		if(mkdir(pathNotDirectory, 0777) == 0)
		{
			cerr << "Mkdir reruns 0 but it should return -1 when  component of the path prefix is not a directory. "<<strerror(errno);
			return Fail;
		} 
		if(errno != ENOTDIR)
		{
			cerr << "Incorrect error set in errno in case of component of the path prefix is not a directory. "<<strerror(errno);
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
Status Mkdir_rmdir::mkdirPathNotEnt()
{
	const char* noentpath = "no_ent_path_1840_3478453/rve/ev";
	if(mkdir(noentpath, 0777) == 0)
	{
		cerr << "Mkdir reruns 0 but it should return -1 when a directory component in pathname does not exist. "<<strerror(errno);
		return Fail;
	}
	if(errno != ENOENT)
	{
		
		cerr << "Incorrect error set in errno. "<<strerror(errno);
		return Fail;
	}
	return Success;
}
Status Mkdir_rmdir::mkdirNameTooLong()
{
	const char* tooLongPath = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	if(mkdir(tooLongPath, 0777) == 0)
	{
		cerr << "Mkdir reruns 0 but it should return -1 when the path is too long  "<<strerror(errno);
		return Fail;
	}
	if(errno != ENAMETOOLONG)
	{
		
		cerr << "Incorrect error set in errno in case of too long file name "<<strerror(errno);
		return Fail;
	}
	return Success;
}











Status Mkdir_rmdir::rmdirNoAccess()
{
	const char * dir = "rmdirnoaccessdir";
	struct passwd * noBody;
	const int FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	if(mkdir(dir, FILE_MODE) != 0)
	{
		cerr<<"Can not make directory. ";
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
	
	if(rmdir(dir) == 0)
	{
		cerr << "Rmdir remove directory. "<<strerror(errno);
		return Fail;
	}
	
	if(errno != EACCES)
	{
		cerr << "Incorrect error set in errno. "<<strerror(errno);
		return Fail;
	}	
	return Success;
}
Status Mkdir_rmdir::rmdirFault()
{
	if(rmdir((char *)-1) == 0)
	{
		cerr<<"Rmdir return 0, but pathname points outside your accessible address space. "<<strerror(errno);
		return Fail;
	}
	if(errno != EFAULT)
	{
		
		cerr << "Incorrect error set in errno. "<<strerror(errno);
		return Fail;
	}
	return Success;
}
Status Mkdir_rmdir::rmdirNotEmpty()
{
	if(mkdir("rmdirtestdir", 0777) != 0)
	{
		cerr<<"Can not make directory. "<<strerror(errno);
		return Unres;
	}
	if(mkdir("rmdirtestdir/rmdirtestdir2", 0777) != 0)
	{
		cerr<<"Can not make directory. "<<strerror(errno);
		return Unres;
	}	
	
	if(rmdir("rmdirtestdir") == 0)
	{
		cerr<<"Rmdir deleted directory, but directory is not empty. "<<strerror(errno);
		return Fail;
	}		
	if(errno != ENOTEMPTY)
	{
		
		cerr << "Incorrect error set in errno. "<<strerror(errno);
		return Fail;
	}
	
	rmdir("rmdirtestdir/rmdirtestdir2");
	rmdir("rmdirtestdir");
	return Success;		
}
Status Mkdir_rmdir::rmdirInval()
{
	const char* invalpath = "/.";
	if(rmdir(invalpath) == 0)
	{
		cerr << "Rmdir reruns 0 but pathname has .  as last component. "<<strerror(errno);
		return Fail;
	}
	if(errno != EINVAL)
	{
		
		cerr << "Incorrect error set in errno. "<<strerror(errno);
		return Fail;
	}
	return Success;	
}
Status Mkdir_rmdir::rmdirNotDir()
{
	const char *filePath="rmdirTest.txt";
	const char *pathNotDirectory = "rmdirTest.txt/somthingelse" ;
		
	try
	{
		File file(filePath, S_IWUSR);
				
		if(rmdir(pathNotDirectory) == 0)
		{
			cerr << "Rmdir reruns 0 but it should return -1 when  component of the path prefix is not a directory  "<<strerror(errno);
			return Fail;
		} 
		if(errno != ENOTDIR)
		{
			cerr << "Incorrect error set in errno in case of component of the path prefix is not a directory "<<strerror(errno);
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
Status Mkdir_rmdir::rmdirPathNotEnt()
{
	const char* noentpath = "no_ent_path_1840_3478453/rve/ev";
	if(rmdir(noentpath) == 0)
	{
		cerr << "Rmdir reruns 0 but it should return -1 when a directory component in pathname does not exist. "<<strerror(errno);
		return Fail;
	}
	if(errno != ENOENT)
	{
		
		cerr << "Incorrect error set in errno. "<<strerror(errno);
		return Fail;
	}
	return Success;	
}
Status Mkdir_rmdir::rmdirNameTooLong()
{
	const char* tooLongPath = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	if(rmdir(tooLongPath) == 0)
	{
		cerr << "Rmdir reruns 0 but it should return -1 when the path is too long  "<<strerror(errno);
		return Fail;
	}
	if(errno != ENAMETOOLONG)
	{
		
		cerr << "Incorrect error set in errno in case of too long file name "<<strerror(errno);
		return Fail;
	}
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
