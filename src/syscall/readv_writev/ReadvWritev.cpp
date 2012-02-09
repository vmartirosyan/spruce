//      ReadvWritev.cpp
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
#include <limits.h>
#include <time.h>
#include <ReadvWritev.hpp>
#include "File.hpp"

const unsigned int BUF_SIZE = 16;  
const unsigned int BUF_COUNT = 2; 
const unsigned int BYTES_COUNT = 5; 
const unsigned int BYTES = 64 * 1024 + 1;

int ReadvWritev::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case ReadvWritevBasicOp1:
				return ReadvWritevBasicFunc1();
			case ReadvEagainErrorOp:
				return ReadvEagainErrorFunc();
			case ReadvBadfdErrorOp1:
				return ReadvBadfdErrorFunc1();
			case ReadvBadfdErrorOp2:
				return ReadvBadfdErrorFunc2();
			case ReadvEfaultErrorOp:
				return ReadvEfaultErrorFunc();
			case ReadvEinvalErrorOp1:
				return ReadvEinvalErrorFunc1();
			case ReadvEinvalErrorOp2:
				return ReadvEinvalErrorFunc2();
			case ReadvEinvalErrorOp3:
				return ReadvEinvalErrorFunc3();
			case ReadvEinvalErrorOp4:
				return ReadvEinvalErrorFunc4();
			case ReadvIsdirErrorOp:
				return ReadvIsdirErrorFunc();
			case WritevBadfdErrorOp1:
				return WritevBadfdErrorFunc1();
			case WritevBadfdErrorOp2:
				return WritevBadfdErrorFunc2();
			case WritevEfaultErrorOp:
				return WritevEfaultErrorFunc();
			case WritevEinvalErrorOp1:
				return WritevEinvalErrorFunc1();
			case WritevEinvalErrorOp2:
				return WritevEinvalErrorFunc2();
			case WritevEinvalErrorOp3:
				return WritevEinvalErrorFunc3();
			default:
				cerr << "Unsupported operation.";
				return Unres;
		}
	}
	
	cerr << "Test was successful";
	return Success;
}

//readv/writev basic functionality:
// writev data to pipe, and readv the same data from the other end of it
Status ReadvWritev::ReadvWritevBasicFunc1()
{
	int pipefd[2];
	int status = pipe(pipefd);
	if (status == -1)
	{
		cerr << "bad";
		cerr << strerror(errno);
		return Unres;
	}
	
	string writeBuf1 = "first";
	string writeBuf2 = "second";
	
	char readBuf1[BUF_SIZE] = {'1'};
	char readBuf2[BUF_SIZE] = {'2'};
	
	struct iovec iovAr[BUF_COUNT];
	
	//preparing struct iovec for writing 
	iovAr[0].iov_base = const_cast<char *>(writeBuf1.c_str());
	iovAr[0].iov_len = writeBuf1.size();
	iovAr[1].iov_base = const_cast<char *>(writeBuf2.c_str());
	iovAr[1].iov_len = writeBuf2.size();
	
		
	ssize_t write_status = writev(pipefd[1], iovAr, BUF_COUNT);
	if (write_status == -1 || write_status != writeBuf1.size() + writeBuf2.size())
	{
		cerr << "writev: " << strerror(errno);
		close(pipefd[0]);
		close(pipefd[1]);
		return Fail;
	}
	
	//preparing struct iovec for reading
	iovAr[0].iov_base = readBuf1;
	iovAr[0].iov_len = writeBuf1.size();
	iovAr[1].iov_base = readBuf2;
	iovAr[1].iov_len = writeBuf2.size();
	
	ssize_t read_status = readv(pipefd[0], iovAr, BUF_COUNT);
	if (read_status == -1 || read_status != writeBuf1.size() + writeBuf2.size())
	{
		cerr << "readv: " << strerror(errno);
		close(pipefd[0]);
		close(pipefd[1]);
		return Fail;
	}
	
	if (strncmp(readBuf1, writeBuf1.c_str(), writeBuf1.size()) != 0)
	{
		cerr << "The data read from the first bufer should be "
			 << writeBuf1 << ", but it is " << readBuf1;
		close(pipefd[0]);
		close(pipefd[1]);
		return Fail;
	}
	
	if (strncmp(readBuf2, writeBuf2.c_str(), writeBuf2.size()) != 0)
	{
		cerr << "The data read from the second bufer should be " 
			 << writeBuf2 << ", but it is " << readBuf2;
		close(pipefd[0]);
		close(pipefd[1]);
		return Fail;
	}
	
	close(pipefd[0]);
	close(pipefd[1]);
	
	return Success;
}

