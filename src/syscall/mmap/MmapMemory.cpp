//      MmapMemory.cpp
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

#include <MmapMemory.hpp>
#include "File.hpp"
#include <sys/mman.h>
#include <sys/user.h>
#include <linux/version.h>

int MmapMemoryTest:: Main(vector<string> args)
{
	if (_mode == Normal)
	{		
		switch (_operation)
		{		
			case Mmap:
				return MmapTest(args);
			case MmapErrEINVAL:
				return MmapErrEINVALTest(args);
			case MmapErrEBADF:
				return MmapErrEBADFTest(args);
			case MmapErrEACCES:
				return MmapErrEACCESTest(args);
			case Munmap:
				return MunmapTest(args);
			case MunmapErr:
				return MunmapErrTest(args);
			case Mremap:
				return MremapTest(args);
			case MremapErrEINVAL:
				return MremapErrEINVALTest(args);
			case Mmap2:
				return MmapTest(args);
			case Mmap2ErrEINVAL:
				return MmapErrEINVALTest(args);
			case Mmap2ErrEBADF:
				return MmapErrEBADFTest(args);
			case Mmap2ErrEACCES:
				return MmapErrEACCESTest(args);
			default:
				return Unknown; 
		}
	}
	else
	{		
	}
	return 0;
}


