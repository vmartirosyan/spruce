//      Getcwd.cpp
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

#include "Getcwd.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <pwd.h>
#include "Directory.hpp"



int Getcwd::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{					
		    case GETCWD_NORMAL_FUNC:
				return GetcwdNormalFunc();	
			case GETCWD_ERR_EACCES:
				return GetcwdNoAcces();
		    case GETCWD_ERR_EFAULT:
				return GetcwdFault();
		    case GETCWD_ERR_EINVAL:
				return GetcwdIncorrectValue();
		    case GETCWD_ERR_ENOENT:
				return GetcwdDirUnlinked();
		    case GETCWD_ERR_ERANGE:
				return GetcwdRange();
			default:
				cerr << "Unsupported operation.";
				return Unsupported;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status Getcwd::GetcwdNormalFunc()
{	
	char * cwd;
	long size = pathconf(".", _PC_PATH_MAX);
	string dirPath;
	
	try
	{		
		cwd = NULL;
		if ((cwd = (char *)malloc((size_t)size)) == NULL)
		{
			cerr << "Can not allocate memory  "<<strerror(errno);
			return Unres;
		}		
		if(getcwd(cwd, (size_t)size) == NULL)
		{
			cerr << "Getcwd return NULL pointer. "<<strerror(errno);
			free(cwd);
			return Fail;
		}
		dirPath = (string)cwd + "getcwdtestdir";
		free(cwd);		
		
		Directory dir(dirPath, 0777);	
		
		if(chdir(dirPath.c_str()) != 0)
		{
			cerr << "Chdir does not change the working directory. "<<strerror(errno);
			return Unres;
		} 		
		
		cwd = NULL;
		if ((cwd = (char *)malloc((size_t)size)) == NULL)
		{
			cerr << "Can not allocate memory  "<<strerror(errno);
			return Unres;
		}	
		if(getcwd(cwd, (size_t)size) == NULL)
		{
			cerr << "Getcwd return NULL pointer. "<<strerror(errno);
			free(cwd);
			return Fail;
		}
		
		if(strcmp(cwd, dirPath.c_str()) != 0)
		{
			cerr << "Directory change error ";
			free(cwd);
			return Fail;
		}
		free(cwd);
		return Success;

	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
}

Status Getcwd::GetcwdNoAcces()
{
	char * dirPath = "getced_noaccess_dir";
    struct passwd * noBody;
     
	try
	{
		Directory dir((string)dirPath, 0666);	
			 
		if (chdir(dirPath) != 0) 
		{
			cerr<<"Chdir can not change the working directory. ";
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
		
		char * cwd = NULL;
		long size = pathconf(".", _PC_PATH_MAX);
		if ((cwd = (char *)malloc((size_t)size)) == NULL)
		{
			cerr << "Can not allocate memory  "<<strerror(errno);
			return Unres;
		}		
		if(getcwd(cwd, size) != NULL)
		{
			cerr << "Getwd return not NULL pointer but it should return NULL when premission to read or search was denied. "<<strerror(errno);
			return Fail;
		}
		if(errno != EACCES)
		{
			cerr << "Incorrect error set in errno in case of premission to read or search was denied. "<<strerror(errno);
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
Status Getcwd::GetcwdFault()
{
	//(char*)-1 is outside of our accessible address space
	long size = pathconf(".", _PC_PATH_MAX); 
	if(getcwd(static_cast<char*>(-1), (size_t)size) != NULL)
	{
		cerr << "Getcwd return not NULL pointer but it should return NULL when the buf points to a bad address. "<<strerror(errno);
		return Fail;
	}
	if(errno != EFAULT)
	{
		cerr << "Incorrect error set in errno in case of the buf points to a bad address. "<<strerror(errno);
		return Fail;
	}
	return Success;
}

Status Getcwd::GetcwdDirUnlinked()
{
	if(mkdir("getcwdtestdir", 0777) != 0)
	{
		cerr<<"Can not make directore. "<<strerror(errno);
		return Unres;
	}
	if(chdir("getcwdtestdir") != 0)
	{
		cerr << "Can not change the working directory. "<<strerror(errno);
		return Unres;
	} 
	if(rmdir("../getcwdtestdir") != 0)
	{
		cerr << "Can not delete the working directory. "<<strerror(errno);
		return Unres;
	} 	
	char * cwd = NULL;
	long size = pathconf(".", _PC_PATH_MAX);
	if ((cwd = (char *)malloc((size_t)size)) == NULL)
	{
		cerr << "Can not allocate memory  "<<strerror(errno);
		return Unres;
	}		
	if(getcwd(cwd, size) != NULL)
	{
		cerr << "Getcwd return not NULL pointer but it should return NULL when the working directory has been unlinked. "<<strerror(errno);
		free(cwd);
		return Fail;
	}
	free(cwd);
	if(errno != ENOENT)
	{
		cerr << "Incorrect error set in errno in case of the working directory has been unlinked. "<<strerror(errno);
		return Fail;
	}	
	return Success; 
	
}

Status Getcwd::GetcwdRange()
{
	char * cwd = NULL;
	int size = 1;
	

	if ((cwd = (char *)malloc((size_t)size)) == NULL)
	{
		cerr << "Can not allocate memory  "<<strerror(errno);
		return Unres;
	}
	
	if(getcwd(cwd, (size_t)size) != NULL)
	{
		cerr << "Getcwd return not NULL pointer but it should return NULL when size is less than the length of the absolute pathname. "<<strerror(errno);
		free(cwd);
		return Fail;
	}
	free(cwd);
	if(errno != ERANGE)
	{
		cerr << "Incorrect error set in errno in case of size is less than the length of the absolute pathname. "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

Status Getcwd::GetcwdIncorrectValue()
{
	char * cwd = NULL;
	int size = pathconf(".", _PC_PATH_MAX);
	

	if ((cwd = (char *)malloc((size_t)size)) == NULL)
	{
		cerr << "Can not allocate memory  "<<strerror(errno);
		return Unres;
	}
	
	size = 0;
	if(getcwd(cwd, (size_t)size) != NULL)
	{
		cerr << "Getcwd reruns not NULL pointer but it should return NULL when size is zero and buff is not a null pointer. "<<strerror(errno);
		free(cwd);
		return Fail;
	}
	free(cwd);
	if(errno != EINVAL)
	{
		cerr << "Incorrect error set in errno in case of size is zero and buff is not a null pointer. "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}
