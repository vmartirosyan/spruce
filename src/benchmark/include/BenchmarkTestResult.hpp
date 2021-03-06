//      BenchmarkTestResult.hpp
//      
// 		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//
//      Authors: Eduard Bagrov <ebagrov@gmail.com>
//				 Tigran Piloyan <tigran.piloyan@gmail.com>			
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

#ifndef BENCHMARK_TEST_RESULT_HPP
#define BENCHMARK_TEST_RESULT_HPP

#include <stdio.h>
#include "Test.hpp"

class BenchmarkTestResult : public TestResult
{
public:
	
	BenchmarkTestResult(ProcessResult pr, int op, string args, double time): TestResult(pr, op, args)
	{
		timestamp = time;
	}
	BenchmarkTestResult(Status s, string output, int op, string args, double time) : TestResult (s, output, op, args)
	{
		timestamp = time;
	}
	
	virtual ~BenchmarkTestResult()
	{
	}
	
	virtual string ToXML()
	{
		char strToTime[100];
		sprintf(strToTime, "%.2f", timestamp);
		
		return TestResult::ToXML() + "\n\t<Time>" + static_cast<string>(strToTime) + "</Time>";
	
		//return Operation::ToString(static_cast<Operations>(_operation)) + " : " + 
		//		StatusToString() + " : " + _output + " : " + _arguments + " : " + static_cast<string>(strToTime);
	}
protected:
	double timestamp;
};

#endif
