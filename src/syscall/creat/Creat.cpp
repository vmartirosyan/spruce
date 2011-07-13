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

#include <Creat.hpp>
#include <unistd.h>
#include <fcntl.h>
#include "File.hpp"

int CreatTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case CreatTestTooLongPath:
				return CreatTooLongPath();
			case CreatTestFileExist:
				return CreatFileExist();
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
	if(ret_val==0)
	{
			cerr << "creat returned 0 in case of too long old path name";
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



Status CreatTest::CreatFileExist()
{	
	
	int fd ;
	int ret_val;
	const char* filename="Creattestfile.txt";

	try
	{
		File file(filename, S_IRWXU);		
		ret_val = creat(filename,S_IRWXU);	
		
		
		if(ret_val==0)
		{
			cerr << "Creat returned 0 ,then path is already exist.";
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

