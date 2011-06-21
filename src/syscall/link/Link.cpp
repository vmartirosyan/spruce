//      Link.cpp
//      
//      Copyright 2011  Suren Gishyan
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

#include "Link.hpp"
#include <unistd.h>
#include <fcntl.h>
#include "File.hpp"

int LinkTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case TooLongNewPath:
				return LinkTooLongNewPath();
			case TooLongOldPath:
			   return LinkTooLongOldPath();
			case NewPathAleadyExist:
				return LinkNewPathAlreadyExist();
			case OldPathIsDirectory:
				return LinkOldPathIsDirectory();
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

Status LinkTest::LinkTooLongOldPath()
{	
	
	
	const char* filename="testfile.txt";
	const char* tooLongfilename="abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	int ret_val = link(filename,tooLongfilename);
	if(ret_val==0)
	{
			cerr << "Link return 0 in case of too long old path name";
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



Status LinkTest::LinkTooLongNewPath()
{
	
	
	char buf[1024];
	size_t count = 10;
	int fd ;
	int ret_val;
	const char* filename="testfile.txt";
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
	char buf[1024];
	size_t count = 10;
	int fd ;
	int ret_val;
	const char* filename1="testfile1.txt";
	const char* filename2="testfile1.txt";
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
	char buf[1024];
	size_t count = 10;
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


