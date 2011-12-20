//      DupFileDescriptor.hpp
//      
//		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//      	Ruzanna Karakozova <r.karakozova@gmail.com>
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

#include <MprotectMemory.hpp>
#include <sys/mman.h>
#include "File.hpp"

int MprotectMemoryTest:: Main(vector<string> args)
{
	if(_mode == Normal)
	{
		switch(_operation)
		{
			case Mprotect:
				return MprotectTest(args);
			case MprotectErrEACCES:
				return MprotectErrEACCESTest(args);
			case MprotectErrENOMEM:
				return MprotectErrENOMEMTest(args);
			default:
				return Unknown;
		}
	}
	else
	{
	}
	return 0;
}

int MprotectMemoryTest:: MprotectTest(vector<string> args)
{
	int status = Success;
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		int fd = file.GetFileDescriptor();
		char * buff = (char *)"abc";
		if(write(fd, buff, strlen(buff)) == -1)
		{
			cerr << "write failed: " << strerror(errno);
			return Unres;
 		}
		
		size_t length = strlen(buff);
		int prot = PROT_READ;
		int flags = MAP_PRIVATE;
		off_t offset = 0;
	
		void * addr = mmap(0, length, prot, flags, fd, offset);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Unres;	
		}
		
		//memory can not be accessed
		if(mprotect(addr, length, PROT_NONE) == -1)
		{
			cerr << "System call mprotect failed: " << strerror(errno);
			status = Fail;
		}		
		else
		{
			
		}
		
	}
	catch(Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	return status;
}

int MprotectMemoryTest:: MprotectErrEACCESTest(vector<string> args)
{
	int status = Success;
	try
	{
		File file("file1", S_IRUSR | S_IWUSR, O_RDONLY);
		int fd = file.GetFileDescriptor();
		size_t length = 3;
		int prot = PROT_READ;
		int flags = MAP_SHARED;
		off_t offset = 0;
	
		void * addr = mmap(0, length, prot, flags, fd, offset);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			status = Unres;	
		}
		else if(mprotect(addr, length, PROT_WRITE) != -1 || errno != EACCES)
		{
			cerr << "EACCES error expected: file is read-only, PROT_WRITE can't be set" << strerror(errno);
			status = Fail;
		}		
	}
	catch(Exception ex)
	{
		cerr << ex.GetMessage();
		status = Unres;
	}
	
	return status;
}

int MprotectMemoryTest:: MprotectErrENOMEMTest(vector<string> args)
{
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		int fd = file.GetFileDescriptor();
		int length = 3;	
		void * addr = mmap(0, length, PROT_READ, MAP_PRIVATE, fd, 0);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Unres;	
		}
		
		if(munmap(addr, length) == -1)
		{
			cerr << "System call munmap failed: " << strerror(errno);
			return Unres;
		}
		
		if(mprotect(addr, length, PROT_NONE) != -1 || errno != ENOMEM)
		{
			cerr << "ENOMEM error expected: memory was not mapped";
			return Fail;
		}
	}
	catch(Exception err)
	{
		cerr << err.GetMessage() << endl;
		return Unres;
	}
	return Success;
}
