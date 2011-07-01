//      ReadWriteFile.hpp
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

#ifndef READ_FILE_H
#define READ_FILE_H

#include "SyscallTest.hpp"
#include "File.hpp"

// Operations
enum ReadFileDescriptorSyscalls
{
	ReadBadFileDescriptor1,
	ReadBadFileDescriptor2,
	ReadEinvalError,
	ReadIsdirError,
	ReadEfaultError,
	ReadEagainError,
	WriteBadFileDescriptor1,
	WriteBadFileDescriptor2,
	WriteEfaultError,
	WriteEagainError,
	ReadWrite1,
	ReadWrite2
};

class ReadWriteFileTest : public SyscallTest
{			
public:	
	ReadWriteFileTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "read_write")
	{
		
	}
	virtual ~ReadWriteFileTest() 
	{		
	}	
	Status ReadBadFileDescriptorTest1();
	Status ReadBadFileDescriptorTest2();
	Status ReadEinvalErrorTest();
	Status ReadIsdirErrorTest();	
	Status ReadEfaultErrorTest();
	Status ReadEagainErrorTest();
	Status WriteBadFileDescriptorTest1();
	Status WriteBadFileDescriptorTest2();
	Status WriteEfaultErrorTest();
	Status WriteEagainErrorTest();
	Status ReadWriteTest1();
	Status ReadWriteTest2();
protected:
	virtual int Main(vector<string> args);

};
#endif
