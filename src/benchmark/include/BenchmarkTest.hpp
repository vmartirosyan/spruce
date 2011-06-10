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
	
	ProcessResult* Execute()
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
	
protected:
	virtual Status Main(vector<string>) = 0;
};

#endif
