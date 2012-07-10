//      Link.cpp
//
//      Copyright (C) 2011, Institute for System Programming
//       	                of the Russian Academy of Sciences (ISPRAS)
//		Author(s):
//      Suren Gishyan <sgishyan@gmail.com>
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

#include "Creat.hpp"
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include "File.hpp"
#include "StatFile.hpp"

int CreatTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case CREAT_ENAMETOOLONG:
				return CreatTooLongPath();
			case CREAT_EMFILE:
				return CreatMaxProcessFilesOpen();
			case CREAT_ENOTDIR:
				return CreatPathIsNotDir();
			case CREAT_EFAULT:
				return CreatOutside();
			case CREAT_ELOOP:
				return CreatLoopInSymLink();
			case CREAT_EACCES:
				return CreatNoAcces();
			case CREAT_ENOENT:
				return CreatPathCompIsNotDir();		
		    case CREAT_EISDIR:
				return CreatPathIsDir();
			case CREAT_NORMAL:
				return CreatNormal();
			
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status CreatTest::CreatTooLongPath()
{	
	
	
	const char* tooLongfilename="abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	
	int ret_val = creat(tooLongfilename,S_IRWXU);
	if(ret_val != -1)
	{
			cerr << "creat returned normal file decriptor in case of too long old path name";
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





Status CreatTest::CreatMaxProcessFilesOpen()
{	
	
	
	
	long max_files_open=sysconf(_SC_OPEN_MAX);
	buf=new int[max_files_open+1];
	for (file_index=0; file_index <= max_files_open; file_index++)
	{
		//Generating new filename
		sprintf(fname, "testfile%d.txt", file_index);
	
		//Creating new file
		int ret_val = creat(fname,S_IRWXU);	
		
		
		//Saving file descriptors
		buf[file_index]=ret_val;
		
		
	
		if(ret_val==-1 && file_index<=max_files_open)
		{
			if(errno!=EMFILE)
			{
				
				cerr << "Incorrect error set in errno when the process already has the maximum number of files open."<<strerror(errno);
				CreatMaxProcessCleanUp();
				return Fail;
			}
			else
			{
				CreatMaxProcessCleanUp();
				return Success;
			}
		}
	}
	cerr << "No error was accured when the process already has the maximum number of files open.";
	return Fail;
}

void CreatTest::CreatMaxProcessCleanUp()
{
	for(int i=file_index-1;i>=0;i--)
	{
		
		sprintf(fname, "testfile%d.txt", i);
		close(buf[i]);
		unlink(fname);
	}
	
}




Status CreatTest::CreatOutside()
{
	
	int ret_val =0;		
	
    //(char*)-1 is outside of our accessible address space
	ret_val = creat((char*)-1, S_IRWXU );
	if(ret_val != -1)
	{
			cerr << "Creat returned normal file decriptor when the pathname is outside of your acessible address space";
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



Status CreatTest::CreatPathIsNotDir()
{
	
	const char * filename = "notadir"	;	
	int ret_val = 0;
	File file(filename, S_IRWXU);
		
    //Тhe path component is not a directory	
	ret_val = creat("notadir/test.txt", S_IRWXU );
	
	if(ret_val != -1)
	{
			cerr << "Creat returns normal file descriptor when the part of the path is not a directory";
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

Status CreatTest::CreatLoopInSymLink()
{
	
	int ret_val =0;	
    string dirPath = this->_statDir + "/creat_loop_dir";
    string link1 = dirPath + "/link1";
    string link2 = dirPath + "/link2";
    
    string filePath = link1 + "/creat_file";
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
	ret_val = creat(filePath.c_str(), S_IRWXU );
   
    if (ret_val != -1) {
        cerr<<"Creat should return -1 when we are passing pathname with looping symbolic links.";
        return Fail;
    }    
    if (errno != ELOOP) {
        cerr << "Incorrect error set in errno in case of looping symbolic links "<<strerror(errno);
        return Fail;
    }
     
    return Success;
	
}

Status CreatTest::CreatNoAcces ()
{
	
    string dirPath = this->_statDir + "/creat_noaccess_dir";
    string filePath = dirPath + "/creat_file";
    struct passwd * noBody;
    int ret_val;
    //struct stat stat_buf;
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


Status CreatTest::CreatPathCompIsNotDir()
{
	  const char* notrealDirInPath="/home/hhgg44zzz44a/test.txt";
	
	int ret_val = creat(notrealDirInPath, S_IRWXU );
				
	if(ret_val != -1 )
	{
			cerr << "Creat returned a normal file descriptor in when path component was not a directory";
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




Status CreatTest::CreatPathIsDir()
{	
    string dirPath = this->_statDir + "/CreatPathIsDir";
    int ret_val;
    const int FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    
    if (mkdir (dirPath.c_str(), FILE_MODE) == -1) 
    {
        cerr<<"mkdir() can't create directory.";
        return Unres;
    }
   
   //passing Dir as a path expecting to get EISDIR error
	ret_val = creat(dirPath.c_str(), S_IRWXU);
		
    if(ret_val != -1 )
	{
			cerr << "Creat returned 0 but path was Directory";
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


Status CreatTest::CreatNormal()
{	
	const char * filename = "creatNormal.txt"	;	
	int ret_val = creat(filename,S_IRWXU);
	if(ret_val==-1)
	{
			cerr << "creat returned error in case of noraml filename."<<strerror(errno);
			return Fail;
	}
	unlink(filename);
	return Success;
	
	
}


