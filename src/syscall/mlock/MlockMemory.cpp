//      MlockMemory.cpp
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

#include <MlockMemory.hpp>
#include "File.hpp"
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <linux/version.h>

int MlockMemoryTest:: Main(vector<string> args)
{
	if(_mode == Normal)
	{
		switch(_operation)
		{
			case Mlock:
				return MlockTest(args);
			case MlockErrEINVAL:
				return MlockErrEINVALTest(args);
			case MlockErrENOMEM:
				return MlockErrENOMEMTest(args);
			case Munlock:
				return MunlockTest(args);
			case MunlockErrEINVAL:
				return MunlockErrEINVALTest(args);
			case MunlockErrENOMEM:
				return MunlockErrENOMEMTest(args);
			case Mlockall:
				return MlockallTest(args);
			case MlockallErrEINVAL:
				return MlockallErrEINVALTest(args);
			case Munlockall:
				return MunlockallTest(args);
			default:
				return Unknown;
		}
	}
	else
	{
	}
	
	return 0;
}

int MlockMemoryTest:: MlockTest(vector<string> args)
{
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		int fd = file.GetFileDescriptor();
		int prot = PROT_READ;
		int flags = MAP_PRIVATE;
		off_t offset = 0;	
		char * buff = (char *)"abc";
		size_t length = strlen(buff);		
		if(write(fd, buff, length) == -1)
		{
			cerr << "write failed: " << strerror(errno);
			return Unres;
 		}
		void * addr = mmap(0, length, prot, flags, fd, offset);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Unres;	
		}
		
		if(mlock(addr, length) == -1)
		{
			cerr << "System call mlock failed: " << strerror(errno);
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

int MlockMemoryTest:: MlockErrEINVALTest(vector<string> args)
{
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		int fd = file.GetFileDescriptor();
		size_t length = 3;
		int prot = PROT_READ;
		int flags = MAP_PRIVATE;
		off_t offset = 0;
	
		void * addr = mmap(0, length, prot, flags, fd, offset);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Unres;	
		}
		else if(mlock(addr, -1) != -1 || errno != EINVAL)
		{
			cerr << "EINVAL error expected: length is negative";
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

int MlockMemoryTest:: MlockErrENOMEMTest(vector<string> args)
{
	int status = Success;
	if(mlock(0, 3) != -1 || errno != ENOMEM)
	{
		cerr << "ENOMEM error expected: address range doesn't correspond to mapped pages";
		status = Fail;
	}
	
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,9)
	try
	{
		struct rlimit rlim;
		if(getrlimit(RLIMIT_MEMLOCK, &rlim) == -1)
		{
			cerr << "System call getrlimit failed: " << strerror(errno);
			status = Unres;
		}
		
		struct rlimit rlimOld = rlim; 
		
		rlim.rlim_cur = 1;
		if(setrlimit(RLIMIT_MEMLOCK, &rlim) == -1)
		{
			cerr << "System call setrlimit failed: " << strerror(errno);
			status = Unres;		
		}
		
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		int fd = file.GetFileDescriptor();
		size_t length = 3;
		int prot = PROT_READ;
		int flags = MAP_PRIVATE;
		off_t offset = 0;
		void * addr = mmap(0, length, prot, flags, fd, offset);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			status = Unres;	
		}
		else if(mlock(addr, 5) != -1 || errno != ENOMEM)
		{
			cerr << "ENOMEM error expected: RLIMIT_MEMLOCK is set";
			status = Fail;
		}	 

		if(setrlimit(RLIMIT_MEMLOCK, &rlimOld) == -1)
		{
			cerr << "System call setrlimit failed: " << strerror(errno);
			status = Unres;		
		}
	}
	catch(Exception ex)
	{
		cerr << ex.GetMessage();
		status = Unres;
	}
#endif
	
	return status;
}

int MlockMemoryTest:: MunlockTest(vector<string> args)
{
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		int fd = file.GetFileDescriptor();
		size_t length = 3;
		int prot = PROT_READ;
		int flags = MAP_PRIVATE;
		off_t offset = 0;
	
		void * addr = mmap(0, length, prot, flags, fd, offset);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Unres;	
		}
		
		if(munlock(addr, length) == -1)
		{
			cerr << "System call munlock failed: " << strerror(errno);
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

int MlockMemoryTest:: MunlockErrEINVALTest(vector<string> args)
{
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		int fd = file.GetFileDescriptor();
		size_t length = 3;
		int prot = PROT_READ;
		int flags = MAP_PRIVATE;
		off_t offset = 0;
	
		void * addr = mmap(0, length, prot, flags, fd, offset);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Unres;	
		} 
		else if(munlock(addr, -1) != -1 || errno != EINVAL)
		{
			cerr << "EINVAL error expected: length is negative";
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

int MlockMemoryTest:: MunlockErrENOMEMTest(vector<string> args)
{
	if(munlock(0, 3) != -1 || errno != ENOMEM)
	{
		cerr << "ENOMEM error expected: address range doesn't correspond to mapped pages";
		return Fail;
	}
	
	return Success;
}

int MlockMemoryTest:: MlockallTest(vector<string> args)
{
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		int fd = file.GetFileDescriptor();
		int prot = PROT_READ;
		int flags = MAP_PRIVATE ;
		off_t offset = 0;	
		char * buff = (char *)"abc";
		size_t length = strlen(buff);		
		
		if(write(fd, buff, length) == -1)
		{
			cerr << "write failed: " << strerror(errno);
			return Unres;
 		}
		
		void * addr = mmap(0, length, prot, flags, fd, offset);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Unres;	
		}
		
		if(mlockall(MCL_FUTURE) == -1)
		{
			cerr << "System call mlockall failed: " << strerror(errno);
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

int MlockMemoryTest:: MlockallErrEINVALTest(vector<string> args)
{
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		int fd = file.GetFileDescriptor();
		size_t length = 3;
		int prot = PROT_READ;
		int flags = MAP_PRIVATE;
		off_t offset = 0;
	
		void * addr = mmap(0, length, prot, flags, fd, offset);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Unres;	
		}
		if(mlockall(MCL_FUTURE & MCL_CURRENT) != -1 || errno != EINVAL)
		{
			cerr << "EINVAL error expected: flags contain invalid value";
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

int MlockMemoryTest:: MunlockallTest(vector<string> args)
{
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		int fd = file.GetFileDescriptor();
		size_t length = 3;
		int prot = PROT_READ;
		int flags = MAP_PRIVATE;
		off_t offset = 0;
	
		void * addr = mmap(0, length, prot, flags, fd, offset);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Unres;	
		}
		if(munlockall() == -1)
		{
			cerr << "System call munlockall failed: " << strerror(errno);
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
