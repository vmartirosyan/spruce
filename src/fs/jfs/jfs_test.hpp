//      jfs_test.hpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Vahram Martirosyan <vmartirosyan@gmail.com>
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

#ifndef JFS_TEST_H
#define JFS_TEST_H

#include <test.hpp>
#include <stdlib.h>
#include <sstream>
using std::stringstream;

class JFSTestResult : public TestResult
{
public:
	JFSTestResult(TestResult* tr, string syscallName):
		TestResult(*tr),  _syscallName(syscallName)
	{}
	virtual string ToXML()
	{
		
		stringstream str;
		str << rand();
		
		return "<JFS Name=\"" + _syscallName + "\" Id=\"" + str.str() + " \">" + TestResult::ToXML() + 
			"\n\t" +  "</JFS>";
	}
protected:
	string _syscallName;
};

class JFSTest : public Test
{
public:
	JFSTest(Mode m, int op, string a, string syscallName):
		Test(m, op, a), _syscallName(syscallName) 
	{
	}
	virtual JFSTestResult* Execute(vector<string> args)
	{
		TestResult* tr = (TestResult*)Test::Execute(args);						
		JFSTestResult* ext4fsTestResult = new JFSTestResult(tr, _syscallName);			
		delete tr;
		return ext4fsTestResult;			
	}
protected:
		string _syscallName;
};

#endif /* JFS_TEST_H */
