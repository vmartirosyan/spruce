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

#ifndef TEST_BASE_H
#define TEST_BASE_H

#include "common.hpp"
#include "process.hpp"
#include "operations.hpp"
#include <sstream>
#include <fstream>
using namespace std;

class TestResult : public ProcessResult
{
public:
	/*TestResult():
		ProcessResult(Unknown, "No output"),
		_operation(0),
		_arguments("No arguments provided") {}*/
	TestResult(ProcessResult pr, int op, string args):
		ProcessResult(pr),
		_operation(op),
		_stroperation(OperationToString()),
		_arguments(args) {}
	TestResult(Status s, string output, int op, string args):
		ProcessResult((int)s, output),
		_operation(op),
		_stroperation(OperationToString()),
		_arguments(args) {}
	TestResult(ProcessResult pr, string op, string args):
		ProcessResult(pr),
		_stroperation(op),
		_arguments(args) {}
	TestResult(Status s, string output, string op, string args):
		ProcessResult((int)s, output),
		_stroperation(op),
		_arguments(args) {}
	TestResult(TestResult const & tr) : 
		ProcessResult(tr),
		_operation(tr._operation),
		_stroperation(tr._stroperation),
		_arguments(tr._arguments) {}
	 	 	 
	virtual string ToXML();
	virtual string OperationToString()
	{
		return Operation::ToString((Operations)_operation);
	}
protected:
	int _operation;
	string _stroperation;
	string _arguments;
	string StatusToString();
	
};

class TestResultCollection 
{
public:
	void AddResult(Status s, string output, int op, string args)
	{
		TestResult * tmp = new TestResult(s,output,op,args);
		_results.push_back(tmp);
	}
	void AddResult(TestResult * result)
	{
		_results.push_back(result);
	}
	string ToXML()
	{
		string result = "";
		for ( vector<TestResult *>::iterator i = _results.begin(); i != _results.end(); ++i )
		{			
			result += (*i)->ToXML();			
		}
		
		return result;
	}
	Status GetStatus()
	{
		for ( vector<TestResult *>::iterator i = _results.begin(); i != _results.end(); ++i )
			if ( (*i)->GetStatus() != Success )
				return Fail;
		return Success;
	}
	string GetOutput() const
	{
		return "asdf";
	}
	~TestResultCollection()
	{
		for ( vector<TestResult *>::iterator i = _results.begin(); i != _results.end(); ++i )
			delete (*i);
	}
private:
	vector<TestResult*> _results;
};

class Test : public Process
{
public:	
	Test(Mode m, int op, string a):
		_mode(m),
		_operation(op),
		_args(a) {}
	Test(Mode m, string op, string a):
		_mode(m),
		_stroperation(op),
		_args(a) {}
	Test(const Test & t):
		_mode(t._mode),
		_operation(t._operation),
		_stroperation(t._stroperation),
		_args(t._args) {}
	
	ProcessResult * Execute(vector<string> args = vector<string>());
	virtual ~Test() {}
protected:
	virtual int Main(vector<string> args) = 0;
	Mode _mode;
	int _operation;
	string _stroperation;
	string _args;
};

class TestCollection
{
public:
	TestResultCollection Run();
	void AddTest(Test * t)
	{
		_tests.push_back(t);
	}
	void Merge( TestCollection &);
	~TestCollection()
	{
		for ( vector<Test *>::iterator i = _tests.begin(); i != _tests.end(); ++i )
		{
			delete (*i);
		}
	}	
private:
	vector<Test *> _tests;
};

#endif /* TEST_BASE_H */
