//      Readlink.cpp
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

#include <Readlink.hpp>
#include "File.hpp"

int ReadlinkTest:: Main(vector<string> args)
{
	if (_mode == Normal)
	{		
		switch (_operation)
		{					
			case Readlink:
				return ReadlinkSyscallTest(args);
			case ReadlinkErrEINVAL:
				return ReadlinkErrEINVALTest(args);
			case ReadlinkErrENOTDIR:
				return ReadlinkErrENOTDIRTest(args);
			case ReadlinkErrENOENT:
				return ReadlinkErrENOENTTest(args);
			case Readlinkat:
				return ReadlinkatSyscallTest(args);
			case ReadlinkatErrEINVAL:
				return ReadlinkatErrEINVALTest(args);
			case ReadlinkatErrEBADF:
				return ReadlinkatErrEBADFTest(args);
			case ReadlinkatErrENOTDIR:
				return ReadlinkatErrENOTDIRTest(args);
			case ReadlinkatErrENOENT:
				return ReadlinkatErrENOENTTest(args);
			case Symlink:
				return SymlinkTest(args);
			case SymlinkErrEACCES:
				return SymlinkErrEACCESTest(args);
			case SymlinkErrEEXIST:
				return SymlinkErrEEXISTTest(args);
			case SymlinkErrENOENT:
				return SymlinkErrENOENTTest(args);
			case SymlinkErrENOTDIR:
				return SymlinkErrENOTDIRTest(args);
			default:
				return Unknown; 
		}
	}
	else
	{		
	}
	return 0;
}

int ReadlinkTest:: ReadlinkSyscallTest(vector<string> args)
{
	try
	{
		const char * slink = "slinkfile";
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		if(symlink(file.GetPathname().c_str(), slink) != 0 && errno != EEXIST)
		{
			cerr << "System call symlink failed: " << strerror(errno);
			return Unres;
		}
		
		char * buff = new char[10];
		int len = 4;		
				
		if(readlink(slink, buff, len) != file.GetPathname().length())
		{
			cerr << "System call readlink failed: " << strerror(errno);
			return Fail; 
		}
				
	}
	catch(Exception e)
	{
		cerr << e.GetMessage();
		return Unres;
	}
	return Success;
}

int ReadlinkTest:: ReadlinkErrEINVALTest(vector<string> args)
{
	int status = Success;
	try
	{
		const char * slink = "slinkfile1";
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		if(symlink(file.GetPathname().c_str(), slink) != 0  && errno != EEXIST)
		{
			cerr << "System call symlink failed: " << strerror(errno);
			status = Unres;
		}		
		char * buff = new char[10];
		int len = -1;		
		if(readlink(slink, buff, len) != -1 || errno != EINVAL)
		{
			cerr << "EINVAL error expected: length is negative";
			status = Fail; 
		}
		
		if(readlink(file.GetPathname().c_str(), buff, 3) != -1 || errno != EINVAL)
		{
			cerr << "EINVAL error expected: named file is not a symbolic link";
			status = Fail; 
		}		
	}
	catch(Exception e)
	{
		cerr << e.GetMessage();
		return Unres;
	}	
	return status;
}

int ReadlinkTest:: ReadlinkErrENOTDIRTest(vector<string> args)
{
	try
	{
		const char * slink = "slinkfile";
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		if(symlink(file.GetPathname().c_str(), slink) != 0 && errno != EEXIST)
		{
			cerr << "System call symlink failed: " << strerror(errno);
			return Unres;
		}
		
		char * buff = new char[10];
		int len = 4;		
		
		if(readlink("file/slinkfile", buff, len) != -1 && errno != ENOTDIR)
		{
			cerr << "ENOTDIR error expected: a component of a path prefix is not a directory" << strerror(errno);
			return Fail; 
		}		
	}
	catch(Exception e)
	{
		cerr << e.GetMessage();
		return Unres;
	}
	return Success;
}

int ReadlinkTest:: ReadlinkErrENOENTTest(vector<string> args)
{
	char * buff = new char[10];
	if(readlink("file1", buff, 1) != -1 || errno != ENOENT)
	{
		cerr << "ENOENT error expected: the named file doesn't exist";
		return Fail;
	}	
	return Success;
}

