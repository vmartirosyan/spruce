//      ReadLink.hpp
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

#ifndef READLINK_H
#define READLINK_H

#include "SyscallTest.hpp"
#include <sys/stat.h>

// Operations 
/*enum ReadlinkSyscalls
{	
	Readlink,
	ReadlinkErrEINVAL,
	ReadlinkErrENOTDIR,
	ReadlinkErrENOENT,
	Readlinkat,
	ReadlinkatErrEINVAL,
	ReadlinkatErrEBADF,
	ReadlinkatErrENOTDIR,
	ReadlinkatErrENOENT,
	Symlink,
	SymlinkErrEACCES,
	SymlinkErrEEXIST,
	SymlinkErrENOENT,
	SymlinkErrENOTDIR
};*/

class ReadlinkTest: public SyscallTest
{
public:
	ReadlinkTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "readlink")
	{			
	}
	
	virtual ~ReadlinkTest() {}
	
protected:
	virtual int Main(vector<string> args);
	
private:
	int ReadlinkSyscallTest(vector<string> args);
	int ReadlinkErrEINVALTest(vector<string> args);
	int ReadlinkErrENOTDIRTest(vector<string> args);
	int ReadlinkErrENOENTTest(vector<string> args);
	int ReadlinkatSyscallTest(vector<string> args);
	int ReadlinkatErrEINVALTest(vector<string> args);
	int ReadlinkatErrEBADFTest(vector<string> args);
	int ReadlinkatErrENOTDIRTest(vector<string> args);
	int ReadlinkatErrENOENTTest(vector<string> args);
	int SymlinkTest(vector<string> args);
	int SymlinkErrEACCESTest(vector<string> args);
	int SymlinkErrEEXISTTest(vector<string> args);
	int SymlinkErrENOENTTest(vector<string> args);
	int SymlinkErrENOTDIRTest(vector<string> args);
};

#endif /*READLINK_H*/
