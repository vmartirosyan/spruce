//      Openat.cpp
//      
//      Copyright (C) 2011, Institute for System Programming
//       	                of the Russian Academy of Sciences (ISPRAS)
//		Author(s):
//      Gurgen Torozyan <gurgen.torozyan@gmail.com>
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

#include <Openat.hpp>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include "File.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include "StatFile.hpp"
#include <dirent.h>

int Openat::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
		case OPENAT_ENAMETOOLONG:
				return openatTooLongPath();
			case OPENAT_ENOTDIR:
				return openatPathIsNotDir();
			case OPENAT_EFAULT:
				return openatOutside();
			case OPENAT_ELOOP:
				return	openatLoopInSymLink();
			case OPENAT_EACCES:
				return	openatNoAcces();
			case OPENAT_ENOENT:
				return openatPathCompIsNotDir();
			case OPENAT_EEXIST:
				return openatPathAlreadyExist();
		    case OPENAT_EISDIR:
				return openatPathIsDir();
			case OPENAT_EMFILE:
				return openatMaxFileNumOpened();
			case OPENAT_ENODEV:
				return openatNoSuchDev();
			case OPENAT_ETXTBSY:
				return openatPathisExecutable();
			case OPENAT_ENXIO:
				return openatNoDevWRBL();
			case OPENAT_NORMAL:
				return openatNormal();

			
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}



Status Openat::openatTooLongPath()
{
	
	
  const char* tooLongfilename="abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	
	
	
	int dir_fd, fd;
    DIR *d = opendir(_buildDir.c_str());
    if(d == NULL)
    {
		cerr << "Opendir failed"<<strerror(errno);
		return Unres;
	}
    dir_fd = dirfd(d);
    fd = openat( dir_fd,tooLongfilename, O_RDWR);
	
	
				
	if(fd != -1)
	{
			cerr << "Openat returned normal file decriptor in case of too long old path name";
			unlink(tooLongfilename);
			return Fail;
	}
	else
	{
		if(errno!=ENAMETOOLONG)
		{	//	unlink("tekk000i.txt");
		
			cerr << "Incorrect error set in errno in case of too long file name "<<strerror(errno);
			return Fail;
		}
	}
	
	return Success;
	
}

Status Openat::openatOutside()
{
	
	int ret_val =0;	
	int dir_fd, fd;	
	
	//(char*)-1 is outside of our accessible address space
    
    
    DIR *d = opendir(_buildDir.c_str());
    
    if(d == NULL)
    {
		cerr << "Opendir failed"<<strerror(errno);
		return Unres;
	}
	
    dir_fd = dirfd(d);
    ret_val = openat( dir_fd,(char*)-1, O_CREAT);
   
	if(ret_val != -1)
	{
			cerr << "Openat returned normal file decriptor when the pathname is outside of your acessible address space";
			return Fail;
	}
	else
	{
		if(errno!=EFAULT)
		{
				cerr << "Incorrect error set in errno in case of EFAULT "<<strerror(errno);
				return Fail;
		}
	}
	return Success;
	
}



Status Openat::openatPathIsNotDir()
{
	
	const char * filename = "notadir"	;	
	int ret_val = 0;
	
	int dir_fd;
	File file(filename, S_IRWXU);

    DIR *d = opendir(_buildDir.c_str());
    if(d == NULL)
    {
		cerr << "Opendir failed"<<strerror(errno);
		return Unres;
	}
    dir_fd = dirfd(d);
    ret_val = openat( dir_fd,"notadir/test.txt", O_CREAT);
	
	
	
	if(ret_val != -1)
	{
			cerr << "Openat returned normal file decriptor when the part of the path is not a directory";
			return Fail;
	}
	else
	{
		if(errno!=ENOTDIR)
		{
				cerr << "Incorrect error set in errno in case of too long file name "<<strerror(errno);
				return Fail;
		}
	}
	return Success;
	
}