int ReadlinkTest:: ReadlinkatSyscallTest(vector<string> args)
{
	const char * dirPathname = "readlinkatFolder";
	if(mkdir(dirPathname, (mode_t)(S_IRUSR | S_IWUSR | S_IXUSR)) == -1 && errno != EEXIST)	
	{
		cerr << "System call mkdir failed: " << strerror(errno);
		return Unres;
	}
	
	int dirfd = open(dirPathname, 0);
	if(dirfd == -1)
	{
		cerr << "System call open failed: " << strerror(errno);
		return Unres; 
	}
	
	try
	{
		const char * slink = "readlinkatFolder/slinkfile2";
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		if(symlink(file.GetPathname().c_str(), slink) != 0  && errno != EEXIST)
		{
			cerr << "System call symlink failed: " << strerror(errno);
			return Unres;
		}	
		
		char * buff = new char[10];
		int len = 4;
		if(readlinkat(dirfd, "slinkfile2", buff, len) == -1)
		{
			cerr << "System call readlinkat failed: " << strerror(errno);
			return Fail;
		}		
	}
	catch(Exception e)
	{
		cerr << e.GetMessage();
		return Unres;
	}
	return Success;
}

int ReadlinkTest:: ReadlinkatErrEINVALTest(vector<string> args)
{
	const char * dirPathname = "readlinkatFolder";
	if(mkdir(dirPathname, (mode_t)(S_IRUSR | S_IWUSR | S_IXUSR)) == -1 && errno != EEXIST)	
	{
		cerr << "System call mkdir failed: " << strerror(errno);
		return Unres;
	}
	
	int dirfd = open(dirPathname, 0);
	if(dirfd == -1)
	{
		cerr << "System call open failed: " << strerror(errno);
		return Unres; 
	}
	
	try
	{
		const char * slink = "readlinkatFolder/slinkfile2";
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		if(symlink(file.GetPathname().c_str(), slink) != 0  && errno != EEXIST)
		{
			cerr << "System call symlink failed: " << strerror(errno);
			return Unres;
		}	
		
		char * buff = new char[10];
		int len = -1;
		if(readlinkat(dirfd, "slinkfile2", buff, len) != -1 && errno != EINVAL)
		{
			cerr << "EINVAL error expected: length is negative";
			return Fail;
		} 	
		
		len = 4;
		if(readlinkat(dirfd, file.GetPathname().c_str(), buff, len) != -1 && errno != EINVAL)
		{
			cerr << "EINVAL error expected: named file is not a symbolic link";
			return Fail;
		} 			
	}
	catch(Exception e)
	{
		cerr << e.GetMessage();
		return Unres;
	}
	return Success;
}

int ReadlinkTest:: ReadlinkatErrEBADFTest(vector<string> args)
{	
	try
	{
		int dirfd = -1;
		const char * slink = "slinkfile2";
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		if(symlink(file.GetPathname().c_str(), slink) != 0  && errno != EEXIST)
		{
			cerr << "System call symlink failed: " << strerror(errno);
			return Unres;
		}	
		
		char * buff = new char[10];
		int len = 4;
		if(readlinkat(dirfd, "slinkfile2", buff, len) != -1 && errno != EBADF)
		{
			cerr << "EBADF error expected: directory fd is invalid " << strerror(errno);
			return Fail;
		} 	
		
	}
	catch(Exception e)
	{
		cerr << e.GetMessage();
		return Unres;
	}
	return Success;
}

int ReadlinkTest:: ReadlinkatErrENOTDIRTest(vector<string> args)
{	
	try
	{
		const char * slink = "slinkfile2";
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		int dirfd = file.GetFileDescriptor();
		if(symlink(file.GetPathname().c_str(), slink) != 0  && errno != EEXIST)
		{
			cerr << "System call symlink failed: " << strerror(errno);
			return Unres;
		}	
		
		char * buff = new char[10];
		int len = 4;
		if(readlinkat(dirfd, "slinkfile2", buff, len) != -1 && errno != ENOTDIR)
		{
			cerr << "ENOTDIR error expected: pathname is relative and dirfd referres to file other than a directory";
			return Fail;
		} 	
		
	}
	catch(Exception e)
	{
		cerr << e.GetMessage();
		return Unres;
	}
	return Success;
}

