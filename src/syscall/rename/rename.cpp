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
#include "exception.hpp"

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

Status RenameTest::probaTest()
{
	try
	{
		File ("name");
		/*int status = creat("asasas", 0777);
		if (status == -1)
		{
			cerr << "1";
			cerr << strerror(errno);
			return Unres;
		}*/
		
		int status = mkdir("dir", 0777);
		if (status == -1)
		{
			cerr << "mkdir2";
			cerr << strerror(errno);
			return Unres;
		}
		
		errno = 0;
		status = rename("name", "dir/.");
		status = creat("name/asasas", 0777);
		if (status == -1)
		{
			cerr << "1";
			cerr << strerror(errno);
			return Unres;
		}
		cerr << errno << ' ';
		
		cerr << EINVAL << ' ';
		cerr << EISDIR << ' ';
		if (errno != EFAULT || status != -1)
		{
			cerr << strerror(errno);
			//rmdir("dir");
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
