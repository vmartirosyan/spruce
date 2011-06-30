//      ReadWriteFile.cpp
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

#include <sys/timerfd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <time.h>
#include <ReadWriteFile.hpp>
#include "File.hpp"

int ReadWriteFileTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case ReadBadFileDescriptor1:
				return ReadBadFileDescriptorTest1();
			case ReadBadFileDescriptor2:
				return ReadBadFileDescriptorTest2();
			case ReadEinvalError:
				return ReadEinvalErrorTest();
			case ReadIsdirError:
				return ReadIsdirErrorTest();
			case ReadEfaultError:
				return ReadEfaultErrorTest();
			case ReadEagainError:
				return ReadEagainErrorTest();
			case WriteBadFileDescriptor1:
				return WriteBadFileDescriptorTest1();
			case WriteBadFileDescriptor2:
				return WriteBadFileDescriptorTest2();
			case WriteEfaultError:
				return WriteEfaultErrorTest();
			case WriteEagainError:
				return WriteEagainErrorTest();
			case ReadWrite1:
				return ReadWriteTest1();
			case ReadWrite2:
				return ReadWriteTest2();

			default:
				cerr << "Unsupported operation.";
				return Unres;
		}
	}
	cerr << "Test was successful";
	return Success;
}

// attempt to read from a file, which was opened in write mode
Status ReadWriteFileTest::ReadBadFileDescriptorTest1()
{
	try
	{
		File file("testfile.txt", S_IWUSR);
		
		char buf[1024];
		size_t count = 10;

		size_t fd = open("testfile.txt", O_WRONLY);

		ssize_t status = read(fd, buf, count);
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

// attempt to read from -1 file descriptor
Status ReadWriteFileTest::ReadBadFileDescriptorTest2()
{
	char buf[1024];
	size_t count = 10;

	ssize_t status = read(-1, buf, count);
	if (errno != EBADF || status != -1)
	{
		cerr << "Expected to get bad file descriptor";
		return Fail;
	}
	
	return Success;
}

// attempt to read 0 bytes from file descriptor created by timerfd_create()
Status ReadWriteFileTest::ReadEinvalErrorTest()
{	
	int fd = timerfd_create(CLOCK_REALTIME, 0);
    if (fd == -1)
    {
		cerr << strerror(errno);
		return Unres;
	}
               
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
       
    struct itimerspec new_value;
    new_value.it_value.tv_sec = now.tv_sec;
    new_value.it_value.tv_nsec = now.tv_nsec;
    if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
	{
		cerr << strerror(errno);
		return Unres;
	}
    
    char buf[1024];
	size_t count = 0;
    ssize_t status = read(fd, buf, 0);
    
    if (errno != EINVAL || status != -1)
    {
		cerr << "Expecting to get EINVAL error";
		return Fail;
	}
	
	return Success;
}

// attempt to read from directory
Status ReadWriteFileTest::ReadIsdirErrorTest()
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
	
	char buf[1024];
	size_t count = 10;

	ssize_t status = read(fd, buf, count);
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

// attempt to read to the buffer which is -1
Status ReadWriteFileTest::ReadEfaultErrorTest()
{
	try
	{
		File file("testfile.txt");
		
		size_t count = 10;

		size_t fd = open("testfile.txt", O_RDONLY);

		ssize_t status = read(fd, (void *)-1, count);
				
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

// attempt to read from nonblocking pipe which is empty from the other side
Status ReadWriteFileTest::ReadEagainErrorTest()
{		
	int pipe[2];
	int status = pipe2(pipe, O_NONBLOCK);
	if (status == -1)
	{
		cerr << strerror(errno);
		return Unres;
	}
	
	char buf[1024];
	size_t count = 10;
	
	ssize_t st = read(pipe[0], buf, count);
	
	if (errno != EAGAIN || st != -1)
	{
		cerr << "Expected to get EAGAIN error";
		return Fail;
	}

	return Success;
}

// attempt to write to a file, which was opened in read mode
Status ReadWriteFileTest::WriteBadFileDescriptorTest1()
{
	try
	{
		File file("testfile.txt");
		
		string buf = "message";

		size_t fd = open("testfile.txt", O_RDONLY);

		ssize_t status = write(fd, buf.c_str(), buf.size());
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

// attempt to write to -1 file descriptor
Status ReadWriteFileTest::WriteBadFileDescriptorTest2()
{
	string buf = "message";
	
	ssize_t status = write(-1, buf.c_str(), buf.size());

	if (errno != EBADF || status != -1)
	{
		cerr << "Expected to get bad file descriptor";
		return Fail;
	}
	
	return Success;
}

// attempt to write to the buffer which is -1
Status ReadWriteFileTest::WriteEfaultErrorTest()
{
	try
	{
		File file("testfile.txt");
		
		size_t count = 10;

		size_t fd = open("testfile.txt", O_WRONLY);

		ssize_t status = write(fd, (void *)-1, count);
		
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

// attempt to write to nonblocking pipe which is full
// by default the pipe's size is 64 Kb
Status ReadWriteFileTest::WriteEagainErrorTest()
{
	int pipe[2];
	int status = pipe2(pipe, O_NONBLOCK);
	if (status == -1)
	{
		cerr << strerror(errno);
		return Unres;
	}
	
	int bytes = 64 * 1024 + 1;
	char *buf;
	memset(buf, '0', bytes);
		
	ssize_t st = write(pipe[1], buf, bytes);
	if (st == -1)
	{
		cerr << strerror(errno);
		free(buf);
		return Unres;
	}
	
	if (errno != EAGAIN || st != -1)
	{
		cerr << "Expected to get EAGAIN error";
		free(buf);
		return Fail;
	}

	free(buf);
	return Success;
}

// writes 'message' data to pipe, and reads from the other end of pipe the
// same data
Status ReadWriteFileTest::ReadWriteTest1()
{		
	int pipefd[2];
	int status = pipe(pipefd);
	if (status == -1)
	{
		cerr << "bad";
		cerr << strerror(errno);
		return Unres;
	}
	
	string buf = "message";		
	ssize_t st = write(pipefd[1], buf.c_str(), buf.size());
	if (st == -1)
	{
		cerr << "An error occured while writing data to pipe";
		close(pipefd[0]);
		close(pipefd[1]);
		return Fail;
	}
	
	char getBuf[1024];
	size_t count = 10;
	st = read(pipefd[0], getBuf, count);
	if (st == -1)
	{
		cerr << "An error occured while reading data from pipe";
		close(pipefd[0]);
		close(pipefd[1]);
		return Fail;
	}
	
	if (strncmp(getBuf, buf.c_str(), buf.size()) != 0)
	{
		cerr << "The data was written " << buf << ' ';
		cerr << "The data was read " << getBuf << ' ';
		close(pipefd[0]);
		close(pipefd[1]);
		return Fail;
	}
	
	close(pipefd[0]);
	close(pipefd[1]);
	return Success;
}

// writes 'message' data to pipe, and reads from the other end of pipe the
// same data with the fewer length, than it was written
Status ReadWriteFileTest::ReadWriteTest2()
{
	int bytesToRead = 6;
	int pipefd[2];
	int status = pipe(pipefd);
	if (status == -1)
	{
		cerr << "bad";
		cerr << strerror(errno);
		return Unres;
	}
	
	string buf = "large_message";
	ssize_t st = write(pipefd[1], buf.c_str(), bytesToRead);
	if (st == -1)
	{
		cerr << "An error occured while writing data to pipe";
		return Fail;
	}
	
	char getBuf[1024];
	size_t count = 10;
	st = read(pipefd[0], getBuf, count);
	if (st == -1)
	{
		cerr << "An error occured while reading data from pipe";
		return Fail;
	}
	
	if (strncmp(getBuf, buf.c_str(), bytesToRead) != 0)
	{
		cerr << "The data was written " << buf.substr(0, bytesToRead) << ' ';
		cerr << "The data was read " << getBuf << ' ';
		return Fail;
	}
	
	return Success;
}
