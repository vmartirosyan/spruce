//      BenchmarkTest.hpp
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


#ifndef BENCHMARK_TEST_HPP
#define BENCHMARK_TEST_HPP

#include <sys/time.h>
#include "test.hpp"
#include "BenchmarkTestResult.hpp"

class BenchmarkTest : public Test
{
public:
	BenchmarkTest(Mode m, int op, string a) : Test (m, op, a)
	{
	}
	
	virtual ~BenchmarkTest()
	{
	}
	
	ProcessResult* Execute(vector<string>)
	{
		struct timeval start, end;
		double seconds, useconds;
		double mtime;
				
		gettimeofday(&start, NULL);
		
		ProcessResult * p_res = Process::Execute();
		
		gettimeofday(&end, NULL);
		seconds  = end.tv_sec  - start.tv_sec;
		useconds = end.tv_usec - start.tv_usec;
		mtime = seconds + useconds / 1000000.0;

		BenchmarkTestResult *bench_test_res = new BenchmarkTestResult(*p_res, _operation, _args, mtime);

		delete p_res;

		return bench_test_res;
	}
	
	vector<string> ParseArguments(string args)
	{
		vector<string> arguments;
		
		while(args.size() != 0)
		{
			int position = args.find(" ");
			if (position == -1)
			{
				string s = args.substr(0, args.size());
				if (s.size() > 0)
					arguments.push_back(s);
				args = "";
			}
			else
			{
				string s = args.substr(0 , position);
				if (s.size() > 0)
					arguments.push_back(s);
				args = args.substr(position + 1, args.size());
			}
		}
		
		return arguments;
	}

protected:
	virtual int Main(vector<string>) = 0;
};

#endif
