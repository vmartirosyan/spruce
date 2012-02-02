//      PreadPwrite.cpp
//      
// 		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author: 
// 			Tigran Piloyan <tigran.piloyan@gmail.com>
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

#include <sys/timerfd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits>
#include <time.h>
#include <PreadPwrite.hpp>
#include "File.hpp"

const unsigned int BUF_SIZE = 16;  
const unsigned int BYTES_COUNT = 13; 
const unsigned int BYTES = 64 * 1024 + 1;
const off_t OFFSET = 3;

int PreadPwrite::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case PreadPwriteBasicOp1:
				return PreadPwriteBasicFunc1();
			case PreadPwriteBasicOp2:
				return PreadPwriteBasicFunc2();
			case PreadEspipeErrorOp:
				return PreadEspipeErrorFunc();
			case PreadBadfdErrorOp1:
				return PreadBadfdErrorFunc1();
			case PreadBadfdErrorOp2:
				return PreadBadfdErrorFunc2();
			case PreadBadfdErrorOp3:
				return PreadBadfdErrorFunc3();
			case PreadEfaultErrorOp:
				return PreadEfaultErrorFunc();
			case PreadEinvalErrorOp1:
				return PreadEinvalErrorFunc1();
			case PreadEinvalErrorOp2:
				return PreadEinvalErrorFunc2();
			case PreadIsdirErrorOp:
				return PreadIsdirErrorFunc();
			case PwriteBadfdErrorOp1:
				return PwriteBadfdErrorFunc1();
			case PwriteBadfdErrorOp2:
				return PwriteBadfdErrorFunc2();
			case PwriteBadfdErrorOp3:
				return PwriteBadfdErrorFunc3();
			case PwriteEfaultErrorOp:
				return PwriteEfaultErrorFunc();
			case PwriteEspipeErrorOp:
				return PwriteEspipeErrorFunc();
			case PwriteEinvalErrorOp1:
				return PwriteEinvalErrorFunc1();
			case PwriteEinvalErrorOp2:
				return PwriteEinvalErrorFunc2();
			default:
				cerr << "Unsupported operation.";
				return Unres;
		}
	}
	
	cerr << "Test was successful";
	return Success;
}

