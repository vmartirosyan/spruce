//      Link.cpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Suren Gishyan <gsuren@gmail.com>
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

#include <Link.hpp>
#include <unistd.h>
#include <fcntl.h>
#include "File.hpp"

int LinkTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case LinkTestTooLongNewPath:
				return LinkTooLongNewPath();
			case LinkTestNewPathAleadyExist:
				return LinkNewPathAlreadyExist();
			case LinkTestOldPathIsDirectory:
				return LinkOldPathIsDirectory();
			case LinkTestNormalFile:
				return LinkNormalLink();
			case LinkTestIsNotDirectory:
				return LinkIsNotDirectory();
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}





Status LinkTest::LinkTooLongNewPath()
{
	
	
	int ret_val;
	const char* filename="link_testfile.txt";
	const char* tooLongfilename="abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	
	try
	{
		File file(filename, S_IWUSR);
		ret_val = link(filename,tooLongfilename);
		
		
		
		if(ret_val==0)
		{
			cerr << "Link return 0 in case of too long new path name";
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
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Fail;
	}
	
}

Status LinkTest::LinkNewPathAlreadyExist()
{

	int fd ;
	int ret_val;
	const char* filename1="link_testfile.txt";
	const char* filename2="link_testfile.txt";
	try
	{
		File file1(filename1, S_IWUSR);
		File file2(filename2, S_IWUSR);
		
		ret_val = link(filename1,filename2);
		
		
		
		if(ret_val==0)
		{
			cerr << "Link return 0 ,then new path is already exist.";
			return Fail;
		}
		else
		{
			if(errno!=EEXIST)
			{
				
				cerr << "Incorrect error set in errno ,then new path is already exist. "<<strerror(errno);
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

Status LinkTest::LinkOldPathIsDirectory()
{

	int fd ;
	int ret_val;
	const char* dirname="testdir";
	const char* filename="linkname.txt";
	fd=mkdir(dirname,S_IWUSR);
	if( fd == -1)
	{
		cerr << "Cannot create directory: Aborting test."<<strerror(errno);
		return Unres;
	}
	
	ret_val = link(dirname,filename);
		
	if(ret_val==0)
	{
		cerr << "Link return 0 then old path is directory.";
		rmdir(dirname);
		return Fail;
	}
	else
	{
		if(errno!=EPERM)
		{
			
			cerr << "Incorrect error set in errno then old path is directory. "<<strerror(errno);
			rmdir(dirname);
			return Fail;
		}
	}
	rmdir(dirname);
	return Success;


}


Status LinkTest::LinkNormalLink()
{

	int fd_orig ;
	int fd_link;
	
	int ret_val;
	const char* buf="ABCDEFGH";
	char read_buf[1024];
	size_t  COUNT=6;
	ssize_t write_count;
	ssize_t read_count;
	const char* filename="link_testfile1.txt";
	const char* filename_link="link_testfile2.txt";

	fd_orig=open(filename,O_RDWR| O_CREAT, S_IRWXU);
	
	if(fd_orig==-1)
	{
		cerr << "Unable create file."<<strerror(errno);
		return Unres;
	}
		
	fd_link = link(filename, filename_link);
	if(fd_link==-1)
	{
		cerr << "Error occured during linking normal file. "<<strerror(errno);
		unlink(filename);
		return Unres;
	}
	

	
	write_count=write(fd_orig, buf, COUNT);
	if(write_count==-1)
	{
		cerr << "Error occured during writing. "<<strerror(errno);
		unlink(filename);
		return Unres;
	}
	

	
	fd_link=open(filename_link,O_RDONLY,S_IRUSR);
	if(fd_link==-1)
	{
		cerr << "Error occured during opening linked file. "<<strerror(errno);
		unlink(filename);
		return Unres;
	}
	
	read_count=read(fd_link, read_buf, write_count);
	if(read_count!=write_count)
	{
		cerr << "Written bytes count is not equal to read bytes count.";
		unlink(filename);
		unlink(filename_link);
		return Fail;
	}
	
	if(memcmp(buf, read_buf, read_count)!=0)
	{
		cerr << "Written bytes are not equal to read bytes count.  ";
		unlink(filename);
		unlink(filename_link);
		return Fail;
	}
	
	
	unlink(filename);
	unlink(filename_link);
	return Success;


}

Status LinkTest::LinkIsNotDirectory()
{

	int ret_val;
	const char* notDirectoryPath="link_testfile.txt/linkname";
	const char* filename2="link_testfile.txt";
	const char* filename="link_testfile1.txt";
	
	
	
	try
	{
		File file(filename, S_IWUSR);
		File file2(filename2, S_IWUSR);
		
		ret_val=link(filename,notDirectoryPath);
					
				
		if(ret_val==0)
		{
				cerr << "Link return 0 in case of non-directory component used as a directory";
				return Fail;
		}
		else
		{
			if(errno!=ENOTDIR)
			{
					
					cerr << "Incorrect error set in errno in case of non-directory component used as a directory "<<strerror(errno);
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

