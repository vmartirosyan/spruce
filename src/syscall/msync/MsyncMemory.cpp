//      MsyncMemory.cpp
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

#include "MsyncMemory.hpp"
#include "File.hpp"
#include <sys/mman.h>
#include <sys/user.h>

int MsyncMemoryTest:: Main(vector<string> args)
{
	if(_mode == Normal)
	{
		switch(_operation)
		{
			case Msync:
				return MsyncTest(args);
			case MsyncErrEINVAL:
				return MsyncErrEINVALTest(args);
			case MsyncErrENOMEM:
				return MsyncErrENOMEMTest(args);
			default:
				return Unknown;
		}
	}
	else
	{
	}
	return 0;
}

int MsyncMemoryTest:: MsyncTest(vector<string> args)
{
	int status = Success;
	try
	{
		File file("file");
		int fd = file.GetFileDescriptor();
		char * buff = (char *)"abc";
		if(write(fd, buff, strlen(buff)) == -1)
		{
			cerr << "System call write failed: " << strerror(errno);
			return Unres;
 		}
			
		void * addr = mmap(0, strlen(buff), PROT_READ, MAP_PRIVATE, fd, 0);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Unres;	
		}
		
		if(msync(addr, strlen(buff), MS_INVALIDATE | MS_SYNC) == -1)
		{
			cerr << "System call msync failed: " << strerror(errno);
			status = Fail;
		}
		
		char * readBuff = 0;
		if(read(fd, readBuff, strlen(buff)) == -1)
		{
			cerr << "System call read failed: " <<strerror(errno);
			status = Unres;
		}
		else if(!strncmp(buff, (char *)readBuff, strlen(buff)))
		{
			cerr << "System call msync failed: file was not synchronized";
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

int MsyncMemoryTest:: MsyncErrEINVALTest(vector<string> args)
{
	int status = Success;
	try
	{
		File file("file");
		int fd = file.GetFileDescriptor();
		char * buff = (char *)"abc";
		if(write(fd, buff, strlen(buff)) == -1)
		{
			cerr << "System call write failed: " << strerror(errno);
			return Unres;
 		}
			
		void * addr = mmap(0, strlen(buff), PROT_READ, MAP_PRIVATE, fd, 0);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Unres;	
		}
		
		if(msync(addr, strlen(buff), MS_ASYNC | MS_INVALIDATE | MS_SYNC) != -1 || errno != EINVAL)
		{
			cerr << "EINVAL error expected: flags contain invalid value" << endl;
			status = Fail;
		}
		
		if(msync(addr, strlen(buff), MS_ASYNC | MS_SYNC) != -1 || errno != EINVAL)
		{
			cerr << "EINVAL error expected: flags contain invalid value" << endl;
			status = Fail;
		}
		
		if(msync((void *)~PAGE_MASK, strlen(buff), MS_INVALIDATE | MS_SYNC) != -1 || errno != EINVAL)
		{
			cerr << "EINVAL error expected: address contains invalid value" << endl;
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

int MsyncMemoryTest:: MsyncErrENOMEMTest(vector<string> args)
{
	int status = Success;
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
			status = Unres;
		}
		else if(msync(addr, length, MS_SYNC | MS_INVALIDATE) != -1 || errno != ENOMEM)
		{
			cerr << "ENOMEM error expected: the indicated memory was not mapped";
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