Status Openat::openatLoopInSymLink()
{
	int dir_fd, fd;
	int ret_val =0;	
    string dirPath = this->_statDir + "/openat_loop_dir";
    string link1 = dirPath + "/link1";
    string link2 = dirPath + "/link2";
    
    string filePath = link1 + "/open_file";
    const int FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
   
    if (mkdir (dirPath.c_str(), FILE_MODE) == -1) {
        cerr<<"mkdir() can't create directory.";
        return Unres;
    }
    if (symlink (dirPath.c_str(), link1.c_str()) == -1) {
        cerr<<"symlink() can't create symlink.";
        return Unres;
    }
    if (symlink (link1.c_str(), link2.c_str()) == -1) {
        cerr<<"symlink() can't create symlink.";
        return Unres;
    }
    if (unlink (link1.c_str()) == -1) {
        cerr<<"remove() can't remove symlink.";
        return Unres;
    }
    if (symlink (link2.c_str(), link1.c_str()) == -1) {
        cerr<<"symlink() can't create symlink.";
        return Unres;
    }    
   

	//passing pathname with looping symbolic links .. expected to get ELOOP
	
	
	DIR *d = opendir(_buildDir.c_str());
    if(d == NULL)
    {
		cerr << "Opendir failed"<<strerror(errno);
		return Unres;
	}
    dir_fd = dirfd(d);
    ret_val = openat( dir_fd,filePath.c_str(), O_CREAT);
	
   
    if (ret_val != -1) {
        cerr<<"Openat should return -1 when we are passing pathname with looping symbolic links.";
        return Fail;
    }    
    if (errno != ELOOP) {
        cerr << "Incorrect error set in errno in case of looping symbolic links "<<strerror(errno);
        return Fail;
    }
     
    return Success;
	
}

Status Openat::openatNoAcces ()
{
	
    string dirPath = this->_statDir + "/openat_noaccess_dir";
    string filePath = dirPath + "/openat_file";
    struct passwd * noBody;
    int ret_val;
    int dir_fd;
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
    
	
    DIR *d = opendir(_buildDir.c_str());
    if(d == NULL)
    {
		cerr << "Opendir failed"<<strerror(errno);
		return Unres;
	}
    dir_fd = dirfd(d);
    ret_val = openat( dir_fd,filePath.c_str(), O_CREAT);
    
    	
	   if (ret_val != -1) {
           cerr<<"Openat should return -1 when search permission was denied .";
       return Fail;
    }
    
    if (errno != EACCES) {
           cerr << "Incorrect error set in errno in case of search permission assces denied "<<strerror(errno);

        return Fail;
    }
    return Success;
}


Status Openat::openatPathCompIsNotDir()
{
	const char* notrealDirInPath="/home/uyyss8z888dezze3/test.txt";
	int ret_val;
	int dir_fd;
		
    DIR *d = opendir(_buildDir.c_str());
    if(d == NULL)
    {
		cerr << "Opendir failed"<<strerror(errno);
		return Unres;
	}
    dir_fd = dirfd(d);
    ret_val = openat( dir_fd,notrealDirInPath, O_CREAT);
    
	
				
	if(ret_val != -1)
	{
			cerr << "Openat returned normal file decriptor in when path component was not a directory";
			return Fail;
	}
	else
	{
		if(errno!=ENOENT)
		{		
			cerr << "Incorrect error set in errno in case of too long file name "<<strerror(errno);
			return Fail;
		}
	}
	return Success;
}

Status Openat::openatPathAlreadyExist()
{
	
	const char * filename = "OpenAtfirstFile.txt";
	
	int ret_val = 0;
	int dir_fd;
	File file(filename, S_IRWXU);
		
    //Тhe path already exist situation expected to get EEXSIST error
    
   
    DIR *d = opendir(_buildDir.c_str());
    dir_fd = dirfd(d);
    ret_val = openat(dir_fd, filename, O_CREAT|O_EXCL);
	
	if(ret_val != -1)
	{
			cerr << "Openat returned normal file decriptor when  the path  already exists";
			return Fail;
	}
	else
	{
		if(errno!=EEXIST)
		{
				cerr << "Incorrect error set in errno when the path already exist"<<strerror(errno);
				return Fail;
		}
	}
	return Success;
	
}


Status Openat::openatPathIsDir()
{	
    string dirPath = this->_statDir + "/OpenAtPathIsDir";
    int ret_val;
    int dir_fd;
    const int FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    
    if (mkdir (dirPath.c_str(), FILE_MODE) == -1) 
    {
        cerr<<"mkdir() can't create directory.";
        return Unres;
    }
   
   //passing Dir as a path expecting to get EISDIR error

	
	
    DIR *d = opendir(_buildDir.c_str());
    if(d == NULL)
    {
		cerr << "Opendir failed"<<strerror(errno);
		return Unres;
	}
    dir_fd = dirfd(d);
    ret_val = openat( dir_fd,dirPath.c_str(), O_WRONLY);
    
	
		
    if(ret_val != -1)
	{
			cerr << "Openat returned normal file decriptor but path was Directory";
			return Fail;
	}
	else
	{
		if(errno!=EISDIR)
		{
				cerr << "Incorrect error set in errno when the path is directory"<<strerror(errno);
				return Fail;
		}
	}
	return Success;
	
}

