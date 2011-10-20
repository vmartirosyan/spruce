//     Select.hpp
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

#ifndef SELECT_H
#define SELECT_H

#include "SyscallTest.hpp"

// Operations
enum SelectSyscalls
{
	SelectInvalidArg1,
	SelectInvalidArg2,
	SelectBadFileDesc1,
	SelectBadFileDesc2,
	SelectNormalCase1,
	SelectNormalCase2,
	SelectNormalCase3,
	SelectNormalPipeCase
	
};

class SelectTest : public SyscallTest
{			
public:	
	SelectTest(Mode mode, int operation, string arguments = "") :
		SyscallTest(mode, operation, arguments, "Select")
	{			
	}
protected:
	virtual ~SelectTest() {}	
	Status SelectTestInvalidArg1Func();	
	Status SelectTestInvalidArg2Func();
	Status SelectTestBadFileDesc1Func();
	Status SelectTestBadFileDesc2Func();
	Status SelectTestNormalCase1Func();
	Status SelectTestNormalCase2Func();
	Status SelectTestNormalCase3Func();
	Status SelectTestNormalPipeCaseFunc();
	
	
	virtual int Main(vector<string> args);	
};
#endif