int ReadlinkTest:: ReadlinkatErrENOENTTest(vector<string> args)
{
	const char * dirPathname = "readlinkatFolder";
	if(mkdir(dirPathname, (mode_t)(S_IRUSR | S_IWUSR | S_IXUSR)) == -1 && errno != EEXIST)	
	{
		cerr << "System call mkdir failed: " << strerror(errno);
		return Unres;
	}
	
	int dirfd = open(dirPathname, 0);
	if(dirfd == -1)
	{
		cerr << "System call open failed: " << strerror(errno);
		return Unres; 
	}
	
	char * buff = new char[10];
	int len = 4;
	if(readlinkat(dirfd, "file1", buff, len) != -1 && errno != ENOENT)
	{
		cerr << "ENOENT error expected: the named file doesn't exist";
		return Fail;
	}
	
	return Success;
}

int ReadlinkTest:: SymlinkTest(vector<string> args)
{
	try
	{
		const char * slink = "slinkfile3";
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		if(symlink(file.GetPathname().c_str(), slink) != 0  && errno != EEXIST)
		{
			cerr << "System call symlink failed: " << strerror(errno);
			return Fail;
		}	
	}
	catch(Exception e)
	{
		cerr << e.GetMessage();
		return Unres;
	}
	return Success;
}

int ReadlinkTest:: SymlinkErrEACCESTest(vector<string> args)
{
	const char * dirPathname = "symlinkFolder";
	if(mkdir(dirPathname, (mode_t)(S_IRUSR)) == -1 && errno != EEXIST)	
	{
		cerr << "System call mkdir failed: " << strerror(errno);
		return Unres;
	}
	try
	{
		const char * slink = "symlinkFolder/slinkfile2";
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		if(symlink(file.GetPathname().c_str(), slink) != -1 || errno != EACCES)
		{
			cerr << "EACCES error expected: write access to the directory containing slink is denied";
			return Fail;
		}	
	}
	catch(Exception e)
	{
		cerr << e.GetMessage();
		return Unres;
	}
	return Success;
}

int ReadlinkTest:: SymlinkErrEEXISTTest(vector<string> args)
{
	try
	{
		const char * slink = "slinkfile";
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		if(symlink(file.GetPathname().c_str(), slink) != 0  && errno != EEXIST)
		{
			cerr << "System call symlink failed: " << strerror(errno);
			return Unres;
		}	
		
		if(symlink(file.GetPathname().c_str(), slink) != -1 || errno != EEXIST)
		{
			cerr << "EEXIST error expected: slink already exists";
			return Fail;
		}
	}
	catch(Exception e)
	{
		cerr << e.GetMessage();
		return Unres;
	}	
	return Success;
}

int ReadlinkTest:: SymlinkErrENOENTTest(vector<string> args)
{
	int status = Success;
	try
	{
		const char * slink = "dir/slinkfile";
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		if(symlink(file.GetPathname().c_str(), slink) != -1  && errno != ENOENT)
		{
			cerr << "ENOENT error expected: a directory component in newpath does not exist";
			status = Fail;
		}
		
		if(symlink("", "slinkfile") != -1 || errno != ENOENT)
		{
			cerr << "ENOENT error expected: oldpath is empty string";
			status = Fail;
		}
	}
	catch(Exception e)
	{
		cerr << e.GetMessage();
		return Unres;
	}	
	return status;
}

int ReadlinkTest:: SymlinkErrENOTDIRTest(vector<string> args)
{
	try
	{
		File file("file", S_IRUSR | S_IWUSR, O_RDWR);
		if(symlink(file.GetPathname().c_str(), "file/slinkfile") != -1  && errno != ENOTDIR)
		{
			cerr << "ENOTDIR error expected: a component used as a directory in newpath is not a directory";
			return Fail;
		}	
	}
	catch(Exception e)
	{
		cerr << e.GetMessage();
		return Unres;
	}
	return Success;
}