Status Openat:: openatMaxFileNumOpened()
{
	int ind, fd,fd2;
	int stat =0;
	int dir_fd;
	pid_t pid;
	
	const char* filename = "openatProcHasMax.txt";
    int max_files = getdtablesize();
    int ret_val;
    DIR *d = opendir(_buildDir.c_str());
    if(d == NULL)
    {
		cerr << "Opendir failed"<<strerror(errno);
		return Unres;
	}
    dir_fd = dirfd(d);
    ret_val = openat( dir_fd,filename, O_CREAT);
    
    if(ret_val == -1)
	{
			cerr << "Openat returned -1 in case of normal filename. "<<strerror(errno);
			return Fail;
	}
  
  
     
    
	for ( ind = 0; ind < max_files+1; ++ind )
	{
		fd =   openat( dir_fd,filename, O_WRONLY,0700);
		
		if(ind == max_files && fd != -1)
		{
			cerr<<"Openat returns normal file decriptor but  the process already has the maximum number of files open ";
			return Fail;
		}

	}
	  
	  
	  if ( errno != EMFILE)
     {
	
		cerr << "Incorrect error set in errno when the process already has the maximum number of files open. "
		     << strerror(errno);
		 return Fail;
		
	  }
	  
	  
    unlink(filename);
	return Success;
}


Status Openat::openatNoSuchDev()
{
	
   
    int  stat, ret_val, fd2 , dir_fd;
    
	const char * filename = "openatnodevtest";
	
    stat = mknod(filename, S_IFCHR | 0666, 0);
    if(stat == -1 )
    {
		cerr<<strerror(errno);
		return Unres;
	}

	
	DIR *d = opendir(_buildDir.c_str());
    if(d == NULL)
    {
		cerr << "Opendir failed"<<strerror(errno);
		return Unres;
	}
    dir_fd = dirfd(d);
    ret_val = openat( dir_fd,filename, O_WRONLY);

	if(ret_val != -1)
	{
		cerr<<"Openat returned normal file decriptor when there is no device to open ";
		return Fail;
	}
	else 	
	
		if(errno != ENODEV)
		{
			if(errno != ENXIO)
			{
			
				cerr<<"Incorrect error set in errno when no corresponding device exists "<< strerror(errno);
				return Fail;
			}
		}

	unlink(filename);
return Success;
}

Status Openat ::openatNoDevWRBL()
{
	int stat, ret_val,fd2,dir_fd;
	const char * filename = "openatnodevFIFOtest";
	
    stat = mknod(filename, S_IFIFO | 0666, 0);
    if(stat == -1 )
    {
		cerr<<strerror(errno);
		return Unres;
	}


	DIR *d = opendir(_buildDir.c_str());
    if(d == NULL)
    {
		cerr << "Opendir failed"<<strerror(errno);
		return Unres;
	}
    dir_fd = dirfd(d);
    ret_val = openat( dir_fd,filename, O_NONBLOCK | O_WRONLY);

	
	
	if(ret_val != -1)
	{
		cerr<<"Openat returned normal file decriptor when there is no device to open ";
		return Fail;
	}
	else 	
	
			if(errno != ENXIO)
			{
			
				cerr<<"Incorrect error set in errno when no corresponding device exists "<< strerror(errno);
				return Fail;
			}
		

	unlink(filename);
}


Status Openat::openatPathisExecutable()
{
	int ret_val = 0 , fd2 ,dir_fd;
	const char* filename = "bin/spruce";
		
	
	
	DIR *d = opendir(_buildDir.c_str());
    if(d == NULL)
    {
		cerr << "Opendir failed"<<strerror(errno);
		return Unres;
	}
    dir_fd = dirfd(d);
    ret_val = openat( dir_fd,filename,  O_WRONLY);
	
	if(ret_val != -1)
	{
		cerr<<"Openat returned normal file decriptor when we try to open executable file with O_WRONLY flag ";
		return Fail;
	}
	else if(errno != ETXTBSY)
	{
		cerr<<"Incorrect error set in errno when we try to open executable file"<<strerror(errno);;
		return Fail;
	}
	
	return Success;
}

Status Openat::openatNormal()
{	
	const char * filename = "openAtNormal.txt"	;	
	int dir_fd,ret_val;
	
	DIR *d = opendir(_buildDir.c_str());
    if(d == NULL)
    {
		cerr << "Opendir failed"<<strerror(errno);
		return Unres;
	}
    dir_fd = dirfd(d);
    ret_val = openat( dir_fd,filename,  O_CREAT);
	
	
	if(ret_val==-1)
	{
			cerr << "Openat returned error in case of noraml filename."<<strerror(errno);
			return Fail;
	}
	unlink(filename);
	return Success;
	
	
}


