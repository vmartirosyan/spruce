//      Test.hpp
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

#ifndef TEST_H
#define TEST_H

#include <TestResult.hpp>
#include "Common.hpp"
#include "Process.hpp"
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <map>
using namespace std;

class Test : public Process
{
public:	
	Test():
		_name("Unknown"),
		_description("None"),
		_func(0),
		_currentPoint("")
		{
		}
	Test(string name, string desc, ProcessFunc func = NULL, Checks supportedChecks = All):
		_name(name),
		_description(desc),
		_func(func),
		_supportedChecks(supportedChecks),
		_performChecks(Functional),
		_currentPoint("")
		{
			 //cerr << "Constructing test: " << _name << endl;
		}
		
	Test(const Test & t):
		_name(t._name),
		_description(t._description),
		_func(t._func),
		_supportedChecks(t._supportedChecks),
		_performChecks(Functional),
		_results(t._results),
		_currentPoint(t._currentPoint)
		{}
		
	string GetName() const
	{
		return _name;
	}
	string GetDescription() const
	{
		return _description;
	}
	bool operator < (const Test &t) const
	{
		return _name < t._name;
	}
	
	void SetChecks(Checks performChecks)
	{
		_performChecks = performChecks;
	}
	
	Checks GetEffectiveChecks() const
	{
		return ( _supportedChecks & _performChecks );
	}
	
	Checks GetSupportedChecks() const
	{
		return ( _supportedChecks  );
	}
	
	void AddResult(Checks c, ProcessResult t)
	{
		_results[c] = t;		
	}
	
	void ClearResults()
	{
		_results.erase(_results.begin(), _results.end());
	}
	
	string GetCurrentPoint() const
	{
		return _currentPoint;
	}
	void SetCurrentPoint(string point)
	{
		_currentPoint = point;
	}
	
	const map<Checks, TestResult>& GetResults() const
	{
		return _results;
	}
	
	ProcessResult * Execute(vector<string> args = vector<string>());
	virtual ~Test() 
	{
		
	}
	
	Status OopsChecker(string &);
	
protected:
	int Main(vector<string>)
	{
		Logger::LogFatal("Main method in Test class should be never called!");
		return -1;
	}
	string _name;
	string _description;
	ProcessFunc _func;
	Checks _supportedChecks;
	Checks _performChecks;
	string _currentPoint;
	map<Checks, TestResult> _results; //One result per check
};

typedef Status (*StartUpFunc) ();
typedef void (*CleanUpFunc) ();

class TestSet
{
public:
	TestSet(string name = "Undefined", Test tests[] = 0, int count = 0):
		_name(name),
		_StartUpFunc(0),
		_CleanUpFunc(0)
	{
		for ( int i = 0; i < count; ++i )
			AddTest(tests[i]);
	}

	Status Run(Checks);
	
	void AddTest(Test t)
	{
		//cerr << "Adding test: " << t.GetName() << endl;
		_tests[t.GetName()] = t;
	}
	
	string GetName() const
	{
		return _name;
	}
	
	void SetStartUpFunc(StartUpFunc _start)
	{
		_StartUpFunc = _start;
	}
	
	void SetCleanUpFunc(CleanUpFunc _clean)
	{
		_CleanUpFunc = _clean;
	}
	
	const std::map<string, Test>& GetTests() const
	{
		return _tests;
	}	
	
	bool operator < (const TestSet &t) const
	{
		return _name < t._name;
	}
	
	void Merge( TestSet & );
		
	~TestSet()
	{
	}	
private:
	string _name;
	std::map<string, Test> _tests;
	StartUpFunc _StartUpFunc;
	CleanUpFunc _CleanUpFunc;
};

class TestPackage
{
public:
	TestPackage():
		_testsets()
	{}
	
	TestPackage(string name, TestSet testsets[] = 0, int count = 0):
		_name(name)
	{
		for ( int i = 0; i < count; ++i )
			AddTestSet(testsets[i]);
	}

	Status Run(Checks);
	
	void AddTestSet(TestSet t)
	{
		//cerr << "Adding testset: " << t.GetName() << endl;
		_testsets[t.GetName()] = t;
	}
	
	string GetName() const
	{
		return _name;
	}
	
	void Merge( TestPackage & tp)
	{
		for ( map<string, TestSet>::iterator i = tp._testsets.begin(); i != tp._testsets.end(); ++i )
			_testsets[i->second.GetName()] = i->second;
	}
	
	const std::map<string, TestSet>& GetTestSets() const
	{
		return _testsets;
	}
	
	~TestPackage()
	{
	}	
private:
	string _name;
	std::map<string, TestSet> _testsets;
};



#endif /* TEST_H */
