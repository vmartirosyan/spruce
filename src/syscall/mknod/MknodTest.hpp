//     MknodTest.hpp
//
// 		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//
//      Author:  Ani Tumanyan <ani.tumanyan92@gmail.com>
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

#ifndef MKNODTEST_H
#define MKNODTEST_H

#include "SyscallTest.hpp"

// Operations
/*enum MknodSyscalls
{
	MknodPathExists,
	MknodIsSymLink,
	MknodInvalidArg1,
	MknodInvalidArg2,
	MknodBadAdress,
	MknodTooLongPathName,
	MknodNotDir,
	MknodPermDenied,
	MknodNoFile1,
	MknodNoFile2,
	MknodNormalCase1
	MknodNormalCase2
	
};*/

class MknodTest : public SyscallTest
{			
public:	
	MknodTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "Mknod")
	{			
	}
protected:
	virtual ~MknodTest() {}	
	Status MknodTestPathExistsFunc();	
	Status MknodTestIsSymLinkFunc();	
	Status MknodTestInvalidArg1Func();  
	Status MknodTestInvalidArg2Func();
	Status MknodTestBadAdressFunc();
	Status MknodTestTooLongPathNameFunc();
	Status MknodTestNotDirFunc();
	Status MknodTestPermDeniedFunc();
	Status MknodTestNoFile1Func();
	Status MknodTestNoFile2Func();
	Status MknodTestNormalCase1Func();
	Status MknodTestNormalCase2Func();
	
	virtual int Main(vector<string> args);	
};
#endif