//pread/pwrite basic functionality:
//write data to the file from the given offset, 
//and read the same data from it
Status PreadPwrite::PreadPwriteBasicFunc1()
{
	try {
		File file("testfile.txt");
			
		size_t fd = open("testfile.txt", O_RDWR);
		
		string writeBuf = "Something to write!!!";
		ssize_t write_status = pwrite(fd, writeBuf.c_str(), BYTES_COUNT, OFFSET);
		if (write_status == -1 || write_status != BYTES_COUNT)
		{
			cerr << "writev: " << strerror(errno);
			return Fail;
		}
		
		char readBuf[BUF_SIZE] = {'a'};
		ssize_t read_status = pread(fd, readBuf, BYTES_COUNT, OFFSET);
		if (read_status == -1 || read_status != BYTES_COUNT)
		{
			cerr << "pread: " << strerror(errno);
			return Fail;
		}
		
		if (strncmp(readBuf, writeBuf.c_str(), BYTES_COUNT) != 0)
		{
			cerr << "The data was written: " << writeBuf.substr(0, BYTES_COUNT) << "\n"
				 << "The data was read: " << readBuf;
			return Fail;
		}
	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

//pread/pwrite basic functionality:
//write data to the file from the begining, 
//and read data from it with the given offset
Status PreadPwrite::PreadPwriteBasicFunc2()
{
	try {
		File file("testfile.txt");
			
		size_t fd = open("testfile.txt", O_RDWR);
		
		string writeBuf = "write with offset";
		write(fd, writeBuf.c_str(), 2*BYTES_COUNT);
		
		ssize_t write_status = pwrite(fd, writeBuf.c_str(), BYTES_COUNT, 0);
		if (write_status == -1 || write_status != BYTES_COUNT)
		{
			cerr << "writev: " << strerror(errno);
			return Fail;
		}
		
		char readBuf[BUF_SIZE] = {'a'};
		ssize_t read_status = pread(fd, readBuf, BYTES_COUNT, OFFSET);
		if (read_status == -1 || read_status != BYTES_COUNT)
		{
			cerr << "pread: " << strerror(errno);
			return Fail;
		}
		
		string subStr = writeBuf.substr(OFFSET, BYTES_COUNT);
		if (strncmp(readBuf, subStr.c_str(), BYTES_COUNT) != 0)
		{
			cerr << "The data was written: " << subStr << "\n"
				 << "The data was read: " << readBuf;
			return Fail;
		}
	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to pread from a pipe
Status PreadPwrite::PreadEspipeErrorFunc()
{	
	char buf[BUF_SIZE];
		
	int pipe[2];
	int status = pipe2(pipe, 0);
	if (status == -1)
	{
		cerr << strerror(errno);
		return Unres;
	}
	
	ssize_t pread_status = pread(pipe[0], buf, BYTES_COUNT, OFFSET);
	if (errno != ESPIPE || pread_status != -1)
	{
		cerr << "Expected to get ESPIPE error";
		return Fail;
	}

	return Success;
}

// attempt to pread from a file, which was opened in write mode
Status PreadPwrite::PreadBadfdErrorFunc1()
{
	char buf[BUF_SIZE];
	
	try
	{
		File file("testfile.txt", S_IWUSR);
		
		size_t fd = open("testfile.txt", O_WRONLY);
		close(fd);

		ssize_t status = pread(fd, buf, BYTES_COUNT, OFFSET);
		if (errno != EBADF || status != -1)
		{
			cerr << "Expected to get bad file descriptor";
			return Fail;
		}
	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to pread from -1 file descriptor
Status PreadPwrite::PreadBadfdErrorFunc2()
{
	char buf[BUF_SIZE];
	
	ssize_t status = pread(-1, buf, BYTES_COUNT, OFFSET);
	if (errno != EBADF || status != -1)
	{
		cerr << "Expected to get bad file descriptor";
		return Fail;
	}
	
	return Success;
}

// attempt to pread from a file, that was closed
Status PreadPwrite::PreadBadfdErrorFunc3()
{
	char buf[BUF_SIZE];
	
	try
	{
		File file("testfile.txt", S_IWUSR);
		
		size_t fd = open("testfile.txt", O_WRONLY);
		close(fd);

		ssize_t status = pread(fd, buf, BYTES_COUNT, OFFSET);
		if (errno != EBADF || status != -1)
		{
			cerr << "Expected to get bad file descriptor";
			return Fail;
		}
	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to pread to the buffer which is -1
Status PreadPwrite::PreadEfaultErrorFunc()
{
	try
	{
		File file("testfile.txt");
		
		size_t fd = open("testfile.txt", O_RDONLY);

		ssize_t status = pread(fd, reinterpret_cast<void *>(-1), BYTES_COUNT, OFFSET);
		if (errno != EFAULT || status != -1)
		{
			cerr << "Expected to get EFAULT error";
			return Fail;
		}
	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to pread, when the offset is negative
Status PreadPwrite::PreadEinvalErrorFunc1()
{	
	char buf[BUF_SIZE];
	
	try
	{
		File file("testfile.txt");
		size_t fd = open("testfile.txt", O_RDWR);
		
		ssize_t status = pread(fd, buf, BYTES_COUNT, -1);
		if (errno != EINVAL || status != -1)
		{
			cerr << "Expected to get EINVAL error";
			return Fail;
		}
	} 
	catch (Exception ex) {		
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to pread, when offset is beyond the end of a seekable device
Status PreadPwrite::PreadEinvalErrorFunc2()
{	
	char buf[BUF_SIZE];
	
	try
	{
		File file("testfile.txt");
		size_t fd = open("testfile.txt", O_RDWR);
		
		ssize_t status = pread(fd, buf, BYTES_COUNT, std::numeric_limits<off_t>::max() + 1);
		if (errno != EINVAL || status != -1)
		{
			cerr << "Expected to get EINVAL error";
			return Fail;
		}
	} 
	catch (Exception ex) {		
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to pread from a directory
Status PreadPwrite::PreadIsdirErrorFunc()
{
	int dir = mkdir("directory", 777);
	if (dir == -1)
	{
		cerr << strerror(errno);
		return Unres;
	}
	
	size_t fd = open("directory", O_DIRECTORY);
	if (fd == -1)
	{
		cerr << strerror(errno);
		rmdir("directory");
		return Unres;
	}
	
	char buf[BUF_SIZE];
	
	ssize_t status = pread(fd, buf, BYTES_COUNT, OFFSET);
	if (errno != EISDIR || status != -1)
	{
		cerr << "Expecting to get EISDIR error";
		rmdir("directory");
		return Fail;
	}
		
	int rmstatus = rmdir("directory");
	if (rmstatus == -1)
	{
		return Unknown;
	}
	
	return Success;
}

// attempt to pwrite to a file, which was opened in read mode
Status PreadPwrite::PwriteBadfdErrorFunc1()
{
	char buf[BUF_SIZE];
	
	try
	{
		File file("testfile.txt");
		
		size_t fd = open("testfile.txt", O_RDONLY);

		ssize_t status = pwrite(fd, buf, BYTES_COUNT, OFFSET);
		if (errno != EBADF || status != -1)
		{
			cerr << "Expected to get bad file descriptor";
			return Fail;
		}
	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to pwrite to -1 file descriptor
Status PreadPwrite::PwriteBadfdErrorFunc2()
{
	char buf[BUF_SIZE];

	ssize_t status = pwrite(-1, buf, BYTES_COUNT, OFFSET);
	if (errno != EBADF || status != -1)
	{
		cerr << "Expected to get bad file descriptor";
		return Fail;
	}
	
	return Success;
}

// attempt to pwrite from a file, that was closed
Status PreadPwrite::PwriteBadfdErrorFunc3()
{
	char buf[BUF_SIZE] = {'a'};
	
	try
	{
		File file("testfile.txt", S_IWUSR);
		
		size_t fd = open("testfile.txt", O_WRONLY);
		close(fd);

		ssize_t status = pwrite(fd, buf, BYTES_COUNT, OFFSET);
		if (errno != EBADF || status != -1)
		{
			cerr << "Expected to get bad file descriptor";
			return Fail;
		}
	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to pwrite to the buffer which is -1
Status PreadPwrite::PwriteEfaultErrorFunc()
{
	try
	{
		File file("testfile.txt");
		
		size_t fd = open("testfile.txt", O_WRONLY);

		ssize_t status = pwrite(fd, reinterpret_cast<void *>(-1), BYTES_COUNT, OFFSET);
		
		if (errno != EFAULT || status != -1)
		{
			cerr << "Expected to get EFAULT";
			return Fail;
		}
	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to pwrite to a pipe 
Status PreadPwrite::PwriteEspipeErrorFunc()
{
	char buf[BUF_SIZE] = {'a'};
	
	int pipe[2];
	int status = pipe2(pipe, 0);
	if (status == -1)
	{
		cerr << strerror(errno);
		return Unres;
	}
	
	
	ssize_t pwrite_status = pwrite(pipe[1], buf, BYTES_COUNT, OFFSET);
	if (errno != ESPIPE || pwrite_status != -1)
	{
		cerr << "Expected to get EAGAIN error";
		return Fail;
	}

	return Success;
}

// attempt to pwrite, when the offset is negative
Status PreadPwrite::PwriteEinvalErrorFunc1()
{	
	char buf[BUF_SIZE];
	
	try
	{
		File file("testfile.txt");
		size_t fd = open("testfile.txt", O_RDWR);
		
		ssize_t status = pwrite(fd, buf, BYTES_COUNT, -1);
		if (errno != EINVAL || status != -1)
		{
			cerr << "Expected to get EINVAL error";
			return Fail;
		}
	} 
	catch (Exception ex) {		
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to pread, when the offset is beyond the end of a seekable device
Status PreadPwrite::PwriteEinvalErrorFunc2()
{	
    char buf[BUF_SIZE];
	
	try
	{
		File file("testfile.txt");
		size_t fd = open("testfile.txt", O_RDWR);
		
		ssize_t status = pwrite(fd, buf, BYTES_COUNT, std::numeric_limits<off_t>::max() + 1);
		if (errno != EINVAL || status != -1)
		{
			cerr << "Expected to get EINVAL error";
			return Fail;
		}
	} 
	catch (Exception ex) {		
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

//// attempt to writev, when sum of iov_len values overflows an ssize_t value
//Status PreadPwrite::WritevEinvalErrorFunc3()
//{	
    //string buf1;
	//string buf2;
		
	//struct iovec iovAr[BUF_COUNT];
	
	//iovAr[0].iov_base = const_cast<char *>(buf1.c_str());
	//iovAr[0].iov_len = SSIZE_MAX;
	//iovAr[1].iov_base = const_cast<char *>(buf2.c_str());
	//iovAr[1].iov_len = 1;
	
	//try
	//{
		//File file("testfile.txt");
		//size_t fd = open("testfile.txt", O_RDWR);
		
		//ssize_t status = writev(fd, iovAr, BUF_COUNT);
		//if (errno != EINVAL || status != -1)
		//{
			//cerr << "Expecting to get EINVAL error";
			//return Fail;
		//}
	//} 
	//catch (Exception ex) {		
		//cerr << ex.GetMessage();
		//return Unres;
	//}
	
	//return Success;
//}

