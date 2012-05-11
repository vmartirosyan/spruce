//      test.hpp
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

#include "test.hpp"
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <algorithm>

char * StatusMessages[] = {
	(char * )"Success",
	(char * )"Failed",
	(char * )"Unresolved",
	(char * )"Timeout",
	(char * )"Signaled",
	(char * )"Unsupported",
	(char * )"Unknown"
	};
	
ProcessResult * Test::Execute(vector<string> args)
{
	ProcessResult * p_res = Process::Execute(args);
	
	TestResult * t_res = new TestResult(*p_res, _operation, _args);
	
	delete p_res;
	
	return t_res;
}

void TestCollection::Merge(TestCollection & tc)
{
	//for ( vector<Test *>::iterator i = tc._tests.begin(); i != tc._tests.end(); ++i)		
	for ( unsigned int i = 0; i < tc._tests.size(); )
	{
		_tests.push_back(tc._tests[i]);
		// Erase the original pointer so that the destructors do not overlap
		tc._tests.erase(tc._tests.begin() + i);
	}
}

TestResultCollection TestCollection::Run()
{
	TestResultCollection Results;
		
	for ( unsigned int index = 0 ; index < _tests.size(); ++index)
	{
		Results.AddResult( (TestResult *)_tests[index]->Execute() );
		//delete _tests[index];
	}	
	
	return Results;
}

string TestResult::StatusToString()
{ 
	if ( _status >= Success && _status <= Unknown )
		return (string)StatusMessages[_status];
	else
		return (string)StatusMessages[Unknown];
}

string TestResult::ToXML()
{
	// Just for debugging
	// return OperationToString() + " : " + StatusToString() + " : " + _output + " : Arguments: " + _arguments;
	// The real XML...
	return "\n\t<Operation>" + OperationToString() + "</Operation>\n\t<Status>" + StatusToString() + "</Status>\n\t<Output>" +
		_output +  "</Output>\n\t<Arguments>" + _arguments + "</Arguments>";
	
	//return _arguments;
	
}
