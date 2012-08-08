//      MprotectMemory.cpp
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

#include "MprotectMemory.hpp"
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
		File file("file");
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
	try
	{
		File file1("file1", S_IRUSR | S_IWUSR, O_RDONLY | O_CREAT);
		int fd = file1.GetFileDescriptor();
		size_t length = 3;
		int prot = PROT_READ;
		int flags = MAP_SHARED;
		off_t offset = 0;
	
		void * addr = mmap(0, length, prot, flags, fd, offset);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Unres;	
		}
		if(mprotect(addr, length, PROT_WRITE) != -1 || errno != EACCES)
		{
			cerr << "EACCES error expected: file is read-only, PROT_WRITE can't be set";
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

int MprotectMemoryTest:: MprotectErrEINVALTest(vector<string> args)
{
	int status = Success;
	
	try
	{		
		const int FileCount = 1;
		int FileFlags = O_CREAT | O_RDWR;
		int FileMode = S_IRWXU;
		
		string FilePaths[FileCount];
		File Files[FileCount];
		int FDs[FileCount];
		for ( int i = 0 ; i < FileCount; ++i )
		{
			char buf[2];
			sprintf(buf, "%d", i);
			FilePaths[i] = "MprotectErrInval_file_" + (string)buf;
			FDs[i] = Files[i].Open(FilePaths[i], FileMode, FileFlags);
		}	
		
		int length = 3;	
		void * addr = mmap(0, length, PROT_READ, MAP_PRIVATE, FDs[0], 0);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Unres;
		}
		
		if(mprotect(addr + 1, length, PROT_NONE) != -1 || errno != EINVAL)
		{
			cerr << "EINVAL error expected: memory was not mapped";
			status = Fail;
		}
		
		if(mprotect((void *)(-1), 3, PROT_NONE) != -1 || errno != EINVAL)
		{
			cerr << "EINVAL error expected: addr is not a valid pointer";
			status = Fail;
		}
		
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}
		
	return status;
}

int MprotectMemoryTest:: MprotectErrENOMEMTest(vector<string> args)
{
	try
	{
		File file("file");
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
