//      getdents.cpp
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

#include <sys/stat.h>
#include <stdio.h>
#include <getdents.hpp>
#include <dirent.h>
#include <exception.hpp>
#include <File.hpp>
#include <sstream>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <time.h>

struct linux_dirent {
           long           d_ino;
           off_t          d_off;
           unsigned short d_reclen;
           char           d_name[];
       };
       
Status GetDentsTest::createDirWithFiles(string directory, int number)
{
	int st = mkdir(directory.c_str(), 0777);
	if (st == -1)
	{
		cerr << strerror(errno);
		return Unres;
	}
	
	st = chdir(directory.c_str());
	if (st == -1)
	{
		cerr << strerror(errno);
		rmdir(directory.c_str());
		return Unres;
	}
	
	for (int i = 0; i < number; ++i)
	{
		std::stringstream filename;
		filename << i;
		
		int st = creat(filename.str().c_str(), (mode_t)(S_IRUSR | S_IWUSR));
		if (st == -1)
		{
			cerr << strerror(errno);
			return Unres;
		}
	}
	
	chdir("../");
	
	return Success;
}

Status GetDentsTest::deleteDir(string dirName)
{
	int st = rmdir(dirName.c_str());
	if (st == -1)
	{
		cerr << strerror(errno);
		return Unknown;
	}
	return Success;
}

string GetDentsTest::randomName(int number)
{
	srand ( time(NULL) );
	
	string name = "";
	for (int i = 0; i < number; ++i)
	{
		int ch = rand() % 26 + 97;
		name += char(ch);
	}
	return name;
}

int GetDentsTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case GetDentsEbadfError:
				return GetDentsEbadfErrorTest();
			case GetDentsEfaultError:
				return GetDentsEfaultErrorTest();
			case GetDentsEinvalError:
				return GetDentsEinvalErrorTest();
			case GetDentsEnotdirError:
				return GetDentsEnotdirErrorTest();
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

// attempt to call getdents from a -1 file descriptor
Status GetDentsTest::GetDentsEbadfErrorTest()
{
	unsigned int count = 1024;
	char buf[1024];
	int status;
	char d_type;
		
	status = syscall(SYS_getdents, -1, buf, count);
	if (status != -1 || errno != EBADF)
	{
		cerr << strerror(errno);
		return Fail;
	}

	return Success;
}

// attempt to call getdents with -1 buffer
Status GetDentsTest::GetDentsEfaultErrorTest()
{
	int fd = open(directory.c_str(), O_RDONLY | O_DIRECTORY);
	if (fd == -1)
	{
		cerr << strerror(errno);
		return Unres;
	}
	
	unsigned int count = 1024;
	int status;
	char d_type;
		
	status = syscall(SYS_getdents, fd, -1, count);
	
	if (status != -1 || errno != EFAULT)
	{
		cerr << strerror(errno);
		return Fail;
	}

	return Success;
}

// attempt to call getdents with count = 1, small count is passed
Status GetDentsTest::GetDentsEinvalErrorTest()
{
	int fd = open(directory.c_str(), O_RDONLY | O_DIRECTORY);
	if (fd == -1)
	{
		cerr << strerror(errno);
		return Unres;
	}
	
	unsigned int count = 1024;
	int status;
	char d_type;
	char buf[1024];
		
	status = syscall(SYS_getdents, fd, buf, 1);
	
	if (status != -1 || errno != EINVAL)
	{
		cerr << strerror(errno);
		return Fail;
	}

	return Success;
}

// attempt to pass to getdents a descriptor of some file, not a directory
Status GetDentsTest::GetDentsEnotdirErrorTest()
{
	File file(randomName(5));
	int fd = file.GetFileDescriptor();
			
	unsigned int count = 1024;
	int status;
	char d_type;
	char buf[1024];
		
	status = syscall(SYS_getdents, fd, buf, 1);
	
	if (status != -1 || errno != ENOTDIR)
	{
		cerr << strerror(errno);
		return Fail;
	}

	return Success;
}

Status GetDentsTest::probaTest()
{
	linux_dirent *d;
	int fd = open(directory.c_str(), O_RDONLY | O_DIRECTORY);
	if (fd == -1)
	{
		cerr << strerror(errno);
		return Unres;
	}
	
	unsigned int count = 1024;
	char buf[1024];
	int nread;
	char d_type;
		
	nread = syscall(SYS_getdents, fd, buf, count);
	if (nread == -1)
	{
		cerr << strerror(errno);
		return Unres;
	}
	
	for (int bpos = 0; bpos < nread;) 
	{
		d = (struct linux_dirent *) (buf + bpos);
		cerr << d->d_ino << ' ';
		d_type = *(buf + bpos + d->d_reclen - 1);
		
		switch(d_type)
		{
			case DT_REG:
				cerr << "regular";
				break;
			case DT_DIR:
				cerr << "directory";
				break;
			case DT_FIFO:
				cerr << "FIFO";
				break;
			case DT_SOCK:
				cerr << "socket";
				break;
			case DT_LNK:
				cerr << "symlink";
				break;
			case DT_BLK:
				cerr << "block dev";
				break;
			case DT_CHR:
				cerr << "char dev";
				break;
		}
		
		cerr << (char *) d->d_name << ' ';
		bpos += d->d_reclen;
	}
	return Success;
}
