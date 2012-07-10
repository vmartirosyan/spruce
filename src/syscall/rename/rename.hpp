//      rename.hpp
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

#ifndef RENAME_H
#define RENAME_H

#include "SyscallTest.hpp"

using namespace std;

// Operations
/*enum RenameSyscalls
{
	RenameEfaultError1,
	RenameEfaultError2,
	RenameEfaultError3,
	RenameEbusyError,
	RenameEnametoolongError,
	RenameEisdirError,
	RenameEnotdirError,
	RenameGeneral1,
	RenameGeneral2,
	RenameHardLinks,
	RenameSoftLinks1,
	RenameSoftLinks2,
	RenameEnotempty,
	RenameEinval,
	proba
};*/

class RenameTest : public SyscallTest
{
public:	
	RenameTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "rename")
	{
		
	}
	virtual ~RenameTest()
	{
		
	}
	
	Status RenameEfaultError1Test();
	Status RenameEfaultError2Test();
	Status RenameEfaultError3Test();
	Status RenameEbusyErrorTest();
	Status RenameEnametoolongErrorTest();
	Status RenameEisdirErrorTest();
	Status RenameEnotdirErrorTest();
	Status RenameGeneralTest1();
	Status RenameGeneralTest2();
	Status RenameHardLinksTest();
	Status RenameSoftLinksTest1();
	Status RenameSoftLinksTest2();
	Status RenameEnotemptyTest();
	Status RenameEinvalTest();
	Status probaTest();
	
private:
	void DeleteDirectory(string path);
	
protected:
	virtual int Main(vector<string> args);
	
};
#endif
