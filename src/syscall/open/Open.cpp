//      Chmod.cpp
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

#include <Open.hpp>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "File.hpp"
#include "StatFile.hpp"



int Open::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case OPEN_ENAMETOOLONG:
				return openTooLongPath();
			case OPEN_ENOTDIR:
				return openPathIsNotDir();
			case OPEN_EFAULT:
				return openOutside();
			case OPEN_ELOOP:
				return	openLoopInSymLink();
			case OPEN_EACCES:
				return	openNoAcces();
			case OPEN_ENOENT:
				return openPathCompIsNotDir();
			case OPEN_EEXIST:
				return openPathAlreadyExist();
		    case OPEN_EISDIR:
				return openPathIsDir();
			case OPEN_EMFILE:
				return openMaxFileNumOpened();
			case OPEN_ENODEV:
				return openNoSuchDev();
			case OPEN_ETXTBSY:
				return openPathisExecutable();
			case OPEN_ENXIO:
				return openNoDevWRBL();
			case OPEN_NORMAL:
				return openNormal();
		
			
	        
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}




Status Open::openTooLongPath()
{
	
	
  const char* tooLongfilename="abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	
	int ret_val = open(tooLongfilename, O_CREAT );
				
	if(ret_val != -1)
	{
			cerr << "Open returned normal file decriptor in case of too long old path name";
			return Fail;
	}
	else
	{
		if(errno!=ENAMETOOLONG)
		{		
			cerr << "Incorrect error set in errno in case of too long file name "<<strerror(errno);
			return Fail;
		}
	}
	return Success;
	
}

Status Open::openOutside()
{
	
	int ret_val =0;		
	
    //(char*)-1 is outside of our accessible address space
	ret_val = open((char*)-1, O_CREAT );
	if(ret_val != -1)
	{
			cerr << "Open returned normal file decriptor when the pathname is outside of your acessible address space";
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



Status Open::openPathIsNotDir()
{
	
	const char * filename = "notadir"	;	
	int ret_val = 0;
	File file(filename, S_IRWXU);
		
    //Тhe path component is not a directory	
	ret_val = open("notadir/test.txt", O_CREAT );
	
	if(ret_val != -1)
	{
			cerr << "Open returned normal file decriptor when the part of the path is not a directory";
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

Status Open::openLoopInSymLink()
{
	
	int ret_val =0;	
    string dirPath = this->_statDir + "/open_loop_dir";
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
	ret_val = open(filePath.c_str(), O_CREAT );
   
    if (ret_val != -1) {
        cerr<<"Open should return -1 when we are passing pathname with looping symbolic links.";
        return Fail;
    }    
    if (errno != ELOOP) {
        cerr << "Incorrect error set in errno in case of looping symbolic links "<<strerror(errno);
        return Fail;
    }
     
    return Success;
	
}

Status Open::openNoAcces ()
{
	
    string dirPath = this->_statDir + "/noaccess_dir";
    string filePath = dirPath + "/open_file";
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
    
    
	ret_val = open(filePath.c_str(), O_CREAT );
    
 
    if (ret_val != -1) {
           cerr<<"Open should return -1 when search permission was denied .";
       return Fail;
    }
    
    if (errno != EACCES) {
           cerr << "Incorrect error set in errno in case of search permission assces denied "<<strerror(errno);

        return Fail;
    }
    return Success;
}


Status Open::openPathCompIsNotDir()
{
	  const char* notrealDirInPath="/home/rrttvvzc43433/test.txt";
	
	int ret_val = open(notrealDirInPath, O_CREAT );
				
	if(ret_val != -1)
	{
			cerr << "Open returned normal file decriptor in when path component was not a directory";
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

Status Open::openPathAlreadyExist()
{
	
	const char * filename = "firstFile"	;	
	int ret_val = 0;
	File file(filename, S_IRWXU);
		
    //Тhe path already exist situation expected to get EEXSIST error
	ret_val = open(filename, O_CREAT|O_EXCL);
	
	if(ret_val != -1)
	{
			cerr << "Open returned normal file decriptor when but the path  already exists";
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


Status Open::openPathIsDir()
{	
    string dirPath = this->_statDir + "/PathIsDir";
    int ret_val;
    const int FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    
    if (mkdir (dirPath.c_str(), FILE_MODE) == -1) 
    {
        cerr<<"mkdir() can't create directory.";
        return Unres;
    }
   
   //passing Dir as a path expecting to get EISDIR error
	ret_val = open(dirPath.c_str(), O_WRONLY);
		
	if(ret_val != -1)
	{
			cerr << "Open returned normal file decriptor but path was Directory";
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

Status Open:: openMaxFileNumOpened()
{
	int ind, fd;
	int stat =0;
	pid_t pid;
	const char* filename = "procHasMax.txt";
    int max_files = getdtablesize();
    
    
	open( filename ,O_CREAT );
    
	for ( ind = 0; ind < max_files+1; ++ind )
	{
		fd = open( filename ,O_WRONLY , 0700 );
		
		if(ind == max_files && fd != -1)
		{
			cerr<<"Openat returns normal file decriptor but  the process already has the maximum number of files open ";
			return Fail;
		}

	}
	  
	  
	  if ( errno != EMFILE)
     {
	
		cerr << "Incorrect error set in errno when the process already has the maximum number of files open"
		     << strerror(errno);
		 return Fail;
		
	  }
	  
	  
    unlink(filename);
	return Success;
}


Status Open::openNoSuchDev()
{
	
   
    int  stat, fd;
	const char * filename = "nodevtest";
	
    stat = mknod(filename, S_IFCHR | 0666, 0);
    
    if(stat == -1 )
    {
		cerr<<"mknod:"<<strerror(errno);
		return Unres;
	}

	fd = open(filename, O_WRONLY);
	 
	if(fd != -1)
	{
		cerr<<"Open returned normal file decriptor when there is no device to open ";
		
		return Fail;
	}
	else 	
	
		if(errno != ENODEV && errno != ENXIO)
		{
		
			
				cerr<<"Incorrect error set in errno when no corresponding device exists "<< strerror(errno);
			
				return Fail;
			
		}

	unlink(filename);
	return Success;
}

Status Open ::openNoDevWRBL()
{
	int stat, fd;
	const char * filename = "nodevFIFOtest";
	
    stat = mknod(filename, S_IFIFO | 0666, 0);
    if(stat == -1 )
    {
		cerr<<"mknod:"<<strerror(errno);
		return Unres;
	}

	fd = open(filename, O_NONBLOCK | O_WRONLY);
	
	if(fd != -1)
	{
		cerr<<"Open returned normal file decriptor when there is no device to open ";
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


Status Open::openPathisExecutable()
{
	int fd = 0,fd_exec=0;

	const char* filename = "bin/spruce";
	//const char* filename = "/home/gurgen/buildscript/bin/spruce";
	
		
	fd = open( filename ,O_WRONLY );
	
	if(fd != -1)
	{
		cerr<<"Open returned normal file decriptor when we try to open executable file with O_WRONLY flag ";
		return Fail;
	}
	else if(errno != ETXTBSY)
	{
		cerr<<"Incorrect error set in errno when we try to open executable file."<<strerror(errno);
		return Fail;
	}
	
	return Success;
}

Status Open::openNormal()
{	
	const char * filename = "openNormal.txt"	;	
	int ret_val = open(filename,O_CREAT);
	if(ret_val==-1)
	{
			cerr << "open returned error in case of noraml filename."<<strerror(errno);
			return Fail;
	}
	unlink(filename);
	return Success;
	
	
}