// attempt to readv from nonblocking pipe which is empty from the other side
Status ReadvWritev::ReadvEagainErrorFunc()
{		
	int pipe[2];
	int status = pipe2(pipe, O_NONBLOCK);
	if (status == -1)
	{
		cerr << strerror(errno);
		return Unres;
	}
	
	string buf1;
	string buf2;
		
	struct iovec iovAr[BUF_COUNT];

	iovAr[0].iov_base = const_cast<char *>(buf1.c_str());
	iovAr[0].iov_len = BYTES_COUNT;
	iovAr[1].iov_base = const_cast<char *>(buf2.c_str());
	iovAr[1].iov_len = BYTES_COUNT;
	
	ssize_t readv_status = readv(pipe[0], iovAr, BUF_COUNT);
	if (errno != EAGAIN || readv_status != -1)
	{
		cerr << "Expected to get EAGAIN error";
		return Fail;
	}

	return Success;
}

// attempt to readv from a file, which was opened in write mode
Status ReadvWritev::ReadvBadfdErrorFunc1()
{
	string buf1;
	string buf2;
		
	struct iovec iovAr[BUF_COUNT];
	
	iovAr[0].iov_base = const_cast<char *>(buf1.c_str());
	iovAr[0].iov_len = BYTES_COUNT;
	iovAr[1].iov_base = const_cast<char *>(buf2.c_str());
	iovAr[1].iov_len = BYTES_COUNT;
	
	try
	{
		File file("testfile.txt", S_IWUSR);
		
		size_t fd = open("testfile.txt", O_WRONLY);

		ssize_t status = readv(fd, iovAr, BUF_COUNT);
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

// attempt to readv from -1 file descriptor
Status ReadvWritev::ReadvBadfdErrorFunc2()
{
	string buf1;
	string buf2;
		
	struct iovec iovAr[BUF_COUNT];
	
	iovAr[0].iov_base = const_cast<char *>(buf1.c_str());
	iovAr[0].iov_len = BYTES_COUNT;
	iovAr[1].iov_base = const_cast<char *>(buf2.c_str());
	iovAr[1].iov_len = BYTES_COUNT;

	ssize_t status = readv(-1, iovAr, BUF_COUNT);
	if (errno != EBADF || status != -1)
	{
		cerr << "Expected to get bad file descriptor";
		return Fail;
	}
	
	return Success;
}

// attempt to readv to the buffer which is -1
Status ReadvWritev::ReadvEfaultErrorFunc()
{
	string buf1;
	string buf2;
		
	struct iovec iovAr[BUF_COUNT];
	
	iovAr[0].iov_base = reinterpret_cast<void *>(-1);
	iovAr[0].iov_len = BYTES_COUNT;
	iovAr[1].iov_base = reinterpret_cast<void *>(-1);
	iovAr[1].iov_len = BYTES_COUNT;
	
	try
	{
		File file("testfile.txt");
		
		size_t fd = open("testfile.txt", O_RDONLY);

		ssize_t status = readv(fd, iovAr, BUF_COUNT);
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

// attempt to readv from file descriptor created by timerfd_create()
Status ReadvWritev::ReadvEinvalErrorFunc1()
{	
	int fd = timerfd_create(CLOCK_REALTIME, 0);
    if (fd == -1)
    {
		cerr << strerror(errno);
		return Unres;
	}
               
    string buf1;
	string buf2;
		
	struct iovec iovAr[BUF_COUNT];
	
	iovAr[0].iov_base = const_cast<char *>(buf1.c_str());
	iovAr[0].iov_len = BYTES_COUNT;
	iovAr[1].iov_base = const_cast<char *>(buf2.c_str());
	iovAr[1].iov_len = BYTES_COUNT;
	
    ssize_t status = readv(fd, iovAr, BUF_COUNT);
    if (errno != EINVAL || status != -1)
    {
		cerr << "Expecting to get EINVAL error";
		return Fail;
	}
	
	return Success;
}

// attempt to readv, when third argument is less then 0
Status ReadvWritev::ReadvEinvalErrorFunc2()
{	
    string buf1;
	string buf2;
		
	struct iovec iovAr[BUF_COUNT];
	
	iovAr[0].iov_base = const_cast<char *>(buf1.c_str());
	iovAr[0].iov_len = BYTES_COUNT;
	iovAr[1].iov_base = const_cast<char *>(buf2.c_str());
	iovAr[1].iov_len = BYTES_COUNT;
	
	try
	{
		File file("testfile.txt");
		size_t fd = open("testfile.txt", O_RDWR);
		
		ssize_t status = readv(fd, iovAr, -1);
		if (errno != EINVAL || status != -1)
		{
			cerr << "Expecting to get EINVAL error";
			return Fail;
		}
	} 
	catch (Exception ex) {		
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to readv, when third argument is greater then permitted maximum
Status ReadvWritev::ReadvEinvalErrorFunc3()
{	
    string buf1;
	string buf2;
		
	struct iovec iovAr[BUF_COUNT];
	
	iovAr[0].iov_base = const_cast<char *>(buf1.c_str());
	iovAr[0].iov_len = BYTES_COUNT;
	iovAr[1].iov_base = const_cast<char *>(buf2.c_str());
	iovAr[1].iov_len = BYTES_COUNT;
	
	try
	{
		File file("testfile.txt");
		size_t fd = open("testfile.txt", O_RDWR);
		
		ssize_t status = readv(fd, iovAr, IOV_MAX + 1);
		if (errno != EINVAL || status != -1)
		{
			cerr << "Expecting to get EINVAL error";
			return Fail;
		}
	} 
	catch (Exception ex) {		
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to readv, when sum of iov_len values overflows an ssize_t value
Status ReadvWritev::ReadvEinvalErrorFunc4()
{	
    char buf1[] = "asdf";
    char buf2[] = "qwerty";
		
	struct iovec iovAr[BUF_COUNT];
	
	iovAr[0].iov_base = buf1;
	iovAr[0].iov_len = 0xFFFFFFFF;
	iovAr[1].iov_base = buf2;
	iovAr[1].iov_len = 1;
	
	cerr << "SSIZE_MAX = " << SSIZE_MAX << endl;
	
	try
	{
		File file("testfile.txt");
		size_t fd = open("testfile.txt", O_RDWR);
		
		if(fd == -1)
		{
			cerr<<"Unable open file."<<strerror(errno);
			return Unres;
		}
		
		ssize_t status = readv(fd, iovAr, BUF_COUNT);
		
		if(status != -1)
		{
			cerr<<"readv should return -1 when sum of iov_len values overflows, but it did not";
			return Fail;
		}
				
		if (errno != EINVAL)
		{
			cerr << "Expecting to get EINVAL error, but wrong error set in errno: "<< strerror(errno) <<" errno=" << errno <<"  "<< EBADF << endl;
			return Fail;
		}
	} 
	catch (Exception ex) {		
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to readv from a directory
Status ReadvWritev::ReadvIsdirErrorFunc()
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
	
	string buf1;
	string buf2;
		
	struct iovec iovAr[BUF_COUNT];
	
	iovAr[0].iov_base = const_cast<char *>(buf1.c_str());
	iovAr[0].iov_len = BYTES_COUNT;
	iovAr[1].iov_base = const_cast<char *>(buf2.c_str());
	iovAr[1].iov_len = BYTES_COUNT;

	ssize_t status = readv(fd, iovAr, BUF_COUNT);
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

// attempt to writev to a file, which was opened in read mode
Status ReadvWritev::WritevBadfdErrorFunc1()
{
	string buf1;
	string buf2;
		
	struct iovec iovAr[BUF_COUNT];
	
	iovAr[0].iov_base = const_cast<char *>(buf1.c_str());
	iovAr[0].iov_len = BYTES_COUNT;
	iovAr[1].iov_base = const_cast<char *>(buf2.c_str());
	iovAr[1].iov_len = BYTES_COUNT;
	
	try
	{
		File file("testfile.txt");
		
		//string buf = "message";

		size_t fd = open("testfile.txt", O_RDONLY);

		ssize_t status = writev(fd, iovAr, BUF_COUNT);
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

// attempt to writev to -1 file descriptor
Status ReadvWritev::WritevBadfdErrorFunc2()
{
	string buf1;
	string buf2;
		
	struct iovec iovAr[BUF_COUNT];
	
	iovAr[0].iov_base = const_cast<char *>(buf1.c_str());
	iovAr[0].iov_len = BYTES_COUNT;
	iovAr[1].iov_base = const_cast<char *>(buf2.c_str());
	iovAr[1].iov_len = BYTES_COUNT;

	ssize_t status = writev(-1, iovAr, BUF_COUNT);
	if (errno != EBADF || status != -1)
	{
		cerr << "Expected to get bad file descriptor";
		return Fail;
	}
	
	return Success;
}

// attempt to writev to the buffer which is -1
Status ReadvWritev::WritevEfaultErrorFunc()
{
	string buf1;
		
	struct iovec iovAr[BUF_COUNT];
	
	iovAr[0].iov_base = const_cast<char *>(buf1.c_str());
	iovAr[0].iov_len = BYTES_COUNT;
	iovAr[1].iov_base = reinterpret_cast<void *>(-1);
	iovAr[1].iov_len = BYTES_COUNT;
	
	try
	{
		File file("testfile.txt");
		
		size_t fd = open("testfile.txt", O_WRONLY);

		ssize_t status = writev(fd, iovAr, BUF_COUNT);
		
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

// attempt to writev, when third argument is less then 0
Status ReadvWritev::WritevEinvalErrorFunc1()
{	
    string buf1;
	string buf2;
		
	struct iovec iovAr[BUF_COUNT];
	
	iovAr[0].iov_base = const_cast<char *>(buf1.c_str());
	iovAr[0].iov_len = BYTES_COUNT;
	iovAr[1].iov_base = const_cast<char *>(buf2.c_str());
	iovAr[1].iov_len = BYTES_COUNT;
	
	try
	{
		File file("testfile.txt");
		size_t fd = open("testfile.txt", O_RDWR);
		
		ssize_t status = writev(fd, iovAr, -1);
		if (errno != EINVAL || status != -1)
		{
			cerr << "Expecting to get EINVAL error";
			return Fail;
		}
	} 
	catch (Exception ex) {		
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to writev, when third argument is greater then permitted maximum
Status ReadvWritev::WritevEinvalErrorFunc2()
{	
    string buf1;
	string buf2;
		
	struct iovec iovAr[BUF_COUNT];
	
	iovAr[0].iov_base = const_cast<char *>(buf1.c_str());
	iovAr[0].iov_len = BYTES_COUNT;
	iovAr[1].iov_base = const_cast<char *>(buf2.c_str());
	iovAr[1].iov_len = BYTES_COUNT;
	
	try
	{
		File file("testfile.txt");
		size_t fd = open("testfile.txt", O_RDWR);
		
		ssize_t status = writev(fd, iovAr, IOV_MAX + 1);
		if (errno != EINVAL || status != -1)
		{
			cerr << "Expecting to get EINVAL error";
			return Fail;
		}
	} 
	catch (Exception ex) {		
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to writev, when sum of iov_len values overflows an ssize_t value
Status ReadvWritev::WritevEinvalErrorFunc3()
{	
    string buf1;
	string buf2;
		
	struct iovec iovAr[BUF_COUNT];
	
	iovAr[0].iov_base = const_cast<char *>(buf1.c_str());
	iovAr[0].iov_len = 0xFFFFFFFF;
	iovAr[1].iov_base = const_cast<char *>(buf2.c_str());
	iovAr[1].iov_len = 1;
	
	try
	{
		File file("testfile.txt");
		size_t fd = open("testfile.txt", O_RDWR);
		
		ssize_t status = writev(fd, iovAr, BUF_COUNT);
		if (errno != EINVAL || status != -1)
		{
			cerr << "Expecting to get EINVAL error";
			return Fail;
		}
	} 
	catch (Exception ex) {		
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

