//      DupFileDescriptor.hpp
//      
//		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//      	Narek Saribekyan <narek.saribekyan@gmail.com>
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

#ifndef DUP_FILE_DESCRIPTOR_H
#define DUP_FILE_DESCRIPTOR_H

#include "SyscallTest.hpp"
#include <sys/stat.h>

// Operations
enum DupFileDescriptorSyscalls
{
	Dup,
	Dup2,
	Dup3,
	DupErrEBADF,
	Dup2ErrEBADF,
	Dup3ErrEBADF,
	Dup3ErrEINVAL	
};

class DupFileDescriptorTest : public SyscallTest
{			
public:	
	DupFileDescriptorTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "dup")
	{			
	}
	virtual ~DupFileDescriptorTest() {}	
protected:
	virtual int Main(vector<string> args);			
private:
	int DupTest(vector<string> args);
	int Dup2Test(vector<string> args);
	int Dup3Test(vector<string> args);
	int DupErrEBADFTest(vector<string> args);
	int Dup2ErrEBADFTest(vector<string> args);
	int Dup2ErrEBUSYTest(vector<string> args);
	int Dup3ErrEBADFTest(vector<string> args);
	int Dup3ErrEINVALTest(vector<string> args);
	int Dup3ErrEBUSYTest(vector<string> args);
		
	bool statsEqual(struct stat stat1, struct stat stat2);
	bool cloexecFlagOff(int flag);
	bool statusFlagsEqual(int flag1, int flag2);
};
#endif /* DUP_TEST_H */
