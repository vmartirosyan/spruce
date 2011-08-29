//      SyscallTest.hpp
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

#ifndef SYSCALL_TEST_H
#define SYSCALL_TEST_H

#include <test.hpp>

class SyscallTestResult : public TestResult 
{
	public:
		SyscallTestResult(TestResult* tr, string syscallName) :	
		TestResult(*tr), _syscallName(syscallName)		
		{						
		}
		virtual string ToXML()
		{						
			return TestResult::ToXML() + " " + _syscallName;
		}
	protected:
		string _syscallName;
};

class SyscallTest : public Test
{
	public:		
		SyscallTest(Mode mode, int operation, string arguments, string syscallName) : 
		Test(mode, operation, arguments), _syscallName(syscallName) 
		{
		}														
		
		string GetSyscallName() const 
		{
			return _syscallName;
		}				
		
		virtual SyscallTestResult* Execute(vector<string> args)
		{
			TestResult* tr = (TestResult*)Test::Execute(args);						
			SyscallTestResult* syscallTestResult = new SyscallTestResult(tr, _syscallName);			
			delete tr;
			return syscallTestResult;			
		}
	protected:
		string _syscallName;
};
#endif /* SYSCALL_TEST_H */