int MmapMemoryTest:: MmapTest(vector<string> args)
{
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR | O_CREAT);
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
			return Fail;	
		}
		
		if(strncmp(buff, (char *)addr, strlen(buff)))
		{
			cerr << "System call mmap failed: mapped content differs from file content";
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

int MmapMemoryTest:: MmapErrEINVALTest(vector<string> args)
{
	int status = Success;	
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR | O_CREAT);
		int fd = file.GetFileDescriptor();

		void * addr = 0;
		size_t length = 10;
		int prot = PROT_READ;
		int flags = MAP_PRIVATE;
		off_t offset = 0;
		
		addr = mmap(0, length, prot, flags, fd, ~PAGE_MASK);
		if(addr != MAP_FAILED || errno != EINVAL)
		{
			cerr << "EINVAL error expected: offset contains invalid value";
			status = Fail;	
		}
		
	#if  LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,12)
		addr = mmap(0, 0, prot, flags, fd, offset);
		if(addr != MAP_FAILED || errno != EINVAL)
		{
			cerr << "EINVAL error expected: length contains 0 value";
			status = Fail;	
		}
	#endif
		
		addr = mmap(0, length, prot, MAP_PRIVATE | MAP_SHARED, fd, offset);
		if(addr != MAP_FAILED || errno != EINVAL)
		{
			cerr << "EINVAL error expected: flags contains invalid value, MAP_PRIVATE and MAP_SHARED are set";
			status = Fail;	
		}

		addr = mmap(0, length, prot, 0, fd, offset);
		if(addr != MAP_FAILED || errno != EINVAL)
		{
			cerr << "EINVAL error expected: flags contains 0 value";
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

int MmapMemoryTest:: MmapErrEBADFTest(vector<string> args)
{
	void * addr = 0;
	size_t length = 10;
	int prot = PROT_READ;
	int flags = MAP_PRIVATE;
	off_t offset = 0;
	int fd = -1;
	
	addr = mmap(0, length, prot, flags, fd, offset);
	if(addr != MAP_FAILED || errno != EBADF)
	{
		cerr << "EBADF error expected: fd is not a valid descriptor";
		return Fail;
	}	
	
	return Success;
}

int MmapMemoryTest:: MmapErrEACCESTest(vector<string> args)
{
	int status = Success;
	
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_WRONLY | O_CREAT);
		int fd = file.GetFileDescriptor();
		void * addr = 0;
		size_t length = 10;
		int prot = PROT_READ;
		int flags = MAP_PRIVATE;
		off_t offset = 0;

		addr = mmap(0, length, prot, flags, fd, offset);		
		if(addr != MAP_FAILED || errno != EACCES)
		{
			cerr << "EACCES error expected: MAP_PRIVATE was requested, but fd is not opened for reading";
			status = Fail;
		}
		
		addr = mmap(0, length, PROT_WRITE, MAP_SHARED, fd, offset);
		if(addr != MAP_FAILED || errno != EACCES)
		{
			cerr << "EACCES error expected: fd is not opened in read/write mode";
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

int MmapMemoryTest:: MunmapTest(vector<string> args)
{
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR | O_CREAT);
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
		
		if(munmap(addr, length) == -1)
		{
			cerr << "System call munmap failed: " << strerror(errno);
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

int MmapMemoryTest:: MunmapErrTest(vector<string> args)
{
	int status = Success;
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR | O_CREAT);
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
		
		if(munmap(addr, -1) != -1)
		{
			cerr << "Error expected: length contains invalid value";
			status = Fail;
		}
		
		int pageSize = getpagesize();
		if(munmap((void *)(pageSize + 1), length) != -1)
		{
			cerr << "Error expected: addr contains invalid value";
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

int MmapMemoryTest:: MremapTest(vector<string> args)
{
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR | O_CREAT);
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
	
		void * oldAddr = mmap(0, length, prot, flags, fd, offset);
		if(oldAddr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Unres;	
		}
		
		void * newAddr = mremap(oldAddr, length, length + 3, MREMAP_MAYMOVE);
		if(newAddr == MAP_FAILED)
		{
			cerr << "System call mremap failed: " << strerror(errno);
			return Fail;
		}
		
		if(strncmp((char *)oldAddr, (char *)newAddr, strlen((char *)oldAddr)))
		{
			cerr << "System call mmap failed: mapped content differs from initial content";
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

int MmapMemoryTest:: MremapErrEINVALTest(vector<string> args)
{
	int status = Success;
	
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR | O_CREAT);
		int fd = file.GetFileDescriptor();
		size_t length = 3;
		int prot = PROT_READ;
		int flags = MAP_PRIVATE;
		off_t offset = 0;
	
		void * oldAddr = mmap(0, length, prot, flags, fd, offset);
		if(oldAddr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Unres;	
		}
		
		void * newAddr = mremap(oldAddr, length, length + 3, MREMAP_MAYMOVE | MREMAP_FIXED);
		if(newAddr != MAP_FAILED || errno != EINVAL)
		{
			cerr << "EINVAL error expected: flags contain invalid value" << strerror(errno);
			status = Fail;
		}

		newAddr = mremap(oldAddr, length, 0, MREMAP_MAYMOVE);
		if(newAddr != MAP_FAILED || errno != EINVAL)
		{
			cerr << "EINVAL error expected: new size is set to zero" << strerror(errno);
			status = Fail;
		}
		
		newAddr = mremap(oldAddr, length, -1, MREMAP_MAYMOVE);
		if(newAddr != MAP_FAILED || errno != EINVAL)
		{
			cerr << "EINVAL error expected: new size contains invalid value" << strerror(errno);
			status = Fail;
		}
		
		newAddr = mremap(oldAddr, length, length + 3, MREMAP_FIXED);
		if(newAddr != MAP_FAILED || errno != EINVAL)
		{
			cerr << "EINVAL error expected: MREMAP_FIXED was specified without also specifying MREMAP_MAYMOVE" << strerror(errno);
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

int MmapMemoryTest:: Mmap2Test(vector<string> args)
{	
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR | O_CREAT);
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
		off64_t offset = 0;
	
		void * addr = mmap64(0, length, prot, flags, fd, offset);
		if(addr == MAP_FAILED)
		{
			cerr << "System call mmap failed: " << strerror(errno);
			return Fail;	
		}
		
		if(strncmp(buff, (char *)addr, strlen(buff)))
		{
			cerr << "System call mmap failed: mapped content differs from file content";
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

int MmapMemoryTest:: Mmap2ErrEINVALTest(vector<string> args)
{
	int status = Success;
	
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR | O_CREAT);
		int fd = file.GetFileDescriptor();
		void * addr = 0;
		size_t length = 10;
		int prot = PROT_READ;
		int flags = MAP_PRIVATE;
		off_t offset = 0;
		
		addr = mmap64(0, length, prot, flags, fd, ~PAGE_MASK);
		if(addr != MAP_FAILED || errno != EINVAL)
		{
			cerr << "EINVAL error expected: offset contains invalid value";
			status = Fail;	
		}
	
		addr = mmap(0, 0, prot, flags, fd, offset);
		if(addr != MAP_FAILED || errno != EINVAL)
		{
			cerr << "EINVAL error expected: length contains 0 value";
			status = Fail;	
		}
	
		addr = mmap(0, length, prot, MAP_PRIVATE | MAP_SHARED, fd, offset);
		if(addr != MAP_FAILED || errno != EINVAL)
		{
			cerr << "EINVAL error expected: flags contains invalid value, MAP_PRIVATE and MAP_SHARED are set";
			status = Fail;	
		}

		addr = mmap(0, length, prot, 0, fd, offset);
		if(addr != MAP_FAILED || errno != EINVAL)
		{
			cerr << "EINVAL error expected: flags contains 0 value";
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

int MmapMemoryTest:: Mmap2ErrEBADFTest(vector<string> args)
{
	void * addr = 0;
	size_t length = 10;
	int prot = PROT_READ;
	int flags = MAP_PRIVATE;
	off_t offset = 0;
	int fd = -1;
	
	addr = mmap64(0, length, prot, flags, fd, offset);
	if(addr != MAP_FAILED || errno != EBADF)
	{
		cerr << "EBADF error expected: fd is not a valid descriptor";
		return Fail;
	}	
	
	return Success;
}

int MmapMemoryTest:: Mmap2ErrEACCESTest(vector<string> args)
{
	int status = Success;
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_WRONLY | O_CREAT);
		int fd = file.GetFileDescriptor();
		void * addr = 0;
		size_t length = 10;
		int prot = PROT_READ;
		int flags = MAP_PRIVATE;
		off_t offset = 0;

		addr = mmap64(0, length, prot, flags, fd, offset);		
		if(addr != MAP_FAILED || errno != EACCES)
		{
			cerr << "EACCES error expected: MAP_PRIVATE was requested, but fd is not opened for reading";
			status = Fail;
		}
		
		addr = mmap(0, length, PROT_WRITE, MAP_SHARED, fd, offset);
		if(addr != MAP_FAILED || errno != EACCES)
		{
			cerr << "EACCES error expected: fd is not opened in read/write mode";
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
