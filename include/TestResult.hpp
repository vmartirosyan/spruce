//      TestResult.hpp
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

#ifndef TEST_RESULT_H
#define TEST_RESULT_H

#include "Common.hpp"
#include "Process.hpp"
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <map>
using namespace std;

class TestResult : public ProcessResult
{
public:
	TestResult():
		ProcessResult(Unknown, "No output")
		{}
		
	
	TestResult(ProcessResult pr):
		ProcessResult(pr)		
		 {}
		
	TestResult(Status s, string output):
		ProcessResult(s, output)
		{}
	
	TestResult(TestResult const & tr) : 
		ProcessResult(tr)
		 {}
	 	 	 
};

/*class TestSetResult
{
public:
	TestSetResult(string name):
		_name(name),
		_results()
		{}
	void AddResult(Status s, string output, const Test * t)
	{		
		_results.push_back(TestResult (s, output, t));
	}
	void AddResult(TestResult result)
	{
		_results.push_back(result);
	}
	
	const vector<TestResult> & GetTestResults() const
	{
		return _results;
	}

	
	// Returns the highest status in the set.
	Status GetStatus()
	{
		Status max_stat = Success;
		for ( vector<TestResult>::iterator i = _results.begin(); i != _results.end(); ++i )
		{			
			if ( (*i).GetStatus() > max_stat && (*i).GetStatus() >= Unresolved )
				max_stat = (*i).GetStatus();
				
		}
		return max_stat;
	}
	string GetOutput() const
	{
		return "TestSetResult Output";
	}
	string GetName() const
	{
		return _name;
	}
	~TestSetResult()
	{

	}
	const vector<TestResult>& GetResults() const
	{
		return _results;
	}
private:
	string _name;
	vector<TestResult> _results;
};

class TestPackageResult
{
public:
	TestPackageResult(string name):
		_name(name),
		_results()
		{}
	void AddResult(TestSetResult res)
	{		
		_results.push_back(res);
	}
	// Returns the highest status in the set.
	Status GetStatus()
	{
		Status max_stat = Success;
		for ( vector<TestSetResult>::iterator i = _results.begin(); i != _results.end(); ++i )
		{			
			if ( (*i).GetStatus() > max_stat && (*i).GetStatus() >= Unresolved )
				max_stat = (*i).GetStatus();
				
		}
		return max_stat;
	}
	
	const vector<TestSetResult> & GetTestSetResults() const
	{
		return _results;
	}
	
	string GetOutput() const
	{
		return "asdf";
	}
	string GetName() const
	{
		return _name;
	}
	~TestPackageResult()
	{

	}
	const vector<TestSetResult>& GetResults() const
	{
		return _results;
	}
private:
	string _name;
	vector<TestSetResult> _results;
};*/

#endif /* defined TEST_RESULT_H */
