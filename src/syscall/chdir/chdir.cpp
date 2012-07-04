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
#include <sys/types.h>
#include <string.h>
#include "File.hpp"
#include "Directory.hpp"



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
		    case CHDIR_NORMAL_FUNC:
				return ChdirNormalFunc();	
			case CHDIR_ERR_ELOOP:
				return ChdirLoopInSymLink();			
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
	const char *path="chdirTest.txt";
	const char *pathNotDirectory = "chdirTest.txt/somthingelse" ;
	int  ret_chdir;
		
	try
	{
		File file(path, S_IWUSR);
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
		return Unres;
	}

	
}


Status Chdir::ChdirTooLongPath()
{
    int ret_chdir;
	const char* tooLongPath = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	
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


Status Chdir::ChdirFileNotExist()
{
	
	const char *path="/notExistPath198/2/7/1/htap";
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

Status Chdir::ChdirNormalFunc()
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
			cerr << "Chdir does not change the working directory. "<<strerror(errno);
			return Fail;
		} 
		
		
		cwd = NULL;
		size = pathconf(".", _PC_PATH_MAX);
		if ((cwd = (char *)malloc((size_t)size)) != NULL)
			cwd = getcwd(cwd, (size_t)size);
		if(cwd == NULL)
		{
			cerr << "Cannot read changed working directory. "<<strerror(errno);
			return Unres;
		}
		
		if(strcmp(cwd, dirPath.c_str()) != 0)
		{
			cerr << "Directory change error ";
			return Fail;
		}
		
		ret_chdir = chdir(_cwd);
		if(ret_chdir != 0)
		{
			cerr << "Chdir does not change the working directory. "<<strerror(errno);
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


Status Chdir::ChdirLoopInSymLink()
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
/*
Status Chdir::ChdirNoAcces ()
{
	
    string dirPath = this->_statDir + "/creat_noaccess_dir";
    string filePath = dirPath + "/creat_file";
    struct passwd * noBody;
    int ret_val;
    struct stat stat_buf;
    const int FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
   
   
    if (mkdir (dirPath.c_str(), FILE_MODE) == -1) {
        cerr<<"mkdir() can't create directory.";
        return Unres;
    }
   
    StatFile file (filePath);
   
    if (0 != chmod (filePath.c_str(), FILE_MODE)) {
        cerr<<"Can not chmod file";
        return Unres;
    }
    if (0 != chmod (dirPath.c_str(), FILE_MODE)) {
        cerr<<"can not chmod directory";
        return Unres;
    }
    
    // Change root to nobody
    if((noBody = getpwnam("nobody")) == NULL) {
        cerr<< "Can not set user to nobody";
        return Unres;
    }
    if (0 != setuid(noBody->pw_uid)) {
        cerr<<"Can not set uid";
        return Unres;
    }
    
    
	ret_val = creat(filePath.c_str(), S_IRWXU );
    
 
    if (ret_val != -1) {
           cerr<<"Creat should return -1 when search permission was denied .";
       return Fail;
    }
    
    if (errno != EACCES) {
           cerr << "Incorrect error set in errno in case of search permission assces denied "<<strerror(errno);

        return Fail;
    }
    return Success;
}
	
*/
