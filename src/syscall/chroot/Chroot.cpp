//      Chroot.cpp
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

#include "Chroot.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <pwd.h>
#include "File.hpp"
#include "Directory.hpp"


int Chroot::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{			
		    case CHROOT_ERR_ENAMETOOLONG:
				return chrootTooLongPath();
			case CHROOT_ERR_EFAULT:
				return chrootFault();
			case CHROOT_ERR_ENOENT:
				return chrootFileNotExist();
		    case CHROOT_ERR_ENOTDIR:
				return chrootIsNotDirectory();
			case CHROOT_ERR_ELOOP:
				return chrootLoopInSymLink();	
			case CHROOT_ERR_EACCES:
				return chrootNoAcces();
			case CHROOT_ERR_EPERM:
				return chrootNoPerm();
		    case CHROOT_NORMAL_FUNC:
				return chrootNormalFunc();	
			default:
				cerr << "Unsupported operation.";
				return Unsupported;		
		}
	}
	cerr << "Test was successful";
	return Success;
}


Status Chroot::chrootFault()
{
	if(chroot((char *)-1) == 0)
	{
		cerr<<"Chroot return 0, but pathname points outside your accessible address space. "<<strerror(errno);
		return Fail;
	}
	if(errno != EFAULT)
	{
		
		cerr << "Incorrect error set in errno. "<<strerror(errno);
		return Fail;
	}
	return Success;	
}

Status Chroot::chrootIsNotDirectory()
{
	const char *path="chrootTest.txt";
	const char *pathNotDirectory = "chrootTest.txt/somthingelse" ;
	
	try
	{
		File file(path, S_IWUSR);
				
		if(chroot(pathNotDirectory) == 0)
		{
			cerr << "Chroot reruns 0 but it should return -1 when  component of the path prefix is not a directory  "<<strerror(errno);
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


Status Chroot::chrootTooLongPath()
{
	const char* tooLongPath = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";

	if(chroot(tooLongPath) == 0)
	{
		cerr << "Chroot reruns 0 but it should return -1 when the path is too long  "<<strerror(errno);
		return Fail;
	}
	if(errno != ENAMETOOLONG)
	{
		
		cerr << "Incorrect error set in errno in case of too long file name "<<strerror(errno);
		return Fail;
	}
	return Success;	
}


Status Chroot::chrootFileNotExist()
{
	const char *path="/notExistPath198/2/7/1/htap";
	
	if(chroot(path) == 0)
	{
		cerr << "Chroot return 0 but it should return -1 when the file is not exist  "<<strerror(errno);
		return Fail;
	}
	if(errno != ENOENT)
	{
		
		cerr << "Incorrect error set in errno in case of file does not exists "<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

Status Chroot::chrootNormalFunc()
{
	try
	{		
		Directory dir("chrootTestDirectory", 0777);		
		File file("chrootTestDirectory/chrootTestFile.txt");
		
		if(chroot("chrootTestDirectory") != 0)
		{
			cerr << "Chroot can not change the root directory. "<<strerror(errno);
			return Fail;
		} 
		if(access("/chrootTestFile.txt", F_OK) != 0)
		{
			cerr << "Chroot does not change the working directory. ";
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

Status Chroot::chrootLoopInSymLink()
{
	long size;
	char * cwd = NULL;
	size = pathconf(".", _PC_PATH_MAX);

	if ((cwd = (char *)malloc((size_t)size)) == NULL)
	{
		cerr<<"Can not allocate memmory. "<<strerror(errno);
		return Unres;
	}
	cwd = getcwd(cwd, (size_t)size);	
	if (cwd == NULL)
	{
		cerr<<"Can not get current working directory "<<strerror(errno);
		return Unres;
	}	
	
    string dirPath = (string)cwd + "/chdirTestDirectory";
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
	   
		if (chroot(path.c_str()) != -1) 
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

Status Chroot::chrootNoAcces ()
{
    struct passwd * noBody;
    const int FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    const char * dirPath = "chdir_noaccess_dir";
 
 
	try
	{
		Directory dir((string)dirPath, FILE_MODE);	
		
		// Change root to nobody
		if((noBody = getpwnam("nobody")) == NULL) {
			cerr<< "Can not set user to nobody";
			return Unres;
		}
		if (seteuid(noBody->pw_uid) != 0) {
			cerr<<"Can not set uid";
			return Unres;
		}
		 
		if (chroot(dirPath) != -1) {
			cerr<<"Chroot should return -1 when search permission was denied .";
			// Change nobody to root
			if (seteuid(0) != 0)
				cerr<<"Can not set uid";
			return Fail;
		}
		int Error = errno;
		
		// Change nobody to root
		if (seteuid(0) != 0)
			cerr<<"Can not set uid";
			
		if (Error != EACCES) {
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
}

Status Chroot::chrootNoPerm ()
{
    struct passwd * noBody;
    const int FILE_MODE = 0;
    const char * dirPath = "chdir_noperm_dir";
 
	try
	{
		Directory dir((string)dirPath, FILE_MODE);	
		
		// Change root to nobody
		if((noBody = getpwnam("nobody")) == NULL) {
			cerr<< "Can not set user to nobody";
			return Unres;
		}
		if (seteuid(noBody->pw_uid) != 0) {
			cerr<<"Can not set uid";
			return Unres;
		}
		 
		if (chroot(dirPath) != -1) {
			cerr<<"Chroot should return -1 when search permission was denied .";
			// Change nobody to root
			if (seteuid(0) != 0)
				cerr<<"Can not set uid";
			return Fail;
		}
		
		int Error = errno;
		
		// Change nobody to root
		if (seteuid(0) != 0)
			cerr<<"Can not set uid";
			
		if (Error != EPERM) {
			cerr << "Incorrect error set in errno. "<<strerror(errno);
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

