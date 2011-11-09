//     LseekTest.hpp
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

#ifndef LSEEKTEST_H
#define LSEEKTEST_H

#include "SyscallTest.hpp"

// Operations
enum LseekSyscalls
{
	LseekInvalidArg1,
	LseekInvalidArg2,
	LseekBadFileDesc1,
	LseekBadFileDesc2,
	LseekIllegalSeek1,
	LseekIllegalSeek2,
	LseekIllegalSeek3,
	LseekNormalCase1,
	LseekNormalCase2,
	Lseek64InvalidArg1,
	Lseek64InvalidArg2,
	Lseek64BadFileDesc1,
	Lseek64BadFileDesc2
	
	
};

class LseekTest : public SyscallTest
{			
public:	
	LseekTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "Lseek")
	{			
	}
protected:
	virtual ~LseekTest() {}	
	Status LseekTestInvalidArg1Func();	
	Status LseekTestInvalidArg2Func();	
	Status LseekTestBadFileDesc1Func();
	Status LseekTestBadFileDesc2Func();
	Status LseekTestIllegalSeek1Func();
	Status LseekTestIllegalSeek2Func();
	Status LseekTestIllegalSeek3Func();
	Status LseekTestNormalCase1Func();
	Status LseekTestNormalCase2Func();
	Status Lseek64TestInvalidArg1Func();
	Status Lseek64TestInvalidArg2Func();
	Status Lseek64TestBadFileDesc1Func();
	Status Lseek64TestBadFileDesc2Func();
	
	virtual int Main(vector<string> args);	
};
#endif
