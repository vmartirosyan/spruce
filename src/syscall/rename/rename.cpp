//      rename.cpp
//
// 		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//
//      Author: Eduard Bagrov <ebagrov@gmail.com>
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

#include <rename.hpp>
#include <fcntl.h>
#include <stdio.h>
#include "File.hpp"
#include "Directory.hpp"

int RenameTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case RenameEfaultError1:
				return RenameEfaultError1Test();
			case RenameEfaultError2:
				return RenameEfaultError2Test();
			case RenameEbusyError:
				return RenameEbusyErrorTest();
			case RenameEnametoolongError:
				return RenameEnametoolongErrorTest();
			case RenameEisdirError:
				return RenameEisdirErrorTest();
			case RenameEnotdirError:
				return RenameEnotdirErrorTest();
			case proba:
				return probaTest();
			default:
				cerr << "Unsupported operation.";
				return Unres;
		}
	}
	cerr << "Test was successful";
	return Success;
}

// attempt to rename with -1 name
Status RenameTest::RenameEfaultError1Test()
{
	int status = rename("oldname", (char *)-1);
	if (errno != EFAULT || status != -1)
	{
		cerr << strerror(errno);
		return Fail;
	}
	
	return Success;
}

// attempt -1 to rename
Status RenameTest::RenameEfaultError2Test()
{
	int status = rename((char *)-1, "newname");
	if (errno != EFAULT || status != -1)
	{
		cerr << strerror(errno);
		return Fail;
	}

	return Success;
}

// attempt to rename "."
Status RenameTest::RenameEbusyErrorTest()
{
	int status = rename(".", "newname");
	if (errno != EBUSY || status != -1)
	{
		cerr << strerror(errno);
		return Fail;
	}

	return Success;
}

// attempt to rename file with very long string
Status RenameTest::RenameEnametoolongErrorTest()
{
	try
	{
		File file("name");
		
		char *newname = new char[1000];
		if (newname == NULL)
		{
			cerr << "Unable to allocate memory for name of the name";
			return Unres;
		}
		memset(newname, 'a', 1000);
		
		int status = rename("name", newname);
		
		if (errno != ENAMETOOLONG || status != -1)
		{
			cerr << strerror(errno);
			delete newname;
			return Fail;
		}
		
		delete newname;
	}
	catch(Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	return Success;
}

// attempt to rename the filename with the name of existed directory
Status RenameTest::RenameEisdirErrorTest()
{
	try
	{
		File file("nameas");
		Directory dir("dir", 0777);
		
		int status = rename("nameas", "dir");
		if (errno != EISDIR || status != -1)
		{
			cerr << strerror(errno);
			return Fail;
		}
	}
	catch(Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	return Success;
}

// attempt to rename directory with the name of existing filename
Status RenameTest::RenameEnotdirErrorTest()
{
	try
	{
		File file("2");
		Directory dir("1");
		
		errno = 0;
		int status = rename("1", "2");				
		if (errno != ENOTDIR || status != -1)
		{
			cerr << strerror(errno);
			return Fail;
		}
	}
	catch(Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	return Success;
}

Status RenameTest::probaTest()
{
	try
	{
		File file("2");
		Directory dir("1");
		
		errno = 0;
		int status = rename("1", "2");
		
		cerr << errno << ' ';
		cerr << EINVAL << ' ';
		cerr << EISDIR << ' ';
				
		if (errno != EISDIR || status != -1)
		{
			cerr << strerror(errno);
			return Fail;
		}
	}
	catch(Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	return Success;
}
